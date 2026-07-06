/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "LoopPredAssumePass.hpp"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using llvm::IRBuilder;

namespace {
inline Instruction* BBIteratorToInst(BasicBlock::iterator i) {
  Instruction* pointer = reinterpret_cast<Instruction*>(&*i);
  return pointer;
}
}  // namespace

void LoopPredAssumePass::getConditionInLoop(Loop* L) {
  BasicBlock* header = L->getHeader();
  this->map2check_assume =
      header->getParent()->getParent()->getOrInsertFunction(
          "map2check_assume_loop", Type::getVoidTy(header->getContext()),
          Type::getInt1Ty(header->getContext()));
  if (BranchInst* bi = dyn_cast<BranchInst>(header->getTerminator())) {
    if (bi->isConditional()) {
      Value* loopCond = bi->getCondition();

      CmpInst* cmpInst = dyn_cast<CmpInst>(&*loopCond);

      if (bi->getNumSuccessors() > 0) {
        BasicBlock* succ_cond_bb = bi->getSuccessor(0);
        BasicBlock::iterator iT = --succ_cond_bb->end();

        auto* new_inst = cmpInst->clone();
        auto* inst_pos = dyn_cast<Instruction>(&*--succ_cond_bb->end());
        new_inst->insertBefore(inst_pos);

        CmpInst* new_cmpInst = dyn_cast<CmpInst>(&*new_inst);
        new_cmpInst->setPredicate(new_cmpInst->getInversePredicate());

        Value* new_loop_cond =
            dyn_cast<Value>(&*new_cmpInst);  // Convert CmpInst to Value;

        IRBuilder<> builder(BBIteratorToInst(iT));

        Value* args[] = {new_loop_cond};
        builder.CreateCall(this->map2check_assume, args);
      }
    }
  }
}

PreservedAnalyses LoopPredAssumePass::run(Loop& L,
                                           llvm::LoopAnalysisManager& AM,
                                           llvm::LoopStandardAnalysisResults& AR,
                                           llvm::LPMUpdater& U) {
  getConditionInLoop(&L);
  return PreservedAnalyses::none();
}

// --- New Pass Manager plugin registration ---
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopPredAssumePass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::LoopPassManager& LPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "loop-pred-assume") {
                    LPM.addPass(LoopPredAssumePass());
                    return true;
                  }
                  return false;
                });
          }};
}
