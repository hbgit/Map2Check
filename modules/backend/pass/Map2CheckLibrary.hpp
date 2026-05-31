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
#include <llvm/IR/PassManager.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <memory>

#include "DebugInfo.hpp"
#include "LibraryFunctions.hpp"

using llvm::BasicBlock;
using llvm::CallInst;
using llvm::dyn_cast;
using llvm::Function;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::PreservedAnalyses;
using std::make_unique;
using llvm::Value;

struct Map2CheckLibrary : public llvm::PassInfoMixin<Map2CheckLibrary> {
  explicit Map2CheckLibrary(bool svcomp = false) : SVCOMP(svcomp) {}

  PreservedAnalyses run(Function& F, llvm::FunctionAnalysisManager& AM);
  static bool isRequired() { return true; }

 protected:
  void instrumentStartInstruction(LLVMContext* Ctx);
  void instrumentReleaseInstruction(LLVMContext* Ctx);
  void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
  Value* getFunctionNameValue() { return this->functionName; }

 private:
  std::unique_ptr<LibraryFunctions> libraryFunctions;
  bool SVCOMP = false;
  Value* functionName = NULL;
  BasicBlock::iterator currentInstruction;
};

#endif  // MODULES_BACKEND_PASS_MAP2CHECKLIBRARY_HPP_
