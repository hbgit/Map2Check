//Map2Check library
#include "../src_llvm/pass/FuncPass.h"
#include "caller.h"

//LLVM
#include <llvm/LinkAllPasses.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/ADT/Statistic.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Bitcode/ReaderWriter.h>


#include <iostream>
#include <string>

using namespace std;
using namespace llvm;

namespace {
  static void check(std::string E) {
    if (!E.empty()) {
      if (errs().has_colors())
        errs().changeColor(raw_ostream::RED);
      errs () << E << "\n";
      if (errs().has_colors())
        errs().resetColor();
      exit(1);
    }
  }
}

llvm::Module * M;
// Build up all of the passes that we want to do to the module.
PassManager InitialPasses;
PassManager AnalysisPasses;


Caller::Caller( std::string bcprogram_path ) {
	this->pathprogram = bcprogram_path;
}

void Caller::printdata() {
	cout << "File Path:" << this->pathprogram << endl;
	this->parseIrFile();
}

int Caller::parseIrFile(){
	// Parse the input LLVM IR file into a module.
	StringRef filename = this->pathprogram;

	SMDiagnostic SM;
	LLVMContext & Context = getGlobalContext();
	M = ParseIRFile(filename,SM,Context);

	if (!SM.getMessage().empty()){
		check("Problem reading input bitcode/IR: " + SM.getMessage().str());
	}else{
		std::cout << "Successfully read bitcode/IR" << std::endl;

		/*for (Module::iterator I = M->begin(), E = M->end(); I != E; ++I){
			if (I->hasName()) {
				cout << " Name: " << I->getName().str() << endl;
			}
		}**/

		PassRegistry &Registry = *PassRegistry::getPassRegistry();
		initializeAnalysis(Registry);
		InitialPasses.add(llvm::createCFGSimplificationPass());
		InitialPasses.run(*M);
	}


	return 0;
}

int Caller::callPass(){
	// Create a pass manager and fill it with the passes we want to run.
	// legacy::PassManager PM;
	// PM.add(new FuncPass());
	// PM.run(*M);

	AnalysisPasses.add(new FuncPass());
	AnalysisPasses.run(*M);

	return 1;
}
