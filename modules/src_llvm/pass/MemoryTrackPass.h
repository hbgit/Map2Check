#pragma once

#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace llvm;

enum class NonDetType {INTEGER,STRING,FLOAT};

struct MemoryTrackPass : public FunctionPass {
  static char ID;
 MemoryTrackPass(bool SVCOMP = true) : FunctionPass(ID) {
    this->target_function = "";
    this->isTrackingFunction = false;
    this->cleanWitnessInfoFile();
  }
 MemoryTrackPass(std::string function, bool SVCOMP =  true) : FunctionPass(ID) {
  this->SVCOMP = SVCOMP;
  this->target_function = function;
  this->isTrackingFunction = true;
  this->cleanWitnessInfoFile();
  }
 virtual bool runOnFunction(Function &F);

 private:
  void cleanWitnessInfoFile();
  void instrumentKlee(NonDetType nonDetType);
  void instrumentPointer();
  void instrumentTargetFunction();
  void instrumentMalloc();
  void instrumentFree();
  void instrumentKleeInt();
  void instrumentInit();
  void instrumentReleaseMemoryOnCurrentInstruction();
  void instrumentReleaseMemory();
  void instrumentAllocation();
  void instrumentFunctionAddress();
  void runOnAllocaInstruction();
  void runOnCallInstruction();
  void runOnStoreInstruction();
  void switchCallInstruction();
  void prepareMap2CheckInstructions();
  void addWitnessInfo(std::string info);
  void getDebugInfo();
  int getLineNumber();

  bool SVCOMP;
  bool isTrackingFunction;
  std::string target_function;
  bool mainFunctionInitialized = false;
  std::vector<Function*> functionsValues;
  Function* currentFunction;
  DataLayout* currentDataLayout;
  Function* mainFunction;
  Function* caleeFunction;
  BasicBlock::iterator currentInstruction;
  BasicBlock::iterator lastInstructionMain;
  Constant* map2check_target_function;
  Constant* map2check_pointer;
  Constant* map2check_malloc;
  Constant* map2check_free;
  Constant* map2check_init;
  Constant* map2check_alloca;
  Constant* map2check_function;
  Constant* map2check_free_resolved_address;
  Constant*  map2check_klee_int;
  Constant* map2check_success;
  ConstantInt* scope_value;
  ConstantInt* line_value;
  LLVMContext* Ctx;
};


class MemoryTrackPassException : public std::runtime_error {
 public:
 MemoryTrackPassException(std::string message) : std::runtime_error(message) {}
  virtual const char* what() const throw();
};
