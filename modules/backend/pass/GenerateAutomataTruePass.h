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

//From Map2Check Project
#include "DebugInfo.hpp"

using namespace llvm;

struct GenerateAutomataTruePass : public FunctionPass 
{
    static char ID;
    GenerateAutomataTruePass() : FunctionPass(ID) {}
    GenerateAutomataTruePass(std::string function) : FunctionPass(ID) 
    {
        this->target_function = function;
        this->isTrackingFunction = true;    
    }

    virtual bool runOnFunction(Function &F);

    protected:   
    void runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx);
    bool checkBBHasLError(BasicBlock& nowB);

    private:
    //methods
    //attr
    bool isTrackingFunction;
    bool intervals;
    std::string target_function;
    bool mainFunctionInitialized = false;
    std::vector<Function*> functionsValues;
    Function* currentFunction;
    DataLayout* currentDataLayout;
    Function* mainFunction;  
    BasicBlock::iterator currentInstruction;
    BasicBlock::iterator lastInstructionMain;  
    BasicBlock::iterator lastBlockInst;  
    BasicBlock::iterator firstBlockInst;  

    ConstantInt* scope_value;
    ConstantInt* line_value;
    LLVMContext* Ctx;
};


