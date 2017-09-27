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

struct GenerateAutomataTruePass : public FunctionPass 
{
    static char ID;
    GenerateAutomataTruePass(std::string function) : FunctionPass(ID) 
    {
        this->target_function = function;
        this->isTrackingFunction = false;    
    }
 
    virtual bool runOnFunction(Function &F) override;

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
	ConstantInt* scope_value;
	ConstantInt* line_value;
	LLVMContext* Ctx;
};


//char GenerateAutomataTruePass::ID = 1;
//static RegisterPass<GenerateAutomataTruePass> X("generate_automata_true", "Generate Automata True", false, false);


class GenerateAutomataTruePassException : public std::runtime_error {
 public:
 GenerateAutomataTruePassException(std::string message) : std::runtime_error(message) {}
  virtual const char* what() const throw();
};
