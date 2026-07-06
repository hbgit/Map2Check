/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "AssertPass.hpp"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using llvm::dyn_cast;
using llvm::IRBuilder;
using llvm::FunctionCallee;
using llvm::PassPluginLibraryInfo;

PreservedAnalyses AssertPass::run(Function& F,
                                  llvm::FunctionAnalysisManager& AM) {
  this->map2check_assert = F.getParent()->getOrInsertFunction(
      "map2check_assert", Type::getVoidTy(F.getContext()),
      Type::getInt32Ty(F.getContext()), Type::getInt32Ty(F.getContext()),
      PointerType::get(F.getContext(), 0));

  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*instructionIterator));
  this->functionName = builder.CreateGlobalStringPtr(F.getName());

  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      currentInstruction = i;
      if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
        this->runOnCallInstruction(callInst, &F.getContext());
      }
    }
  }
  return PreservedAnalyses::none();
}

void AssertPass::instrumentAssert(CallInst* assertInst, LLVMContext* Ctx) {
  IRBuilder<> builder(assertInst);
  DebugInfo debugInfo(Ctx, assertInst);

  Value* condition = assertInst->getArgOperand(0);
  Value* function_llvm =
      builder.CreateGlobalStringPtr(assertInst->getFunction()->getName());

  Value* args[] = {condition, debugInfo.getLineNumberValue(), function_llvm};

  builder.CreateCall(this->map2check_assert, args);
}

void AssertPass::runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx) {
  Function* calleeFunction = callInst->getCalledFunction();

  if (calleeFunction == NULL) {
    Value* v = callInst->getCalledOperand();
    calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if (calleeFunction == NULL) {
      return;
    }
  }
  if (calleeFunction->getName() == "__VERIFIER_assert") {
    this->instrumentAssert(callInst, Ctx);
  }
}

// --- New Pass Manager plugin registration ---
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "AssertPass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager& FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "assert-pass") {
                    FPM.addPass(AssertPass());
                    return true;
                  }
                  return false;
                });
          }};
}
