/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "Map2CheckLibrary.hpp"

#include <memory>

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/CommandLine.h>

using llvm::CallInst;
using llvm::dyn_cast;
using llvm::IRBuilder;
using llvm::ReturnInst;
using llvm::Twine;

static llvm::cl::opt<std::string> EntryFunction(
    "entry-function",
    llvm::cl::desc("Name of the entry function (default: main)"),
    llvm::cl::init("main"));

PreservedAnalyses Map2CheckLibrary::run(Function& F,
                                        llvm::FunctionAnalysisManager& AM) {
  this->libraryFunctions = make_unique<LibraryFunctions>(&F, &F.getContext());
  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*instructionIterator));
  this->functionName = builder.CreateGlobalStringPtr(F.getName());
  bool isEntry = false;
  if (F.getName() == EntryFunction) {
    currentInstruction = instructionIterator;
    instrumentStartInstruction(&F.getContext());
    isEntry = true;
  }

  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      currentInstruction = i;
      if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
        this->runOnCallInstruction(callInst, &F.getContext());
      }
      if (ReturnInst* inst = dyn_cast<ReturnInst>(&*i)) {
        if (isEntry) this->instrumentReleaseInstruction(&F.getContext());
      }
    }
  }

  if (F.getName() == EntryFunction) {
    Twine new_entry_name("__map2check_main__");
    F.setName(new_entry_name);
  }

  return PreservedAnalyses::none();
}

void Map2CheckLibrary::instrumentStartInstruction(LLVMContext* Ctx) {
  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*currentInstruction));
  Value* argument =
      ConstantInt::getSigned(Type::getInt32Ty(*Ctx), this->SVCOMP ? 1 : 0);
  Value* args[] = {argument};
  builder.CreateCall(this->libraryFunctions->getInitFunction(), args);
}

void Map2CheckLibrary::instrumentReleaseInstruction(LLVMContext* Ctx) {
  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*currentInstruction));
  builder.CreateCall(this->libraryFunctions->getExitFunction());
}

void Map2CheckLibrary::runOnCallInstruction(CallInst* callInst,
                                            LLVMContext* Ctx) {
  Function* calleeFunction = callInst->getCalledFunction();

  if (calleeFunction == NULL) {
    Value* v = callInst->getCalledOperand();
    calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if (calleeFunction == NULL) {
      return;
    }
  }

  if (calleeFunction->getName() == "exit") {
    this->instrumentReleaseInstruction(Ctx);
  } else if (calleeFunction->getName() == "abort") {
    this->instrumentReleaseInstruction(Ctx);
  }
}

// --- New Pass Manager plugin registration ---
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Map2CheckLibrary", LLVM_VERSION_STRING,
          [](llvm::PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager& FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "map2check-library") {
                    FPM.addPass(Map2CheckLibrary());
                    return true;
                  }
                  return false;
                });
          }};
}
