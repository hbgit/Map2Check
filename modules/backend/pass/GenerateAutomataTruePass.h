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
#include "../../frontend/utils/tools.hpp"

using namespace llvm;
namespace Tools = Map2Check::Tools;

struct GenerateAutomataTruePass : public FunctionPass 
{
    static char ID;
    GenerateAutomataTruePass() : FunctionPass(ID) {}
    GenerateAutomataTruePass(std::string function, std::string cprogram_path) : FunctionPass(ID) 
    {
        this->target_function = function;
        this->cprogram_path = cprogram_path;
        this->isTrackingFunction = true;    
        this->sourceCodeHelper = make_unique<Tools::SourceCodeHelper>(Tools::SourceCodeHelper(cprogram_path));
    }

    virtual bool runOnFunction(Function &F);

    protected:   
    void runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx);
    bool checkBBHasLError(BasicBlock& nowB);
    bool isBranchCond(BasicBlock& B);
    std::string convertLLPredicatetoXmlText(Instruction& I);
    std::string getPredicateSymOnXmlText(ICmpInst& icmpInst);

    private:
    //methods
    //attr
    std::unique_ptr<Tools::SourceCodeHelper> sourceCodeHelper;
    bool isTrackingFunction;
    bool intervals;
    StringRef labelBranchCondTrue;
    StringRef labelBranchCondFalse;
    StringRef labelBasicBlock;
    std::string target_function;
    std::string cprogram_path;
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


