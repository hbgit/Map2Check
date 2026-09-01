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

#include <cstdlib>

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

namespace {

/// Counts the call sites TargetPass will match, across the whole module.
unsigned countTargetCallSites(const llvm::Module& M, llvm::StringRef Name) {
  unsigned sites = 0;
  for (const Function& Fn : M) {
    for (const BasicBlock& BB : Fn) {
      for (const llvm::Instruction& I : BB) {
        const CallInst* callInst = dyn_cast<CallInst>(&I);
        if (callInst == nullptr) continue;
        const Function* callee = callInst->getCalledFunction();
        if (callee == nullptr) {
          callee = dyn_cast<Function>(
              callInst->getCalledOperand()->stripPointerCasts());
        }
        if (callee != nullptr && callee->getName() == Name) ++sites;
      }
    }
  }
  return sites;
}

}  // namespace

PreservedAnalyses TargetPass::run(Function& F,
                                  llvm::FunctionAnalysisManager& AM) {
  // Was an unconditional per-FUNCTION write to errs(), which is unbuffered:
  // one syscall for every function in the module, every time the pass runs,
  // and under the hybrid the pass runs once per engine. On the large ECA and
  // Recursive programs it also buried the actual output -- the log of a run
  // that produced no verdict was tens of thousands of copies of this line.
  // Kept behind the environment switch the other passes use, so it is still
  // reachable when debugging the pass itself.
  if (getenv("MAP2CHECK_DEBUG_PASSES") != nullptr) {
    llvm::errs() << "Running TargetPass with: " << this->targetFunctionName
                 << "\n";
  }

  // Reported once per module, not once per function. This is a function pass,
  // so there is no module-entry hook to hang it on; opt runs once per
  // invocation, which makes a local static the honest way to say "first
  // function we see".
  //
  // A target that is never called is NOT an error. If a program genuinely
  // never calls reach_error, then "the error is unreachable" is the correct
  // answer and TRUE is right. What is wrong is that such a TRUE looks exactly
  // like a TRUE earned by exploring the program, and nothing in the output
  // tells them apart.
  //
  // That indistinguishability is the whole of finding B. The CASTLE harness
  // mapped seven CWEs onto `--target-function-name main`; TargetPass looks for
  // calls TO main, a program does not call its own entry point, so zero sites
  // were instrumented. Measured on the v6 baseline: 98 of 217 runs went through
  // that mode and produced TRUE, TIMEOUT or ERROR -- not one FALSE among them,
  // on 59 programs known to be vulnerable. Twelve of those runs were scored as
  // true negatives, credit for answers an oracle that can only say "correct"
  // could not have failed to give.
  static bool announcedTargetCoverage = false;
  if (!announcedTargetCoverage) {
    announcedTargetCoverage = true;
    if (countTargetCallSites(*F.getParent(), this->targetFunctionName) == 0) {
      llvm::errs() << "\n[map2check] WARNING: target function '"
                   << this->targetFunctionName
                   << "' is never called in this module. TargetPass"
                      " instruments call sites, so nothing was instrumented"
                      " and the reachability property holds trivially: a TRUE"
                      " verdict here means the target is absent, not that the"
                      " program is safe.\n";
    }
  }

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
