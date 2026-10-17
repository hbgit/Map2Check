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

#include "RuntimeABI.hpp"

using llvm::dyn_cast;
using llvm::IRBuilder;
using llvm::FunctionCallee;
using llvm::PassPluginLibraryInfo;

PreservedAnalyses AssertPass::run(Function& F,
                                  llvm::FunctionAnalysisManager& AM) {
  this->map2check_assert = Map2Check::getOrInsertRuntime(
      *F.getParent(), Map2Check::RuntimeHook::Assert);

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
  // The core takes an i32; a program that declares __VERIFIER_assert with
  // another integer parameter type still passes "condition != 0".
  if (condition->getType()->isIntegerTy() &&
      !condition->getType()->isIntegerTy(32)) {
    condition = builder.CreateZExt(
        builder.CreateIsNotNull(condition), Type::getInt32Ty(*Ctx));
  }
  Value* function_llvm =
      builder.CreateGlobalStringPtr(assertInst->getFunction()->getName());

  // map2check_is_valid_assert(line, function, expression): the condition
  // goes last in the core API.
  Value* args[] = {debugInfo.getLineNumberValue(), function_llvm, condition};

  builder.CreateCall(this->map2check_assert, args);
}

void AssertPass::instrumentAssertFail(CallInst* assertInst, LLVMContext* Ctx) {
  IRBuilder<> builder(assertInst);
  // A call to __assert_fail is only reached when the assertion already failed,
  // so emit a constant-false condition. glibc signature:
  //   __assert_fail(const char *assertion, const char *file,
  //                 unsigned int line, const char *function)
  Value* condition = ConstantInt::get(Type::getInt32Ty(*Ctx), 0);
  Value* line = assertInst->getArgOperand(2);
  Value* function_llvm = assertInst->getArgOperand(3);

  Value* args[] = {line, function_llvm, condition};

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
  } else if (calleeFunction->getName() == "__assert_fail") {
    this->instrumentAssertFail(callInst, Ctx);
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
