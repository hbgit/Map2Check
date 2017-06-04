//Map2Check library
#include "../backend/pass/MemoryTrackPass.h"

#include "caller.hpp"

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

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <iostream>
#include <string>
#include <regex>
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
    this->cleanGarbage();
  this->pathprogram = bcprogram_path;  

}

void Caller::cleanGarbage() {

   const char* command ="rm -rf klee-* *.log list-* clang.out \
                            *.csv map2check_property \
                            map2check_property_klee_unknown \
                            map2check_property_klee_deref \
                            map2check_property_klee_memtrack \
                            preprocessed.c \
                            map2check_property_klee_free \
                            optimized.bc output.bc inter.bc \
                            result.bc witnessInfo";
  system(command);
}
void Caller::printdata() {
  cout << "File Path:" << this->pathprogram << endl;
  this->parseIrFile();
}

int Caller::parseIrFile(){
  // Parse the input LLVM IR file into a module.
  Map2Check::Log::Debug("Parsing file " + this->pathprogram);
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

int Caller::callPass(bool sv_comp){

  AnalysisPasses.add(new MemoryTrackPass(sv_comp));
  // AnalysisPasses.add(llvm::op)
  AnalysisPasses.run(*M);

  return 1;
}

int Caller::callPass(std::string target_function, bool sv_comp){
  AnalysisPasses.add(new MemoryTrackPass(target_function, sv_comp));
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

  const char* command = "./bin/llvm-link output.bc lib/Map2CheckFunctions.bc lib/AllocationLog.bc lib/HeapLog.bc lib/Container.bc lib/KleeLog.bc lib/ListLog.bc lib/PropertyGenerator.bc> result.bc";
  system(command);

  // const char* command2 = "./bin/llvm-link inter.bc lib/memoryutils.bc > result.bc";
  // system(command2);

  const char* command3 = "./bin/opt -O3 result.bc > optimized.bc";
  system(command3);

  // system("rm inter.bc");
  // system("rm output.bc");

}

// TODO: Implement using klee api
void Caller::callKlee() {
  /* Execute klee */
  std::ostringstream command;
  command.str("");
  command << Map2Check::Tools::kleeBinary;
  command << " --allow-external-sym-calls  -exit-on-error-type=Assert --optimize optimized.bc  > ExecutionOutput.log";
  system(command.str().c_str());
}


string Caller::compileCFile(std::string cprogram_path) {
  Map2Check::Log::Info("Compiling " + cprogram_path);

  if(!fs::exists(Map2Check::Tools::clangBinary) ||
     !fs::is_regular_file(Map2Check::Tools::clangBinary)) {
    throw InvalidClangBinaryException();
  }

  if (!fs::exists(Map2Check::Tools::clangIncludeFolder) ||
      !fs::is_directory(Map2Check::Tools::clangIncludeFolder)) {
    throw InvalidClangIncludeException();
  }
  std::ostringstream commandRemoveExternMalloc;
  commandRemoveExternMalloc.str("");
  commandRemoveExternMalloc << "cat " << cprogram_path << " | ";
  commandRemoveExternMalloc << "sed -e 's/.*extern.*malloc.*/\\n/g' > preprocessed.c";
  //std::cout << "teste " << commandRemoveExternMalloc.str() << "\n";
  int resultRemove = system(commandRemoveExternMalloc.str().c_str());
  if(resultRemove == -1) {
    throw ErrorCompilingCProgramException();
  }
  std::ostringstream command;
  command.str("");
  command << Map2Check::Tools::clangBinary << " -I"
	  << Map2Check::Tools::clangIncludeFolder
          << " -c -emit-llvm -g -O0 "
          << " -o compiled.bc "
          << "preprocessed.c"
          << " >> clang.out";

  int result = system(command.str().c_str());
  if(result == -1) {
    throw ErrorCompilingCProgramException();
  }
  return ("compiled.bc");
}

const char* CallerException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}


const char* InvalidClangBinaryException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("Could not find clang binary");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}

const char* InvalidClangIncludeException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("Could not find clang include dir");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}

const char* ErrorCompilingCProgramException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("Error while compiling C program, check clang.out");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}
