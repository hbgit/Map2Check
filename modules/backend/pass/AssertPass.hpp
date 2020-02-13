/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_ASSERTPASS_HPP_
#define MODULES_BACKEND_PASS_ASSERTPASS_HPP_

#include <llvm/IR/Constants.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "DebugInfo.hpp"

// using namespace llvm;
// Note:
// using-declaration: using std::vector; <- this is adopted in code style
// using-directive: using namespace std;

using llvm::BasicBlock;
using llvm::CallInst;
using llvm::Constant;
using llvm::Function;
using llvm::FunctionPass;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Value;

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

#endif  // MODULES_BACKEND_PASS_ASSERTPASS_HPP_
