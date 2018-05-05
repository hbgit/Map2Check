#include "caller.hpp"

// C Libs
#include <stdlib.h>
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

// CPP Libs
#include <iostream>
#include <string>
#include <regex>
#include <boost/filesystem.hpp>

// Project Libs
// #include "../backend/pass/MemoryTrackPass.h"
// #include "../backend/pass/GenerateAutomataTruePass.h"
// #include "../backend/pass/TrackBasicBlockPass.hpp"
// #include "../backend/pass/NonDetPass.hpp"
// #include "../backend/pass/Map2CheckLibrary.hpp"
// #include "../backend/pass/TargetPass.h"
// #include "../backend/pass/OverflowPass.h"
#include "utils/log.hpp"
#include "utils/tools.hpp"

namespace fs = boost::filesystem;

namespace {
static inline void check(std::string E) {
  if (!E.empty()) {
    if (llvm::errs().has_colors())
      llvm::errs().changeColor(raw_ostream::RED);

    llvm::errs() << E << "\n";

    if (llvm::errs().has_colors())
      llvm::errs().resetColor();
    exit(1);
  }
}
/* TODO(rafa.sa.xp@gmail.com) Should put this in Caller class
 * but including<llvm/IR/Module.h> on header file gives error */
std::unique_ptr<llvm::Module> M;  //!< Current module */
}  // namespace



Caller::Caller(std::string bcprogram_path) {
  this->cleanGarbage();
  this->pathprogram = bcprogram_path;
}

std::string Caller::preOptimizationFlags() {

}

std::string Caller::postOptimizationFlags() {
  
}

void Caller::cleanGarbage() {
  
}

void Caller::parseIrFile() {

}

int Caller::callPass(Map2CheckMode mode, bool sv_comp) {
  return 1;
}


int Caller::callPass(Map2CheckMode mode, std::string target_function,
                     bool sv_comp) {
  
}

void Caller::generateProcessedByteCodeFile() {

}

void Caller::linkLLVM() {

}

void Caller::callKlee() {

}

std::vector<int> Caller::processClangOutput() {

}

string Caller::compileCFile(std::string cprogram_path) {

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
