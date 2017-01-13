//Map2Check library
#include "../src_llvm/pass/FuncPass.h"
#include "../src_llvm/pass/AllocaPass.h"
#include "../src_llvm/pass/StorePass.h"

#include "caller.h"


//LLVM
#include <llvm/LinkAllPasses.h>
#include <llvm/Pass.h>
#include <llvm/IR/PassManager.h>
#include <llvm/PassRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Linker/Linker.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/ADT/Statistic.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Bitcode/ReaderWriter.h>


#include <iostream>
#include <string>

#include <stdlib.h>

using namespace std;
using namespace llvm;

#define DEBUG_PASS


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

std::unique_ptr<Module> M;
// Build up all of the passes that we want to do to the module.
legacy::PassManager InitialPasses;
legacy::PassManager AnalysisPasses;


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
	M = parseIRFile(filename,SM,Context);

	if (!SM.getMessage().empty()){
		check("Problem reading input bitcode/IR: " + SM.getMessage().str());
	}else{
		std::cout << "Successfully read bitcode/IR" << std::endl;
  }

	// 	PassRegistry &Registry = *PassRegistry::getPassRegistry();
	// 	initializeAnalysis(Registry);
		// InitialPasses.add(llvm::createCFGSimplificationPass());
		// InitialPasses.run(*M);
	// 	}


	return 0;
}

int Caller::callPass(){

  	AnalysisPasses.add(new FuncPass());
    AnalysisPasses.add(new StorePass());
    AnalysisPasses.add(new AllocaPass());

  	AnalysisPasses.run(*M);

	return 1;
}


void Caller::genByteCodeFile() {

  /* Generates an file (named output.bc) that contains the LLVM IR
    after executing the passes
  */

  const char *Filename = "output.bc";
  errs() << "";
  std::error_code EC;
  llvm::raw_fd_ostream file_descriptor(Filename, EC, llvm::sys::fs::F_None  );

  WriteBitcodeToFile(&(*M), file_descriptor);
  file_descriptor.flush();
}

// TODO: Implement using lllvm/clang api
void Caller::linkLLVM() {
  /* Link functions called after executing the passes */

  const char* command = "./bin/llvm-link output.bc lib/utils.bc > inter.bc";
  system(command);

  const char* command2 = "./bin/llvm-link inter.bc lib/memoryutils.bc > result.bc";
  system(command2);

  #ifndef DEBUG
  system("rm inter.bc");
  system("rm output.bc");
  #endif
}

// TODO: Implement using klee api
void Caller::callKlee() {
  /* Execute klee */

  const char* klee_command = "./bin/klee  result.bc";
  system(klee_command);
}
