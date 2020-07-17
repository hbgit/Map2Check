/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * Map2Check -> GPL-2.0
 * CLANG     -> Apache-2.0
 * KLEE      -> NCSA
 * CRAB-LLVM -> Apache-2.0
 * STP,Z3 -> MIT
 *
 * SPDX-License-Identifier: (GPL-2.0 AND Apache-2.0 AND NCSA AND MIT)
 **/

#include "caller.hpp"

#include <stdlib.h>
// CPP Libs
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "utils/gen_crypto_hash.hpp"
#include "utils/log.hpp"
#include "utils/tools.hpp"
// namespace fs = boost::filesystem;
// }  // namespace

using std::ifstream;
using std::regex;
using std::smatch;

namespace {
inline std::string getLibSuffix() { return ".so"; }

bool isWitnessFileCreated() {
  Map2Check::Log::Debug("Checking file");
  std::ifstream infile("map2check_checked_error");
  if (infile.is_open()) {
    Map2Check::Log::Debug("Found file!");
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
  
  currentPath = boost::filesystem::current_path().string();
  boost::filesystem::current_path(currentPath + "/" + programHash);
  Map2Check::Log::Debug("Changing to Map2Check tmp files dir: " +
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
    case (NonDetGenerator::None): {  // TODO(hbgit): Should generate binary
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
          
      Map2Check::Log::Debug(command.str().c_str());
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

int Caller::checkNondetFunctPass() {
  std::ostringstream cmd;
  cmd.str("");
  cmd << Map2Check::optBinary;

  Map2Check::Log::Info("Look for nondet function call using LLVM pass");
  cmd << " -tailcallopt";
  std::string checkNondetFunctPass = "${MAP2CHECK_PATH}/lib/libCheckNonDetFunctPass";
  cmd << " -load " << checkNondetFunctPass << getLibSuffix()
                   << " -check_nondet_functs -disable-output";

  std::string input_file = "< " + this->pathprogram;
  std::string output_file = " 2> checkNondetFunct-output.txt";

  cmd << input_file << output_file;
  Map2Check::Log::Debug(cmd.str());

  system(cmd.str().c_str());

  std::ifstream file("checkNondetFunct-output.txt");

  // pFile.peek() == std::ifstream::traits_type::eof()
  if ( file.peek() == std::ifstream::traits_type::eof() ) {
    return 0;
  } else {
    return 1;
  }  
}

int Caller::callPass(std::string target_function, bool sv_comp) {
  std::ostringstream transformCommand;
  transformCommand.str("");
  transformCommand << Map2Check::optBinary;
  /* TODO(rafa.sa.xp@gmail.com): Should apply generate_automata_true
   *                             and TrackBasicBlockPass now */

  std::string nonDetPass = "${MAP2CHECK_PATH}/lib/libNonDetPass";

  /* Map2Check::Log::Info("Adding loop pass");
  std::string loopPredAssumePass =
  "${MAP2CHECK_PATH}/lib/libLoopPredAssumePass"; transformCommand << " -load "
  << loopPredAssumePass << getLibSuffix()
                   << " -loop_predicate_assume";*/

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
    case Map2CheckMode::MEMCLEANUP_MODE: {
      Map2Check::Log::Info("Adding memcleanup pass");
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
      Map2Check::Log::Info("Adding reachability pass");
      std::string targetPass = "${MAP2CHECK_PATH}/lib/libTargetPass";
      transformCommand << " -load " << targetPass << getLibSuffix()
                       << " -target_function";

      break;
    }
    case Map2CheckMode::ASSERT_MODE: {
      Map2Check::Log::Info("Adding assert pass");
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

  Map2Check::Log::Info("Linking with map2check library using llvm-link");

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
    case Map2CheckMode::MEMCLEANUP_MODE: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeMemcleanup.bc"
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
    case (NonDetGenerator::Klee): {  // TODO(hbgit): Add klee non det generator
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorKlee.bc";
      break;
    }
    case (NonDetGenerator::LibFuzzer): {
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorLibFuzzy.bc";
      break;
    }
  }

  Map2Check::Log::Debug("Compiling to generate the witness");
  witnessCommand.str("");
  witnessCommand << linkCommand.str();
  witnessCommand << " ${MAP2CHECK_PATH}/lib/WitnessGeneration.bc";
  witnessCommand << "  > " + programHash + "-witness-result.bc";
  Map2Check::Log::Debug(witnessCommand.str());
  system(witnessCommand.str().c_str());

  Map2Check::Log::Debug("Compiling to verify the properties");
  linkCommand << " ${MAP2CHECK_PATH}/lib/WitnessGenerationNone.bc";
  linkCommand << "  > " + programHash + "-result.bc";
  Map2Check::Log::Debug(linkCommand.str());
  system(linkCommand.str().c_str());
}

void Caller::executeAnalysis(std::string solvername) {
  switch (nonDetGenerator) {
    // TODO(hbgit): implement this method
    case (NonDetGenerator::None): {  // TODO(hbgit): Activate mode
      Map2Check::Log::Info("This mode is not supported");
      break;
    }
    case (NonDetGenerator::Klee): {
      Map2Check::Log::Info("Executing Klee with map2check");
      std::ostringstream kleeCommand;
      kleeCommand.str("");
      kleeCommand << "timeout " << (0.8 * this->timeout) << " ";
      kleeCommand << Map2Check::kleeBinary;


      std::vector<std::string> kleebackendsolver = {"z3", "stp"};
      std::vector<std::string> kleemetasolver = {"btor", "yices2"};


      if ( std::count(kleebackendsolver.begin(), kleebackendsolver.end(), solvername) ) {
        // Checkout solver adopted, if is z3 or stp
        // in KLEE add -solver-backend option

        Map2Check::Log::Info("Solver backend caller: " + solvername);
        //  --allow-external-sym-calls
        //  -use-cache
        kleeCommand << " -suppress-external-warnings"
                    << " --external-calls=all"
                    << " -exit-on-error-type=Abort"
                    << " --optimize"
                    << " -use-cex-cache"
                    << " -solver-backend=" + solvername + " "
                    << " -use-construct-hash-metasmt "
                    << " -libc=uclibc"
                    << " ./" + programHash + "-witness-result.bc"
                    << "  > ExecutionOutput.log";
      } else if ( std::count(kleemetasolver.begin(), kleemetasolver.end(), solvername) ) {
        // Checkout solver adopted, if is btor (Boolector) or yices (Yices)
        // in KLEE add - option
        Map2Check::Log::Info("Solver metaSMT caller: " + solvername);

        kleeCommand << " -suppress-external-warnings"
                    << " --external-calls=all"
                    << " -exit-on-error-type=Abort"
                    << " --optimize"
                    << " -use-cex-cache"
                    << " -solver-backend=metasmt "
                    << " -metasmt-backend=" + solvername + " "
                    << " -use-construct-hash-metasmt "
                    << " -libc=uclibc"
                    << " ./" + programHash + "-witness-result.bc"
                    << "  > ExecutionOutput.log";
      }

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
      command << "timeout " << (0.5 * this->timeout) << " ";
      command << "./" + programHash +
                     "-fuzzed.out -rss_limit_mb=0 -jobs=8"
              << " > fuzzer.output";

      // "-fuzzed.out -rss_limit_mb=4000 -jobs=8 -use_value_profile=1 "

      Map2Check::Log::Debug(command.str().c_str());
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

/**
* Apply source code transformation for concurrent 
* programs adopting Lazy CSeq
* https://www.southampton.ac.uk/~gp1y10/cseq/
**/
std::string Caller::applyCSeqTransformation(std::string preprocessed_code){
  Map2Check::Log::Info("Applying CSeq on " + preprocessed_code);

  std::string cseq_code_output = preprocessed_code + ".cseq.c";

  /**
   * Flow:
   * 1) Change to Cseq directory, because cseq modules limitation of the
   *    absolute path
   * 2) Run CSeq and print the code transformation 
   * */

  std::string currentPathTrack = boost::filesystem::current_path().string();
 
  std::string path_cseq = currentPath + Map2Check::cseqPath;
  
  boost::filesystem::current_path(path_cseq);

  Map2Check::Log::Debug("Changing to CSeq dir: " +
                        boost::filesystem::current_path().string());  

  std::ostringstream command;
  command.str("");
  command << "./" << Map2Check::cseqBinary << " --input "<< currentPathTrack << "/" << preprocessed_code
          << " --load lazy "
          << " --backend cpachecker > " << currentPathTrack << "/" << cseq_code_output;
  //<< cseq_code_output;      

  /**
   * For Cseq is necessary some code transformation, e.g., all variable 
   * not initialize on main function need to be initialized with nondet values
   * Checkout example: ./map2check --debug --target-function --smt-solver z3 ../samples/map2check-preprocessed.c.cseq.c
   * 
   * */    

  Map2Check::Log::Info(command.str());
  system(command.str().c_str());

  /**
   * Since CSeq dont include pthread.h header file
   * We need to replace the name of the pthread functions, e.g.,
   * pthread_create, once LibFuzzer fail in it's execution 
   * */

  // Returning the current Map2Check directory
  Map2Check::Log::Debug("Changing to Map2Check tmp files dir");
  boost::filesystem::current_path(currentPathTrack);

  return cseq_code_output;
  
}

/** This function should:
 * (1) Remove unsupported functions and clean the C code
 * (2) Generate .bc file from code
 * (3) Check for overflow errors on compilation
 */
void Caller::compileCFile(bool is_llvm_bc) {
  if (!is_llvm_bc) {
    Map2Check::Log::Debug("The input is a C source code");
    Map2Check::Log::Info("Preprocessing the code: " + this->pathprogram);

    // (1) Remove unsupported functions and clean the C code
    std::ostringstream commandRemoveExternMalloc;
    commandRemoveExternMalloc.str("");
    commandRemoveExternMalloc << "cat " << this->pathprogram << " | ";
    commandRemoveExternMalloc << "sed -e 's/extern void [*].[^_]*lloc.*/ / g' "
                              << " > " << programHash << "-preprocessed.c ";
    // Map2Check::Log::Info(commandRemoveExternMalloc.str().c_str());
    system(commandRemoveExternMalloc.str().c_str());

    std::ostringstream commandRemoveExternMemset;
    commandRemoveExternMemset.str("");
    commandRemoveExternMemset << "sed -i 's/extern void [*]memset.*/ / g' "
                              << " " << programHash << "-preprocessed.c ";
    // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
    system(commandRemoveExternMemset.str().c_str());

    std::ostringstream commandRemoveVoidMemset;
    commandRemoveVoidMemset.str("");
    commandRemoveVoidMemset << "sed -i 's/void [*]memset(void[*], int, size_t);/ / g' "
                              << " " << programHash << "-preprocessed.c ";
    // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
    system(commandRemoveVoidMemset.str().c_str());

    std::ostringstream commandRemoveVoidMemcpy;
    commandRemoveVoidMemcpy.str("");
    commandRemoveVoidMemcpy << "sed -i 's/void [*]memcpy(void[*], const void [*], size_t);/ / g' "
                              << " " << programHash << "-preprocessed.c ";
    // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
    system(commandRemoveVoidMemcpy.str().c_str());

    // TODO: if is a concurrent program then apply the code transformation
    std::string preprocessedFile = programHash + "-preprocessed.c";
    if(this->pthreadCheck){
      preprocessedFile = this->applyCSeqTransformation(preprocessedFile);
    }

    // (2) Generate .bc file from code
    // TODO(hbgit): -Winteger-overflow should be called only if is on overflow
    // mode
    std::string compiledFile = programHash + "-compiled.bc";
    std::ostringstream command;
    command.str("");
    // for KLEE dont forget https://github.com/klee/klee/issues/902
    command << Map2Check::clangBinary << " -I" << Map2Check::clangIncludeFolder
            << " -Wno-everything "
            << " -Winteger-overflow "
            << " -c -emit-llvm -g"
            << " " << Caller::preOptimizationFlags() 
            << " -Xclang -disable-O0-optnone -o " << compiledFile
            << " " << preprocessedFile
            << " > " << programHash << "-clang.out 2>&1";

    Map2Check::Log::Info("Compiling: " + preprocessedFile);
    Map2Check::Log::Info(command.str());

    system(command.str().c_str());
    this->pathprogram = compiledFile;

  } else {
    Map2Check::Log::Debug("The input is a LLVM bitcode");
    std::string compiledFile = programHash + "-compiled.bc";
    std::ostringstream command;
    command.str("");
    command << " cp " << this->pathprogram << " " << compiledFile;
    system(command.str().c_str());
    this->pathprogram = compiledFile;
  }

  // TODO(hbgit): (3) Check for overflow errors on compilation
}

void Caller::compileToCrabLlvm() {
  Map2Check::Log::Info("Compiling using crab-llvm in " + this->pathprogram);

  // (1) Remove unsupported functions and clean the C code
  // TODO(hbgit): improve regex to the next line
  std::ostringstream commandRemoveExternMalloc;
  commandRemoveExternMalloc.str("");
  commandRemoveExternMalloc << "cat " << this->pathprogram << " | ";
  commandRemoveExternMalloc << "sed -e 's/extern void [*].[^_]*lloc.*/ / g' "
                            << " > " << programHash << "-preprocessed.c ";
  system(commandRemoveExternMalloc.str().c_str());

  std::ostringstream commandRemoveExternMemset;
  commandRemoveExternMemset.str("");
  commandRemoveExternMemset << "sed -i 's/extern void [*]memset.*/ / g' "
                            << " " << programHash << "-preprocessed.c ";
  // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
  system(commandRemoveExternMemset.str().c_str());

  std::ostringstream commandRemoveVoidMemset;
  commandRemoveVoidMemset.str("");
  commandRemoveVoidMemset << "sed -i 's/void [*]memset(void[*], int, size_t);/ / g' "
                            << " " << programHash << "-preprocessed.c ";
  // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
  system(commandRemoveVoidMemset.str().c_str());

  std::ostringstream commandRemoveVoidMemcpy;
  commandRemoveVoidMemcpy.str("");
  commandRemoveVoidMemcpy << "sed -i 's/void [*]memcpy(void[*], const void [*], size_t);/ / g' "
                            << " " << programHash << "-preprocessed.c ";
  // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
  system(commandRemoveVoidMemcpy.str().c_str());


  // (2) Generate .bc file from code
  // TODO(hbgit): -Winteger-overflow should be called only if is on overflow
  // mode CLANG PATH
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

  std::ostringstream tmp_ld_p;
  tmp_ld_p << getenv("LD_LIBRARY_PATH");


  getPathLibCrabCommand << "LD_LIBRARY_PATH="
                        << tmp_ld_p.str().c_str()
                        << ":"
                        << getMapPath.str().c_str()
                        << "/bin/crabllvm/lib";

  // std::ostringstream tmp_ld;
  // tmp_ld << getenv("LD_LIBRARY_PATH");
  // std::cout << tmp_ld.str().c_str();

  std::string tmp_gplibcc = getPathLibCrabCommand.str().c_str();
  char* c_gplibcc = new char[tmp_gplibcc.length() + 1];
  std::copy(tmp_gplibcc.c_str(), tmp_gplibcc.c_str() + tmp_gplibcc.length() + 1,
            c_gplibcc);
  putenv(c_gplibcc);

  std::string compiledFile = programHash + "-compiled.bc";
  std::ostringstream command;
  command.str("");
  command << Map2Check::crabBinary << " -o " << compiledFile
          << " -m 64 -g --crab-disable-warnings "
             "--llvm-pp-loops --crab-promote-assume --crab-inter "
             "--crab-track=num --crab-add-invariants=block-entry "
          << " " << programHash << "-preprocessed.c ";

  //<< " -m 64 --crab-dom=oct --crab-track=num --crab-add-invariants=all "

  system(command.str().c_str());

  this->pathprogram = compiledFile;
}

}  // namespace Map2Check
