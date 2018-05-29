#include "caller.hpp"

#include <stdlib.h>
// CPP Libs
#include <iostream>
#include <string>
#include <fstream>
#include <regex>
// #include <boost/filesystem.hpp>


#include "utils/log.hpp"
#include "utils/tools.hpp"

// namespace fs = boost::filesystem;

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

int Caller::callPass(Map2CheckMode mode, bool sv_comp) {
  std::ostringstream transformCommand;
  transformCommand.str("");
  transformCommand << Map2Check::Tools::optBinary;
  /* TODO(rafa.sa.xp@gmail.com): Should apply generate_automata_true 
   *                             and TrackBasicBlockPass now */
  transformCommand << " " 
           << " result.bc > optimized.bc";


  // Map2Check::Log::Debug("Applying NonDetPass\n");
  // AnalysisPasses.add(new NonDetPass());

  // // switch (mode) {
  // //   case (Map2CheckMode::MEMTRACK_MODE):
  // //     Map2Check::Log::Debug("Applying MemoryTrackPass\n");
  // //     AnalysisPasses.add(new MemoryTrackPass(sv_comp));
  // //     break;
  // //   case (Map2CheckMode::OVERFLOW_MODE):
  // //     Map2Check::Log::Debug("Applying OverflowPass\n");
  // //     {
  // //       std::vector<int> lines = this->processClangOutput();
  // //       AnalysisPasses.add(new OverflowPass(lines));
  // //     }
  // //     break;
  // // }

  // Map2Check::Log::Debug("Applying Map2CheckLibrary\n");
  // AnalysisPasses.add(new Map2CheckLibrary(sv_comp));
  // AnalysisPasses.run(*M);
  // Map2Check::Log::Debug("Finished instrumentation\n");

  return 1;
}


int Caller::callPass(Map2CheckMode mode, std::string target_function,
                     bool sv_comp) {
  return -1;
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
  Map2Check::Log::Info("Compiling " + cprogram_path);

  std::ostringstream commandRemoveExternMalloc;
  commandRemoveExternMalloc.str("");
  commandRemoveExternMalloc << "cat " << cprogram_path << " | ";
  commandRemoveExternMalloc << "sed -e 's/.*extern.*malloc.*//g' "
                            << "  -e 's/.*void \\*malloc(size_t size).*//g' "
                            <<" > preprocessed.c";

  int resultRemove = system(commandRemoveExternMalloc.str().c_str());

  std::ostringstream command;
  command.str("");
  command << Map2Check::Tools::clangBinary << " -I"
          << Map2Check::Tools::clangIncludeFolder
          << " -Wno-everything "
          << " -Winteger-overflow "
          << " -c -emit-llvm -g"
          << " " << Caller::preOptimizationFlags()
          << " -o compiled.bc "
          << "preprocessed.c"
          << " > clang.out 2>&1";

  int result = system(command.str().c_str());
  return ("compiled.bc");
}
