// FuncPass.h
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

using namespace llvm;

struct FuncPass : public FunctionPass {
  static char ID;
  FuncPass() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F);
};
