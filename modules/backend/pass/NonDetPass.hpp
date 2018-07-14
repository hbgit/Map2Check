#pragma once

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

#include "DebugInfo.hpp"
#include "NonDetFunctions.hpp"

using namespace llvm;

enum class NonDetType {
  INTEGER,
  UNSIGNED,
  POINTER,
  //  STRING,
  LONG,
  ASSUME,
  USHORT,
  CHAR
};

struct NonDetPass : public FunctionPass {
  static char ID;
  NonDetPass() : FunctionPass(ID) {}
  virtual bool runOnFunction(Function& F);

 protected:
  void instrumentInstruction();
  /**
   * @brief Checks if current function is a non det call and instruments it
   */
  void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
  Value* getFunctionNameValue() { return this->functionName; }

 private:
  void instrumentNonDet(NonDetType type, Function* caleeFunction);
  void instrumentNonDetInt(CallInst* callInst, LLVMContext* Ctx);
  void instrumentNonDetUnsigned(CallInst* callInst, LLVMContext* Ctx);
  void instrumentNonDetChar(CallInst* callInst, LLVMContext* Ctx);
  void instrumentNonDetPointer(CallInst* callInst, LLVMContext* Ctx);
  void instrumentNonDetLong(CallInst* callInst, LLVMContext* Ctx);
  void instrumentNonDetUshort(CallInst* callInst, LLVMContext* Ctx);

  std::unique_ptr<NonDetFunctions> nonDetFunctions;
  Value* functionName = NULL;
  BasicBlock::iterator currentInstruction;
};

class NonDetPassException : public std::runtime_error {
 public:
  explicit NonDetPassException(std::string message)
      : std::runtime_error(message) {}
  virtual const char* what() const throw();
};
