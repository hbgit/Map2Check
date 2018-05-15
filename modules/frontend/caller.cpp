#include "caller.hpp"

#include <stdlib.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/raw_ostream.h>
// CPP Libs
#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <boost/filesystem.hpp>


// Project Libs
// #include "../backend/pass/MemoryTrackPass.h"
// #include "../backend/pass/GenerateAutomataTruePass.h"
// #include "../backend/pass/TrackBasicBlockPass.hpp"
#include "../backend/pass/NonDetPass.hpp"
#include "../backend/pass/Map2CheckLibrary.hpp"
// #include "../backend/pass/TargetPass.h"
// #include "../backend/pass/OverflowPass.h"
#include "utils/log.hpp"
#include "utils/tools.hpp"

namespace fs = boost::filesystem;

// namespace {
// static inline void check(std::string E) {
//   if (!E.empty()) {
//     if (llvm::errs().has_colors())
//       llvm::errs().changeColor(raw_ostream::RED);

//     llvm::errs() << E << "\n";

//     if (llvm::errs().has_colors())
//       llvm::errs().resetColor();
//     exit(1);
//   }
// }
// }  // namespace


namespace Map2Check {
Caller::Caller(std::string bcprogram_path) {
  this->cleanGarbage();
  this->pathprogram = bcprogram_path;
}

std::string Caller::preOptimizationFlags() {
  std::ostringstream flags;
  flags.str("");
  flags << "-O0";
  return flags.str();
}

std::string Caller::postOptimizationFlags() {
  std::ostringstream flags;
  flags.str("");
  flags << "-O3";
  return flags.str();
}

void Caller::cleanGarbage() {
  std::ostringstream removeCommand;
  removeCommand.str("");
  removeCommand << "rm -rf"
              << " klee-*"
              << " *.log"
              << " list-*"
              << " *.csv"
              << " map2check_property"
              << " automata_list_log.st"
              << " track_bb_log.st"
              << " map2check_property_klee_unknown"
              << " map2check_property_klee_deref"
              << " map2check_property_klee_memtrack"
              << " map2check_property_overflow"
              << " map2check_property_klee_free"
              << " preprocessed.c"
              << " optimized.bc"
              << " output.bc"
              << " witnessInfo";

  int result = system(removeCommand.str().c_str());
}

void Caller::parseIrFile() {
  Map2Check::Log::Debug("Parsing file " + this->pathprogram);
  llvm::StringRef filename = this->pathprogram;

  llvm::SMDiagnostic SM;
  llvm::LLVMContext Context;
  M = llvm::parseIRFile(filename, SM, Context);
}

int Caller::callPass(Map2CheckMode mode, bool sv_comp) {
  llvm::legacy::PassManager AnalysisPasses;
  // Pass to generate_automata_true
  // Map2Check::Log::Debug("Applying GenerateAutomataTruePass\n");
  // AnalysisPasses.add(new GenerateAutomataTruePass(this->cprogram_fullpath));

  /* TrackBasicBlockPass is here to avoid lost
     position after code instrumentation */
  // Map2Check::Log::Debug("Applying TrackBasicBlockPass\n");
  // AnalysisPasses.add(new TrackBasicBlockPass(this->cprogram_fullpath));

  Map2Check::Log::Debug("Applying NonDetPass\n");
  AnalysisPasses.add(new NonDetPass());

  // switch (mode) {
  //   case (Map2CheckMode::MEMTRACK_MODE):
  //     Map2Check::Log::Debug("Applying MemoryTrackPass\n");
  //     AnalysisPasses.add(new MemoryTrackPass(sv_comp));
  //     break;
  //   case (Map2CheckMode::OVERFLOW_MODE):
  //     Map2Check::Log::Debug("Applying OverflowPass\n");
  //     {
  //       std::vector<int> lines = this->processClangOutput();
  //       AnalysisPasses.add(new OverflowPass(lines));
  //     }
  //     break;
  // }

  Map2Check::Log::Debug("Applying Map2CheckLibrary\n");
  AnalysisPasses.add(new Map2CheckLibrary(sv_comp));
  AnalysisPasses.run(*M);
  Map2Check::Log::Debug("Finished instrumentation\n");

  return 1;
}


int Caller::callPass(Map2CheckMode mode, std::string target_function,
                     bool sv_comp) {
  return -1;
}

void Caller::generateProcessedByteCodeFile() {
  const char *Filename = "output.bc";
  errs() << "";
  std::error_code EC;
  llvm::raw_fd_ostream file_descriptor(Filename, EC, llvm::sys::fs::F_None);
  llvm::WriteBitcodeToFile(&(*M), file_descriptor);
  file_descriptor.flush();
}

void Caller::linkLLVM() {
  /* Link functions called after executing the passes */
  // TODO(rafa.sa.xp@gmail.com) Only link against used libraries

  std::ostringstream linkCommand;
  linkCommand.str("");
  linkCommand << Map2Check::Tools::llvmLinkBinary;
  linkCommand
      << " output.bc"
      << " ${MAP2CHECK_PATH}/lib/Map2CheckFunctions.bc"
      << " ${MAP2CHECK_PATH}/lib/AllocationLog.bc"
      << " ${MAP2CHECK_PATH}/lib/HeapLog.bc"
      // << " ${MAP2CHECK_PATH}/lib/TrackBBLog.bc"
      // << " ${MAP2CHECK_PATH}/lib/BTree.bc"
      << " ${MAP2CHECK_PATH}/lib/Container.bc"
      << " ${MAP2CHECK_PATH}/lib/KleeLog.bc"
      << " ${MAP2CHECK_PATH}/lib/ListLog.bc"
      // << " ${MAP2CHECK_PATH}/lib/PropertyGenerator.bc"
      // << " ${MAP2CHECK_PATH}/lib/BinaryOperation.bc "
      << "  > result.bc";

  // TODO(rafa.sa.xp@gmail.com) Check how optimization can generate errors
  std::ostringstream optimizeCommand;
  optimizeCommand.str("");
  optimizeCommand << Map2Check::Tools::optBinary;
  optimizeCommand << " " << Caller::postOptimizationFlags()
           << " result.bc > optimized.bc";

}

void Caller::executeAnalysis() {

}

std::vector<int> Caller::processClangOutput() {
  const char* path_name = "clang.out";

  std::vector<int> result;

  ifstream in(path_name);
  if (!in.is_open()) {
    Map2Check::Log::Debug("Clang did not generate warning or errors");
    return result;
  }

  Map2Check::Log::Debug("Clang generate warning or errors");

  regex overflowWarning(".*:([[:digit:]]+):[[:digit:]]+:.*Winteger-overflow.*");
  string line;
  smatch match;
  while (getline(in, line)) {
    if (std::regex_search(line, match, overflowWarning) && match.size() > 1) {
      Map2Check::Log::Info("Found warning at line " + match[1].str());
      int lineNumber = std::stoi(match[1].str());
      result.push_back(lineNumber);
    }
  }

  return result;
}

string Caller::compileCFile(std::string cprogram_path) {
  // Map2Check::Log::Info("Compiling " + cprogram_path);

  // std::ostringstream commandRemoveExternMalloc;
  // commandRemoveExternMalloc.str("");
  // commandRemoveExternMalloc << "cat " << cprogram_path << " | ";
  // commandRemoveExternMalloc << "sed -e 's/.*extern.*malloc.*//g' "
  //                           << "  -e 's/.*void \\*malloc(size_t size).*//g' "
  //                           <<" > preprocessed.c";

  // int resultRemove = system(commandRemoveExternMalloc.str().c_str());
  // if (resultRemove == -1) {
  //   throw ErrorCompilingCProgramException();
  // }
  // std::ostringstream command;
  // command.str("");
  // command << Map2Check::Tools::clangBinary << " -I"
  //         << Map2Check::Tools::clangIncludeFolder
  //         << " -Wno-everything "
  //         << " -Winteger-overflow "
  //         << " -c -emit-llvm -g"
  //         << " " << Caller::preOptimizationFlags()
  //         << " -o compiled.bc "
  //         << "preprocessed.c"
  //         << " > clang.out 2>&1";

  // int result = system(command.str().c_str());
  return ("compiled.bc");
}

// const char* CallerException::what() const throw() {
//   std::ostringstream cnvt;
//   cnvt.str("");
//   cnvt << runtime_error::what();
//   Map2Check::Log::Error(cnvt.str());
//   return cnvt.str().c_str();
// }

// const char* InvalidClangBinaryException::what() const throw() {
//   std::ostringstream cnvt;
//   cnvt.str("Could not find clang binary");
//   cnvt << runtime_error::what();
//   Map2Check::Log::Error(cnvt.str());
//   return cnvt.str().c_str();
// }

// const char* InvalidClangIncludeException::what() const throw() {
//   std::ostringstream cnvt;
//   cnvt.str("Could not find clang include dir");
//   cnvt << runtime_error::what();
//   Map2Check::Log::Error(cnvt.str());
//   return cnvt.str().c_str();
// }

// const char* ErrorCompilingCProgramException::what() const throw() {
//   std::ostringstream cnvt;
//   cnvt.str("Error while compiling C program, check clang.out");
//   cnvt << runtime_error::what();
//   Map2Check::Log::Error(cnvt.str());
//   return cnvt.str().c_str();
// }
}  // namespace Map2Check
