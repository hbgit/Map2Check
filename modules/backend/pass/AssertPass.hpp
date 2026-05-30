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
#include <llvm/IR/PassManager.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "DebugInfo.hpp"

// using-declaration style (project convention)
using llvm::BasicBlock;
using llvm::CallInst;
using llvm::Constant;
using llvm::FunctionCallee;
using llvm::PointerType;
using llvm::Function;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::PreservedAnalyses;
using llvm::Value;

struct AssertPass : public llvm::PassInfoMixin<AssertPass> {
  PreservedAnalyses run(Function& F, llvm::FunctionAnalysisManager& AM);

 protected:
  void instrumentAssert(CallInst* assertInst, LLVMContext* Ctx);
  void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
  Value* getFunctionNameValue() { return this->functionName; }

 private:
  FunctionCallee map2check_assert;
  Value* functionName = NULL;
  BasicBlock::iterator currentInstruction;
};

#endif  // MODULES_BACKEND_PASS_ASSERTPASS_HPP_
