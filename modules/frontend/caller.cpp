#include "caller.hpp"

#include <stdlib.h>
// CPP Libs
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include <boost/filesystem.hpp>

#include "utils/gen_crypto_hash.hpp"
#include "utils/log.hpp"
#include "utils/tools.hpp"
// namespace fs = boost::filesystem;
// }  // namespace

namespace {
inline std::string getLibSuffix() { return ".so"; }
}

namespace Map2Check {
Caller::Caller(std::string bcprogram_path, Map2CheckMode mode,
               NonDetGenerator generator) {
  // this->cleanGarbage();
  this->pathprogram = bcprogram_path;
  this->map2checkMode = mode;
  this->nonDetGenerator = generator;
  GenHash hash;
  hash.setFilePath(bcprogram_path);
  hash.generate_sha1_hash_for_file();
  this->programHash = hash.getOutputSha1HashFile();

  std::ostringstream createTempDir;
  createTempDir.str("");
  createTempDir << "mkdir " << programHash;
  system(createTempDir.str().c_str());

  std::ostringstream moveProgram;
  moveProgram << "cp " << bcprogram_path << " " << programHash;
  system(moveProgram.str().c_str());

  Map2Check::Log::Debug("Changing current dir");
  std::string currentPath = boost::filesystem::current_path().string();
  boost::filesystem::current_path(currentPath + "/" + programHash);
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
  removeCommand << "rm -rf ../" << programHash;
  system(removeCommand.str().c_str());
}

void Caller::applyNonDetGenerator() {
  switch (nonDetGenerator) {
  case (NonDetGenerator::None): {
    Map2Check::Log::Info(
        "Map2Check will not generate non deterministic numbers");
    break;
  }
  case (NonDetGenerator::LibFuzzer): {
    Map2Check::Log::Info("Instrumenting with LLVM Libfuzzer");
    std::ostringstream command;
    command.str("");
    command << Map2Check::clangBinary << " -g -fsanitize=fuzzer "
            << " -o " + programHash + "-fuzzed.out"
            << " " + programHash + "-result.bc";

    system(command.str().c_str());

    std::ostringstream commandWitness;
    commandWitness.str("");
    commandWitness << Map2Check::clangBinary << " -g -fsanitize=fuzzer "
                   << " -o " + programHash + "-witness-fuzzed.out"
                   << " " + programHash + "-witness-result.bc";

    system(commandWitness.str().c_str());
    break;
  }
  }
}

int Caller::callPass(std::string target_function, bool sv_comp) {
  std::ostringstream transformCommand;
  transformCommand.str("");
  transformCommand << Map2Check::optBinary;
  /* TODO(rafa.sa.xp@gmail.com): Should apply generate_automata_true
   *                             and TrackBasicBlockPass now */

  std::string nonDetPass = "${MAP2CHECK_PATH}/lib/libNonDetPass";

  Map2Check::Log::Info("Adding nondet pass");
  transformCommand << " -load " << nonDetPass << getLibSuffix() << " -non_det";

  switch (map2checkMode) {
  case Map2CheckMode::MEMTRACK_MODE: {
    Map2Check::Log::Info("Adding memtrack pass");
    std::string memoryTrackPass = "${MAP2CHECK_PATH}/lib/libMemoryTrackPass";
    transformCommand << " -load " << memoryTrackPass << getLibSuffix()
                     << " -memory_track";
    break;
  }
  case Map2CheckMode::OVERFLOW_MODE: {
    Map2Check::Log::Error(
        "Overflow mode is not implemented yet, ignoring option");
    break;
  }
  case Map2CheckMode::REACHABILITY_MODE: {
    Map2Check::Log::Info("Running reachability mode");
    std::string targetPass = "${MAP2CHECK_PATH}/lib/libTargetPass";
    transformCommand << " -load " << targetPass << getLibSuffix()
                     << " -target_function";

    break;
  }
  }

  Map2Check::Log::Info("Adding map2check pass");
  std::string map2checkPass = "${MAP2CHECK_PATH}/lib/libMap2CheckLibrary";
  transformCommand << " -load " << map2checkPass << getLibSuffix()
                   << " -map2check ";

  std::string input_file = "< " + this->pathprogram;
  std::string output_file = "> " + programHash + "-output.bc";

  transformCommand << input_file << output_file;
  Map2Check::Log::Debug(transformCommand.str());

  system(transformCommand.str().c_str());

  return 1;
}

void Caller::linkLLVM() {
  /* Link functions called after executing the passes */
  // TODO(rafa.sa.xp@gmail.com) Only link against used libraries

  Map2Check::Log::Info("Linking with map2check library");

  std::ostringstream witnessCommand;
  std::ostringstream linkCommand;
  linkCommand.str("");
  linkCommand << Map2Check::llvmLinkBinary;
  linkCommand << " " + programHash + "-output.bc"
              << " ${MAP2CHECK_PATH}/lib/Map2CheckFunctions.bc"
              << " ${MAP2CHECK_PATH}/lib/TrackBBLog.bc"
              // TODO: add option for container type
              << " ${MAP2CHECK_PATH}/lib/ContainerRealloc.bc"
              << " ${MAP2CHECK_PATH}/lib/NonDetLog.bc"
              << " ${MAP2CHECK_PATH}/lib/PropertyGenerator.bc";

  switch (map2checkMode) {
  case Map2CheckMode::MEMTRACK_MODE: {
    linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeMemtrack.bc"
                << " ${MAP2CHECK_PATH}/lib/AllocationLog.bc"
                << " ${MAP2CHECK_PATH}/lib/ListLog.bc"
                << " ${MAP2CHECK_PATH}/lib/HeapLog.bc";
    break;
  }
  case Map2CheckMode::OVERFLOW_MODE: {
    Map2Check::Log::Error(
        "Overflow mode is not implemented yet, ignoring option");
    break;
  }
  case Map2CheckMode::REACHABILITY_MODE: {
    // Since the map2check api provides the function, we do not need to do any
    // analysis
    linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeNone.bc";
    break;
  }
  }

  switch (nonDetGenerator) {
  case (NonDetGenerator::None): {
    linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorNone.bc";
    break;
  }
  case (NonDetGenerator::LibFuzzer): {
    linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorLibFuzzy.bc";
    break;
  }
  }

  witnessCommand.str("");
  witnessCommand << linkCommand.str();
  witnessCommand << " ${MAP2CHECK_PATH}/lib/WitnessGeneration.bc";
  witnessCommand << "  > " + programHash + "-witness-result.bc";
  Map2Check::Log::Debug(witnessCommand.str());
  system(witnessCommand.str().c_str());

  linkCommand << " ${MAP2CHECK_PATH}/lib/WitnessGenerationNone.bc";
  linkCommand << "  > " + programHash + "-result.bc";
  Map2Check::Log::Debug(linkCommand.str());
  system(linkCommand.str().c_str());
}

void Caller::executeAnalysis() {
  switch (nonDetGenerator) {
  // TODO: implement this method
  case (NonDetGenerator::None): {
    Map2Check::Log::Info("This mode is not supported");
    break;
  }
  case (NonDetGenerator::LibFuzzer): {
    Map2Check::Log::Info("Executing libfuzzer with map2check");
    std::ostringstream command;
    command.str("");
    command << "./" + programHash + "-fuzzed.out -jobs=2 "
            << " -timeout=" << this->timeout << " > fuzzer.output";
    system(command.str().c_str());

    std::ostringstream commandWitness;
    commandWitness.str("");
    commandWitness << "./" + programHash + "-witness-fuzzed.out crash-*";
    system(commandWitness.str().c_str());
    break;
  }
  }
}

std::vector<int> Caller::processClangOutput() {
  const char *path_name = "clang.out";

  std::vector<int> result;

  ifstream in(path_name);
  if (!in.is_open()) {
    Map2Check::Log::Debug("Clang did not generate warning or errors");
    return result;
  }
  Map2Check::Log::Debug("Clang generate warning or errors");

  // This regex captures accused line number for overflow warnings (from clang)
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

/** This function should:
 * (1) Remove unsuported functions and clean the C code
 * (2) Generate .bc file from code
 * (3) Check for overflow errors on compilation
 */
void Caller::compileCFile() {
  Map2Check::Log::Info("Compiling " + this->pathprogram);

  // (1) Remove unsuported functions and clean the C code
  std::ostringstream commandRemoveExternMalloc;
  commandRemoveExternMalloc.str("");
  commandRemoveExternMalloc << "cat " << this->pathprogram << " | ";
  commandRemoveExternMalloc << "sed -e 's/.*extern.*malloc.*/ / g' "
                            << "  -e 's/.*void \\*malloc(size_t size).*//g' "
                            << " > " << programHash << "-preprocessed.c ";

  system(commandRemoveExternMalloc.str().c_str());

  // (2) Generate .bc file from code
  // TODO: -Winteger-overflow should be called only if is on overflow mode
  std::string compiledFile = programHash + "-compiled.bc";
  std::ostringstream command;
  command.str("");
  command << Map2Check::clangBinary << " -I" << Map2Check::clangIncludeFolder
          << " -Wno-everything "
          << " -Winteger-overflow "
          << " -c -emit-llvm -g"
          << " " << Caller::preOptimizationFlags() << " -o " << compiledFile
          << " " << programHash << "-preprocessed.c "
          << " > " << programHash << "-clang.out 2>&1";

  system(command.str().c_str());

  this->pathprogram = compiledFile;

  // TODO: (3) Check for overflow errors on compilation
}
} // namespace Map2Check
