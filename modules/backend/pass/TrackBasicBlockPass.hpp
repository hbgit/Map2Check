/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_TRACKBASICBLOCKPASS_HPP_
#define MODULES_BACKEND_PASS_TRACKBASICBLOCKPASS_HPP_

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

#include "../../frontend/utils/tools.hpp"
#include "DebugInfo.hpp"
#include "LibraryFunctions.hpp"

// using namespace llvm;
namespace Tools = Map2Check;

using llvm::BasicBlock;
using llvm::Function;
using llvm::LLVMContext;
using llvm::PreservedAnalyses;
using std::make_unique;
using llvm::Value;

struct TrackBasicBlockPass : public llvm::PassInfoMixin<TrackBasicBlockPass> {
  TrackBasicBlockPass() = default;
  explicit TrackBasicBlockPass(std::string c_program_path)
      : c_program_path(std::move(c_program_path)) {
    this->sourceCodeHelper = make_unique<Tools::SourceCodeHelper>(
        Tools::SourceCodeHelper(this->c_program_path));
  }
  PreservedAnalyses run(Function& F, llvm::FunctionAnalysisManager& AM);

 protected:
  void runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx);
  void hasCallOnBasicBlock(BasicBlock& B, LLVMContext* Ctx);
  void instrumentEntryBB(BasicBlock& B, LLVMContext* Ctx);
  void instrumentInstBB(BasicBlock::iterator& iT);
  bool checkInstBbIsAssume(BasicBlock::iterator& iT);
  Value* getFunctionNameValue() { return this->functionName; }

 private:
  std::unique_ptr<Tools::SourceCodeHelper> sourceCodeHelper;
  std::string c_program_path;
  std::unique_ptr<LibraryFunctions> libraryFunctions;
  bool initializedFunctionName = false;
  bool isUnreachableInst = false;
  BasicBlock::iterator st_lastBlockInst;
  BasicBlock::iterator currentInstruction;
  Function* currentFunction;
  LLVMContext* Ctx;
  Value* functionName = NULL;
  int numLineBlk_ori;
  int numLineBlk_AA;
};

class TrackBasicBlockPassException : public std::runtime_error {
 public:
  explicit TrackBasicBlockPassException(std::string message)
      : std::runtime_error(message) {}
  virtual const char* what() const throw();
};

#endif  // MODULES_BACKEND_PASS_TRACKBASICBLOCKPASS_HPP_
