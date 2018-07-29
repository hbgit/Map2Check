#include "caller.hpp"

#include <stdlib.h>
// CPP Libs
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
// #include <boost/filesystem.hpp>

#include "utils/log.hpp"
#include "utils/tools.hpp"

// namespace fs = boost::filesystem;
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

  system(removeCommand.str().c_str());
}

int Caller::callPass(Map2CheckMode mode, bool sv_comp) {
  std::ostringstream transformCommand;
  transformCommand.str("");
  transformCommand << Map2Check::optBinary;
  /* TODO(rafa.sa.xp@gmail.com): Should apply generate_automata_true
   *                             and TrackBasicBlockPass now */
  transformCommand << " -load ${MAP2CHECK_PATH}/lib/libNonDetPass.so "
                   << "-non_det < entry.bc > non_det.bc";

  system(transformCommand.str().c_str());

  transformCommand.str("");
  transformCommand << Map2Check::optBinary;
  /* TODO(rafa.sa.xp@gmail.com): Should apply generate_automata_true
   *                             and TrackBasicBlockPass now */
  transformCommand << " -load ${MAP2CHECK_PATH}/lib/libMap2CheckLibrary.so "
                   << "-map2check < non_det.bc > output.bc";

  system(transformCommand.str().c_str());

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
  linkCommand << Map2Check::llvmLinkBinary;
  linkCommand << " output.bc"
              << " ${MAP2CHECK_PATH}/lib/Map2CheckFunctions.bc"
              << " ${MAP2CHECK_PATH}/lib/AllocationLog.bc"
              << " ${MAP2CHECK_PATH}/lib/HeapLog.bc"
              << " ${MAP2CHECK_PATH}/lib/TrackBBLog.bc"
              // << " ${MAP2CHECK_PATH}/lib/BTree.bc"
              << " ${MAP2CHECK_PATH}/lib/Container.bc"
              << " ${MAP2CHECK_PATH}/lib/KleeLog.bc"
              << " ${MAP2CHECK_PATH}/lib/ListLog.bc"
              << " ${MAP2CHECK_PATH}/lib/PropertyGenerator.bc"
              // << " ${MAP2CHECK_PATH}/lib/BinaryOperation.bc "
              << "  > result.bc";

  system(linkCommand.str().c_str());

  // TODO(rafa.sa.xp@gmail.com) Check how optimization can generate errors
  std::ostringstream optimizeCommand;
  optimizeCommand.str("");
  optimizeCommand << Map2Check::optBinary;
  optimizeCommand << " " << Caller::postOptimizationFlags()
                  << " result.bc > optimized.bc";
}

void Caller::executeAnalysis() {}

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
                            << " > preprocessed.c";

  system(commandRemoveExternMalloc.str().c_str());

  std::ostringstream command;
  command.str("");
  command << Map2Check::clangBinary << " -I" << Map2Check::clangIncludeFolder
          << " -Wno-everything "
          << " -Winteger-overflow "
          << " -c -emit-llvm -g"
          << " " << Caller::preOptimizationFlags() << " -o compiled.bc "
          << "preprocessed.c"
          << " > clang.out 2>&1";

  system(command.str().c_str());
  return ("compiled.bc");
}
}  // namespace Map2Check
