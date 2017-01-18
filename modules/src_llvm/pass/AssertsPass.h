// AssertsPass.h
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
using namespace llvm;

struct AssertsPass : public FunctionPass {
private:
  std::string target_function;
public:
  
  static char ID;
  AssertsPass() : FunctionPass(ID) {}
  AssertsPass(std::string functions) : FunctionPass(ID) {
    this->target_function = functions;
  }


  virtual bool runOnFunction(Function &F);


};
