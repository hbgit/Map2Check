/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#include "AssertPass.hpp"

using llvm::dyn_cast;
using llvm::IRBuilder;
using llvm::RegisterPass;

bool AssertPass::runOnFunction(Function& F) {
  this->map2check_assert = F.getParent()->getOrInsertFunction(
      "map2check_assert", Type::getVoidTy(F.getContext()),
      Type::getInt32Ty(F.getContext()), Type::getInt32Ty(F.getContext()),
      Type::getInt8PtrTy(F.getContext()));

  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  // IRBuilder<> builder((Instruction*)&*instructionIterator);
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
  return true;
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
    Value* v = callInst->getCalledValue();
    calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if (calleeFunction == NULL) {
      return;
    }
  }
  if (calleeFunction->getName() == "__VERIFIER_assert") {
    this->instrumentAssert(callInst, Ctx);
  }
}

char AssertPass::ID = 12;
static RegisterPass<AssertPass> X("validate_asserts",
                                  "Add checks for __VERIFIER_assert");
