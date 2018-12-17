#pragma once

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Metadata.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <memory>

#include "DebugInfo.hpp"

using namespace llvm;

struct AssertPass : public FunctionPass {
  static char ID;
  AssertPass() : FunctionPass(ID) {}
  virtual bool runOnFunction(Function& F);

 protected:
  void instrumentAssert(CallInst* assertInst, LLVMContext* Ctx);
  void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
  Value* getFunctionNameValue() { return this->functionName; }

 private:
  Constant* map2check_assert = NULL;
  Value* functionName = NULL;
  BasicBlock::iterator currentInstruction;
};