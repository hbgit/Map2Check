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
#include "LibraryFunctions.hpp"

using namespace llvm;

struct TrackBasicBlockPass : public FunctionPass {
	static char ID;
	TrackBasicBlockPass() : FunctionPass(ID) { }
	virtual bool runOnFunction(Function &F);
	
	protected:
		void runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx);
		void instrumentLastInstBB(BasicBlock::iterator& iT);
		Value* getFunctionNameValue() { return this->functionName; }
  
	private: 
		std::unique_ptr<LibraryFunctions> libraryFunctions; 
		bool initializedFunctionName = false; 
		BasicBlock::iterator st_lastBlockInst;  
		BasicBlock::iterator currentInstruction;
		Function* currentFunction;
		LLVMContext* Ctx;
		Value* functionName = NULL;
		int numLineBlk;
};



class TrackBasicBlockPassException : public std::runtime_error {
 public:
 TrackBasicBlockPassException(std::string message) : std::runtime_error(message) {}
  virtual const char* what() const throw();
};


