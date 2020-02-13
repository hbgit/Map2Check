/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_MAP2CHECKLIBRARY_HPP_
#define MODULES_BACKEND_PASS_MAP2CHECKLIBRARY_HPP_

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

#include <memory>

// using namespace llvm;
#include "DebugInfo.hpp"
#include "LibraryFunctions.hpp"

using llvm::BasicBlock;
using llvm::CallInst;
using llvm::dyn_cast;
using llvm::Function;
using llvm::FunctionPass;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::make_unique;
using llvm::Value;

struct Map2CheckLibrary : public FunctionPass {
  static char ID;
  explicit Map2CheckLibrary(bool svcomp) : FunctionPass(ID) { SVCOMP = svcomp; }
  Map2CheckLibrary() : FunctionPass(ID) {}
  virtual bool runOnFunction(Function& F);

 protected:
  void instrumentStartInstruction(LLVMContext* Ctx);
  void instrumentReleaseInstruction(LLVMContext* Ctx);
  void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
  Value* getFunctionNameValue() { return this->functionName; }

 private:
  std::unique_ptr<LibraryFunctions> libraryFunctions;
  // bool hasInitialized = false;
  bool SVCOMP = false;
  Value* functionName = NULL;
  BasicBlock::iterator currentInstruction;
};

#endif  // MODULES_BACKEND_PASS_MAP2CHECKLIBRARY_HPP_
