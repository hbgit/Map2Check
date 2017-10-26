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
#include "../../frontend/utils/tools.hpp"

using namespace llvm;
namespace Tools = Map2Check::Tools;

struct TrackBasicBlockPass : public FunctionPass {
	static char ID;
	TrackBasicBlockPass() : FunctionPass(ID) { }
	TrackBasicBlockPass(std::string cprogram_path) : FunctionPass(ID) 
	{ 
		this->cprogram_path = cprogram_path;        
        this->sourceCodeHelper = make_unique<Tools::SourceCodeHelper>(Tools::SourceCodeHelper(cprogram_path));
	}
	virtual bool runOnFunction(Function &F);
	
	protected:
		void runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx);
		void instrumentEntryBB(BasicBlock& B, LLVMContext* Ctx); 
		void instrumentInstBB(BasicBlock::iterator& iT);
		bool checkInstBBIsAssume(BasicBlock::iterator& iT);
		Value* getFunctionNameValue() { return this->functionName; }
  
	private: 
		std::unique_ptr<Tools::SourceCodeHelper> sourceCodeHelper;
		std::string cprogram_path;
		std::unique_ptr<LibraryFunctions> libraryFunctions; 
		bool initializedFunctionName = false; 
		BasicBlock::iterator st_lastBlockInst;  
		BasicBlock::iterator currentInstruction;
		Function* currentFunction;
		LLVMContext* Ctx;
		Value* functionName = NULL;
		int numLineBlk_ori;
		int numLineBlk_AA;
};



class TrackBasicBlockPassException : public std::runtime_error {
 public:
 TrackBasicBlockPassException(std::string message) : std::runtime_error(message) {}
  virtual const char* what() const throw();
};


