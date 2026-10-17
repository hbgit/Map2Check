/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_NONDETPASS_HPP_
#define MODULES_BACKEND_PASS_NONDETPASS_HPP_

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
#include "RuntimeABI.hpp"

using llvm::BasicBlock;
using llvm::CallInst;
using llvm::Function;
using llvm::LLVMContext;
using llvm::PreservedAnalyses;
using llvm::Value;

/// Logs every __VERIFIER_nondet_<t> value through the core
/// map2check_save_nondet_log_<t> of map2check-library, and turns the
/// program's assume idioms into the library's __VERIFIER_assume.
struct NonDetPass : public llvm::PassInfoMixin<NonDetPass> {
  PreservedAnalyses run(Function &F, llvm::FunctionAnalysisManager &AM);
  static bool isRequired() { return true; }

 private:
  void instrumentNonDet(CallInst *callInst, llvm::StringRef type);
  void instrumentCrabAssume(CallInst *callInst);
  Value *functionName = nullptr;
};

class NonDetPassException : public std::runtime_error {
 public:
  explicit NonDetPassException(std::string message)
      : std::runtime_error(message) {}
  virtual const char *what() const throw();
};

#endif  // MODULES_BACKEND_PASS_NONDETPASS_HPP_
