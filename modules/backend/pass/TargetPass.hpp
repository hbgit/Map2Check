/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
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
#include <llvm/Support/CommandLine.h>

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
//using llvm::cl;

// Target option
static llvm::cl::opt<std::string> TargetNameOption("function-name", llvm::cl::desc("Specify the target function"), llvm::cl::init("__VERIFIER_error"));
struct TargetPass : public FunctionPass {
  static char ID;
  TargetPass() : FunctionPass(ID) {
    targetFunctionName = TargetNameOption;
  }
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
  std::string targetFunctionName;
};

#endif  // MODULES_BACKEND_PASS_TARGETPASS_HPP_
