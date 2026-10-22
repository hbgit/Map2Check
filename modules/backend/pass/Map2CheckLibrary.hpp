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
  /// map2check-library's AnalysisMode, the argument of map2check_set_mode.
  enum class AnalysisMode : int {
    None = 0,
    Memtrack = 1,
    Memcleanup = 2,
    Overflow = 3,
    Assert = 4,
    Target = 5
  };

  /// Mode from the -m2c-analysis-mode option (default none).
  Map2CheckLibrary();
  explicit Map2CheckLibrary(AnalysisMode mode) : mode(mode) {}

  PreservedAnalyses run(Function& F, llvm::FunctionAnalysisManager& AM);
  static bool isRequired() { return true; }

 protected:
  void instrumentStartInstruction(LLVMContext* Ctx);
  void instrumentReleaseInstruction(LLVMContext* Ctx);
  void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
  Value* getFunctionNameValue() { return this->functionName; }

 private:
  AnalysisMode mode = AnalysisMode::None;
  llvm::FunctionCallee setModeFunction;
  llvm::FunctionCallee initFunction;
  llvm::FunctionCallee successFunction;
  Value* functionName = NULL;
  BasicBlock::iterator currentInstruction;
};

#endif  // MODULES_BACKEND_PASS_MAP2CHECKLIBRARY_HPP_
