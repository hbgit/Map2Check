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

using namespace llvm;
#include "DebugInfo.hpp"
#include "LibraryFunctions.hpp"

struct Map2CheckLibrary : public FunctionPass {
  static char ID;
  Map2CheckLibrary(bool svcomp) : FunctionPass(ID) { SVCOMP = svcomp; }
  Map2CheckLibrary() : FunctionPass(ID) {}
  virtual bool runOnFunction(Function& F);

 protected:
  void instrumentStartInstruction(LLVMContext* Ctx);
  void instrumentReleaseInstruction(LLVMContext* Ctx);
  void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
  Value* getFunctionNameValue() { return this->functionName; }

 private:
  std::unique_ptr<LibraryFunctions> libraryFunctions;
  // bool hasInitialized = false;
  bool SVCOMP = false;
  Value* functionName = NULL;
  BasicBlock::iterator currentInstruction;
};
