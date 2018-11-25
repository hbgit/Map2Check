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

bool isWitnessFileCreated() {
  Map2Check::Log::Info("Checking file");
  std::ifstream infile("map2check_checked_error");
  if (infile.is_open()) {
    Map2Check::Log::Info("Found file!");
    return true;
  }
  return false;
}
}  // namespace

namespace Map2Check {
Caller::Caller(std::string bc_program_path, Map2CheckMode mode,
               NonDetGenerator generator) {
  // this->cleanGarbage();
  this->pathprogram = bc_program_path;
  this->map2checkMode = mode;
  this->nonDetGenerator = generator;
  GenHash hash;
  hash.setFilePath(bc_program_path);
  hash.generate_sha1_hash_for_file();
  this->programHash = hash.getOutputSha1HashFile() + ".map2check";

  std::ostringstream createTempDir;
  createTempDir.str("");
  createTempDir << "mkdir " << programHash;
  system(createTempDir.str().c_str());

  std::ostringstream moveProgram;
  moveProgram << "cp " << bc_program_path << " " << programHash;
  system(moveProgram.str().c_str());

  Map2Check::Log::Debug("Changing current dir");
  currentPath = boost::filesystem::current_path().string();
  boost::filesystem::current_path(currentPath + "/" + programHash);
  Map2Check::Log::Debug("Current path: " +
                        boost::filesystem::current_path().string());
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
  flags << "-O2 ";
  return flags.str();
}

void Caller::cleanGarbage() {
  boost::filesystem::current_path(currentPath);
  std::ostringstream removeCommand;
  removeCommand.str("");
  removeCommand << "rm -rf " << programHash;
  Map2Check::Log::Debug("Remove " + removeCommand.str());
  system(removeCommand.str().c_str());
}

void Caller::applyNonDetGenerator() {
  switch (nonDetGenerator) {
    case (NonDetGenerator::None): {  // TODO: Should generate binary
      Map2Check::Log::Info(
          "Map2Check will not generate non deterministic numbers");
      break;
    }
    case (NonDetGenerator::Klee): {
      Map2Check::Log::Info("Applying optimizations for klee");
      break;
    }
    case (NonDetGenerator::LibFuzzer): {
      Map2Check::Log::Info("Instrumenting with LLVM LibFuzzer");
      std::ostringstream command;
      command.str("");

      command
          << Map2Check::clangBinary
          << "  -g -fsanitize=fuzzer -fsanitize-coverage=inline-8bit-counters "
          << Caller::postOptimizationFlags()
          << " -o " + programHash + "-fuzzed.out"
          << " " + programHash + "-result.bc";

      system(command.str().c_str());

      std::ostringstream commandWitness;
      commandWitness.str("");
      commandWitness << Map2Check::clangBinary << "  -g -fsanitize=fuzzer "
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
  transformCommand << " -tailcallopt";
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
      std::string overflowPass = "${MAP2CHECK_PATH}/lib/libOverflowPass";
      transformCommand << " -load " << overflowPass << getLibSuffix()
                       << " -check_overflow";
      break;
    }
    case Map2CheckMode::REACHABILITY_MODE: {
      Map2Check::Log::Info("Running reachability mode");
      std::string targetPass = "${MAP2CHECK_PATH}/lib/libTargetPass";
      transformCommand << " -load " << targetPass << getLibSuffix()
                       << " -target_function";

      break;
    }
    case Map2CheckMode::ASSERT_MODE: {
      Map2Check::Log::Info("Running assert mode");
      std::string assertPass = "${MAP2CHECK_PATH}/lib/libAssertPass";
      transformCommand << " -load " << assertPass << getLibSuffix()
                       << " -validate_asserts";

      break;
    }
    default: { break; }
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
              << " ${MAP2CHECK_PATH}/lib/NonDetLog.bc"
              << " ${MAP2CHECK_PATH}/lib/PropertyGenerator.bc";

  switch (dataStructure) {
    case DataStructure::Array: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/ContainerRealloc.bc";
      break;
    }
    case DataStructure::BTree: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/ContainerBTree.bc"
                  << " ${MAP2CHECK_PATH}/lib/BTree.bc";
      break;
    }
  }

  switch (map2checkMode) {
    case Map2CheckMode::MEMTRACK_MODE: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeMemtrack.bc"
                  << " ${MAP2CHECK_PATH}/lib/AllocationLog.bc"
                  << " ${MAP2CHECK_PATH}/lib/ListLog.bc"
                  << " ${MAP2CHECK_PATH}/lib/HeapLog.bc";
      break;
    }
    case Map2CheckMode::OVERFLOW_MODE: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeOverflow.bc";
      break;
    }
    case Map2CheckMode::ASSERT_MODE: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeAssert.bc";
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
    case (NonDetGenerator::Klee): {  // TODO: Add klee non det generator
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorKlee.bc";
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
    case (NonDetGenerator::None): {  // TODO: Activate mode
      Map2Check::Log::Info("This mode is not supported");
      break;
    }
    case (NonDetGenerator::Klee): {
      Map2Check::Log::Info("Executing Klee with map2check");
      std::ostringstream kleeCommand;
      kleeCommand.str("");
      kleeCommand << "timeout " << (0.8 * this->timeout) << " ";
      kleeCommand << Map2Check::kleeBinary;
      kleeCommand << " -suppress-external-warnings"
                  << " --allow-external-sym-calls"
                  << " -exit-on-error-type=Abort"
                  << " --optimize "
                  << " -solver-backend=z3 "
                  << " -libc=uclibc"
                  << " ./" + programHash + "-witness-result.bc"
                  << "  > ExecutionOutput.log";
      Map2Check::Log::Debug(kleeCommand.str());
      int result = system(kleeCommand.str().c_str());
      Map2Check::Log::Warning("Exited klee with " + std::to_string(result));
      if (result == 31744)  // Timeout
        gotTimeout = true;

      break;
    }
    case (NonDetGenerator::LibFuzzer): {
      Map2Check::Log::Info("Executing LibFuzzer with map2check");
      std::ostringstream command;
      command.str("");
      command << "timeout " << (0.2 * this->timeout) << " ";
      command << "./" + programHash +
                     "-fuzzed.out -jobs=2 -use_value_profile=1 "
              << " > fuzzer.output";

      int result = system(command.str().c_str());
      Map2Check::Log::Warning("Exited fuzzer with " + std::to_string(result));
      if (result == 31744)  // Timeout
        gotTimeout = true;

      std::ostringstream commandWitness;
      commandWitness.str("");
      commandWitness << "./" + programHash + "-witness-fuzzed.out crash-*";
      system(commandWitness.str().c_str());
      Map2Check::Log::Debug("Finished fuzzer");

      if (isWitnessFileCreated()) {
        witnessVerified = true;
      }

      break;
    }
  }
  if (isWitnessFileCreated()) {
    witnessVerified = true;
  }
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

  // This regex captures accused line number for overflow warnings (from clang)
  regex overflowWarning(
      ".*:([[:digit:]]+):[[:digit:]]+:.*(Winteger-overflow).*");
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
 * (1) Remove unsupported functions and clean the C code
 * (2) Generate .bc file from code
 * (3) Check for overflow errors on compilation
 */
void Caller::compileCFile(bool is_llvm_bc) {

  if(!is_llvm_bc){
  Map2Check::Log::Info("Compiling " + this->pathprogram);

  // (1) Remove unsupported functions and clean the C code
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
  }else{
      std::string compiledFile = programHash + "-compiled.bc";
      this->pathprogram = compiledFile;
  }

  // TODO: (3) Check for overflow errors on compilation
}

void Caller::compileToCrabLlvm() {
  Map2Check::Log::Info("Compiling using crab-llvm in " + this->pathprogram);

  // (1) Remove unsupported functions and clean the C code
  std::ostringstream commandRemoveExternMalloc;
  commandRemoveExternMalloc.str("");
  commandRemoveExternMalloc << "cat " << this->pathprogram << " | ";
  commandRemoveExternMalloc << "sed -e 's/.*extern.*malloc.*/ / g' "
                            << "  -e 's/.*void \\*malloc(size_t size).*//g' "
                            << " > " << programHash << "-preprocessed.c ";

  system(commandRemoveExternMalloc.str().c_str());

  // (2) Generate .bc file from code
  // TODO: -Winteger-overflow should be called only if is on overflow mode
  // CLANG PATH
  std::ostringstream getPathCLCommand;
  getPathCLCommand.str("");
  std::ostringstream getMapPath;
  getMapPath << getenv("MAP2CHECK_PATH");

  getPathCLCommand << "CLANG_PATH=" << getMapPath.str().c_str() << "/bin";

  std::string tmp_gpcc = getPathCLCommand.str().c_str();
  char* c_gpcc = new char[tmp_gpcc.length() + 1];
  std::copy(tmp_gpcc.c_str(), tmp_gpcc.c_str() + tmp_gpcc.length() + 1, c_gpcc);
  putenv(c_gpcc);

  // Export libCrab.so from CrabLLVM
  std::ostringstream getPathLibCrabCommand;
  getPathLibCrabCommand.str("");

  getPathLibCrabCommand << "LD_LIBRARY_PATH=" << getMapPath.str().c_str()
                        << "/bin/crabllvm/lib";

  std::string tmp_gplibcc = getPathLibCrabCommand.str().c_str();
  char* c_gplibcc = new char[tmp_gplibcc.length() + 1];
  std::copy(tmp_gplibcc.c_str(), tmp_gplibcc.c_str() + tmp_gplibcc.length() + 1,
            c_gplibcc);
  putenv(c_gplibcc);

  std::string compiledFile = programHash + "-compiled.bc";
  std::ostringstream command;
  command.str("");
  command << Map2Check::crabBinary << " -o " << compiledFile
          << " -m 64 -g --crab-disable-warnings --disable-lower-gv "
             "--llvm-pp-loops --crab-promote-assume --crab-inter "
             "--crab-track=num --crab-add-invariants=block-entry "
          << " " << programHash << "-preprocessed.c ";

  //<< " -m 64 --crab-dom=oct --crab-track=num --crab-add-invariants=all "

  system(command.str().c_str());

  this->pathprogram = compiledFile;
}

}  // namespace Map2Check
