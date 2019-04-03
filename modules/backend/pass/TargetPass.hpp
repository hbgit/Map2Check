/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_BACKEND_PASS_TARGETPASS_HPP_
#define MODULES_BACKEND_PASS_TARGETPASS_HPP_

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "DebugInfo.hpp"

// using namespace llvm;
using llvm::BasicBlock;
using llvm::CallInst;
using llvm::Constant;
using llvm::dyn_cast;
using llvm::Function;
using llvm::FunctionPass;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::RegisterPass;
using llvm::Value;

struct TargetPass : public FunctionPass {
  static char ID;
  TargetPass() : FunctionPass(ID) {}
  explicit TargetPass(std::string FunctionName) : FunctionPass(ID) {
    targetFunctionName = FunctionName;
  }
  virtual bool runOnFunction(Function &F);

 protected:
  Value *getFunctionNameValue() { return this->functionName; }
  void runOnCallInstruction(CallInst *callInst, LLVMContext *Ctx);
  void instrumentErrorInstruction(CallInst *callInst, LLVMContext *Ctx);

 private:
  BasicBlock::iterator currentInstruction;
  Constant *targetFunctionMap2Check = NULL;
  Value *functionName = NULL;
  std::string targetFunctionName = "__VERIFIER_error";
};

#endif  // MODULES_BACKEND_PASS_TARGETPASS_HPP_
