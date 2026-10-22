/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_OVERFLOWPASS_HPP_
#define MODULES_BACKEND_PASS_OVERFLOWPASS_HPP_

#include <llvm/IR/Constants.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>
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
#include "RuntimeABI.hpp"

// using namespace llvm;
using llvm::BasicBlock;
using llvm::BinaryOperator;
using llvm::Function;
using llvm::Instruction;
using llvm::LLVMContext;
using llvm::PreservedAnalyses;
using llvm::Type;
using llvm::Value;

struct OverflowPass : public llvm::PassInfoMixin<OverflowPass> {
  OverflowPass() = default;
  PreservedAnalyses run(Function &F, llvm::FunctionAnalysisManager &AM);
  static bool isRequired() { return true; }

 protected:
  Value *getFunctionNameValue() { return this->functionName; }

 private:
  // Signed binop checkers of map2check-library (RuntimeHook::Binop*).
  llvm::FunctionCallee binopAdd;
  llvm::FunctionCallee binopSub;
  llvm::FunctionCallee binopMul;
  llvm::FunctionCallee binopSDiv;
  llvm::FunctionCallee binopSRem;
  Value *functionName = NULL;
  void hasNonDetUint(Instruction *I);
  void listAllUintAssign(BasicBlock &B);

  std::string getValueNameOperator(Value *Vop);
  void listAllUnsignedVar(Function &F);

  std::vector<Value *> valuesThatShouldBeUint;
  std::vector<Value *> storeInstWithUint;
  std::vector<Value *> loadInstWithUint;
  std::vector<std::string> listUnsignedVars;
  bool isUnitAssignment = false;
  std::vector<int> listLineNumUint;

  bool isNSWBinaryOperator(BinaryOperator *op);
  // bool hasNonDetUintOp_1;
  // bool hasNonDetUintOp_2;
  LLVMContext *Ctx;
  /* BasicBlock::iterator currentInstruction; */
};

#endif  // MODULES_BACKEND_PASS_OVERFLOWPASS_HPP_
