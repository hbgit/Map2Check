/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "TargetPass.hpp"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

PreservedAnalyses TargetPass::run(Function& F,
                                  llvm::FunctionAnalysisManager& AM) {
  llvm::errs() << "Running TargetPass with: " << this->targetFunctionName;

  this->targetFunctionMap2Check = F.getParent()->getOrInsertFunction(
      "map2check_target_function", Type::getVoidTy(F.getContext()),
      PointerType::get(F.getContext(), 0), Type::getInt32Ty(F.getContext()),
      Type::getInt32Ty(F.getContext()));

  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*instructionIterator));
  this->functionName = builder.CreateGlobalStringPtr(F.getName());

  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
        currentInstruction = i;
        this->runOnCallInstruction(callInst, &F.getContext());
      }
    }
  }
  return PreservedAnalyses::none();
}

void TargetPass::runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx) {
  Function* calleeFunction = callInst->getCalledFunction();

  if (calleeFunction == NULL) {
    Value* v = callInst->getCalledOperand();
    calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if (calleeFunction == NULL) {
      return;
    }
  }

  if (calleeFunction->getName() == targetFunctionName) {
    this->instrumentErrorInstruction(callInst, Ctx);
  }
}

void TargetPass::instrumentErrorInstruction(CallInst* callInst,
                                            LLVMContext* Ctx) {
  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*currentInstruction));
  Value* name_llvm = functionName;

  DebugInfo debugInfo(Ctx, callInst);

  Value* args[] = {name_llvm, debugInfo.getScopeNumberValue(),
                   debugInfo.getLineNumberValue()};

  builder.CreateCall(targetFunctionMap2Check, args);
}

// --- New Pass Manager plugin registration ---
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "TargetPass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager& FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "target-pass") {
                    FPM.addPass(TargetPass());
                    return true;
                  }
                  return false;
                });
          }};
}
