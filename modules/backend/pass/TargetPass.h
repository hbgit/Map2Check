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
#include "DebugInfo.hpp"

using namespace llvm;

struct TargetPass : public FunctionPass
{
    static char ID;
    TargetPass() : FunctionPass(ID) {}
    TargetPass(std::string FunctionName) : FunctionPass(ID) {
        targetFunctionName = FunctionName;
    }
    virtual bool runOnFunction(Function &F);

protected:
    Value* getFunctionNameValue() { return this->functionName; }
    void runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx);
    void instrumentErrorInstruction(CallInst* callInst, LLVMContext* Ctx);

private:
    BasicBlock::iterator currentInstruction;
    Constant *targetFunctionMap2Check = NULL;
    Value* functionName = NULL;
    std::string targetFunctionName;
};
