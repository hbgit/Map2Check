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

#include "RuntimeABI.hpp"

using llvm::CallInst;
using llvm::dyn_cast;
using llvm::IRBuilder;
using llvm::ReturnInst;
using llvm::Twine;

static llvm::cl::opt<std::string> EntryFunction(
    "entry-function",
    llvm::cl::desc("Name of the entry function (default: main)"),
    llvm::cl::init("main"));

// Values of map2check-library's AnalysisMode (map2check_set_mode). The
// frontend passes the mode of the run; the C runtime got it from the
// AnalysisMode*.bc it linked, the Rust runtime is one library for every mode.
static llvm::cl::opt<Map2CheckLibrary::AnalysisMode> AnalysisModeOption(
    "m2c-analysis-mode",
    llvm::cl::desc("Analysis mode passed to map2check_set_mode"),
    llvm::cl::values(
        clEnumValN(Map2CheckLibrary::AnalysisMode::None, "none",
                   "no property (cover-branches)"),
        clEnumValN(Map2CheckLibrary::AnalysisMode::Memtrack, "memtrack",
                   "memory safety"),
        clEnumValN(Map2CheckLibrary::AnalysisMode::Memcleanup, "memcleanup",
                   "memory cleanup"),
        clEnumValN(Map2CheckLibrary::AnalysisMode::Overflow, "overflow",
                   "signed integer overflow"),
        clEnumValN(Map2CheckLibrary::AnalysisMode::Assert, "assert",
                   "__VERIFIER_assert"),
        clEnumValN(Map2CheckLibrary::AnalysisMode::Target, "target",
                   "reachability of the target function")),
    llvm::cl::init(Map2CheckLibrary::AnalysisMode::None));

Map2CheckLibrary::Map2CheckLibrary() : mode(AnalysisModeOption) {}

PreservedAnalyses Map2CheckLibrary::run(Function& F,
                                        llvm::FunctionAnalysisManager& AM) {
  llvm::Module& M = *F.getParent();
  // Normally done by NonDetPass already; idempotent.
  Map2Check::resolveProgramDefinitions(M);
  this->setModeFunction =
      Map2Check::getOrInsertRuntime(M, Map2Check::RuntimeHook::SetMode);
  this->initFunction =
      Map2Check::getOrInsertRuntime(M, Map2Check::RuntimeHook::Init);
  this->successFunction =
      Map2Check::getOrInsertRuntime(M, Map2Check::RuntimeHook::Success);
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
  // map2check_set_mode(mode) must precede map2check_init(); init no longer
  // takes the dead isSvComp argument of the C runtime.
  Value* mode = ConstantInt::getSigned(Type::getInt32Ty(*Ctx),
                                       static_cast<int>(this->mode));
  Value* args[] = {mode};
  builder.CreateCall(this->setModeFunction, args);
  builder.CreateCall(this->initFunction);
}

void Map2CheckLibrary::instrumentReleaseInstruction(LLVMContext* Ctx) {
  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*currentInstruction));
  builder.CreateCall(this->successFunction);
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
