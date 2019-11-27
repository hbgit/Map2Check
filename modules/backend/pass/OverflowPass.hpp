/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
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
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "DebugInfo.hpp"
#include "OperationsFunctions.hpp"

// using namespace llvm;
using llvm::BasicBlock;
using llvm::BinaryOperator;
using llvm::Function;
using llvm::FunctionPass;
using llvm::Instruction;
using llvm::LLVMContext;
using llvm::Value;

struct OverflowPass : public FunctionPass {
  static char ID;
  OverflowPass() : FunctionPass(ID) {}
  explicit OverflowPass(std::vector<int> lines) : FunctionPass(ID) {
    this->errorLines = lines;
  }
  virtual bool runOnFunction(Function &F);

 protected:
  Value *getFunctionNameValue() { return this->functionName; }

 private:
  std::unique_ptr<OperationsFunctions> operationsFunctions;
  Value *functionName = NULL;
  void hasNonDetUint(Instruction *I);
  void listAllUintAssign(BasicBlock &B);

  std::string getValueNameOperator(Value *Vop);
  void listAllUnsignedVar(Function &F);

  std::vector<int> errorLines;
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
