/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_BACKEND_PASS_MEMORYTRACKPASS_HPP_
#define MODULES_BACKEND_PASS_MEMORYTRACKPASS_HPP_

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

// using namespace llvm;
using llvm::BasicBlock;
using llvm::Constant;
using llvm::ConstantInt;
using llvm::Function;
using llvm::FunctionPass;
using llvm::LLVMContext;

struct MemoryTrackPass : public FunctionPass {
  static char ID;
  explicit MemoryTrackPass(bool SVCOMP = false) : FunctionPass(ID) {
    this->SVCOMP = SVCOMP;
  }
  virtual bool runOnFunction(Function& F);

 private:
  void instrumentPointer();
  void instrumentMalloc();
  void instrumentCalloc();
  void instrumentRealloc();
  void instrumentMemset();
  void instrumentMemcpy();
  void instrumentPosixMemAllign();
  void instrumentFree();
  void instrumentInit();
  void instrumentAlloca();
  void instrumentArrayAlloca();
  void instrumentNotStaticArrayAlloca();
  void instrumentAllocation();
  void instrumentFunctionAddress();
  void instrumentFunctionArgumentAddress();
  void runOnAllocaInstruction();
  void runOnCallInstruction();
  void runOnStoreInstruction();
  void runOnLoadInstruction();
  void switchCallInstruction();
  void prepareMap2CheckInstructions();
  // void addWitnessInfo(std::string info);
  void getDebugInfo();
  int getLineNumber();

  bool SVCOMP;
  bool mainFunctionInitialized = false;
  std::vector<Function*> functionsValues;
  Function* currentFunction;
  // DataLayout* currentDataLayout;
  Function* mainFunction;
  Function* calleeFunction;
  BasicBlock::iterator currentInstruction;
  BasicBlock::iterator lastInstructionMain;
  Constant* map2check_pointer;
  Constant* map2check_malloc;
  Constant* map2check_calloc;
  Constant* map2check_free;
  Constant* map2check_alloca;
  Constant* map2check_non_static_alloca;
  Constant* map2check_posix;
  Constant* map2check_load;
  Constant* map2check_check_deref;
  Constant* map2check_function;
  Constant* map2check_free_resolved_address;
  ConstantInt* scope_value;
  ConstantInt* line_value;
  LLVMContext* Ctx;
};

class MemoryTrackPassException : public std::runtime_error {
 public:
  explicit MemoryTrackPassException(std::string message)
      : std::runtime_error(message) {}
  virtual const char* what() const throw();
};

#endif  // MODULES_BACKEND_PASS_MEMORYTRACKPASS_HPP_
