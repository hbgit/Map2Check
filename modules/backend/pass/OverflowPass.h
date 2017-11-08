#pragma once


#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/Metadata.h>
#include <llvm/IR/DebugInfoMetadata.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <memory>

#include "DebugInfo.hpp"
#include "OperationsFunctions.hpp"


using namespace llvm;

struct OverflowPass : public FunctionPass {
  static char ID;
 OverflowPass() : FunctionPass(ID) { }
  virtual bool runOnFunction(Function &F);
 protected:
  Value* getFunctionNameValue() { return this->functionName; }
 private:  
  std::unique_ptr<OperationsFunctions > operationsFunctions;
  Value* functionName = NULL;
  void hasNonDetUint(Instruction* I);
  void listAllUintAssig(BasicBlock &B);
  
  void listAllUnsignedVar(Function &F);
  
  std::vector<Value*> storeInstWithUint;
  std::vector<Value*> loadInstWithUint;
  bool isUnitAssigment = false;
  std::vector<int> listLineNumUint;
  bool hasNonDetUintOp_1;
  bool hasNonDetUintOp_2;
  LLVMContext* Ctx;
  /* BasicBlock::iterator currentInstruction; */
};
