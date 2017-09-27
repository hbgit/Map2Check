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

#include <memory>

#include "DebugInfo.hpp"
#include "KleeFunctions.hpp"

using namespace llvm;

enum class NonDetType {INTEGER,
                       POINTER,
                       STRING,
                       LONG,
                       ASSUME,
                       USHORT,
                       CHAR};

struct NonDetPass : public FunctionPass {
  static char ID;
 NonDetPass() : FunctionPass(ID) { }
 virtual bool runOnFunction(Function &F);
protected:
  void instrumentInstruction();
  /**
   * @brief Checks if current function is a non det call and instruments it
   */
  void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
  Value* getFunctionNameValue() { return this->functionName; }
  
private:
  void instrumentKlee(NonDetType type, Function *caleeFunction);
  void instrumentKleeInt(CallInst* callInst, LLVMContext* Ctx);
  void instrumentKleeChar(CallInst* callInst, LLVMContext* Ctx);
  void instrumentKleePointer(CallInst* callInst, LLVMContext* Ctx);
  void instrumentKleeLong(CallInst* callInst,LLVMContext* Ctx);
  void instrumentKleeUshort(CallInst* callInst, LLVMContext* Ctx);

  std::unique_ptr<KleeFunctions> kleeFunctions;
  Value* functionName = NULL;
  BasicBlock::iterator currentInstruction;
};



class NonDetPassException : public std::runtime_error {
 public:
 NonDetPassException(std::string message) : std::runtime_error(message) {}
  virtual const char* what() const throw();
};


