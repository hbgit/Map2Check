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

using namespace llvm;

struct OverflowPass : public FunctionPass {
    static char ID;
    OverflowPass() : FunctionPass(ID) { }
    virtual bool runOnFunction(Function &F);
protected:
    Value* getFunctionNameValue() { return this->functionName; }
private:
    Value* functionName = NULL;
    BasicBlock::iterator currentInstruction;
};
