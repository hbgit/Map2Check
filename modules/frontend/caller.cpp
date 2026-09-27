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
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

#include "test_suite/ktest_reader.hpp"
#include "utils/gen_crypto_hash.hpp"
#include "utils/log.hpp"
#include "utils/slicer.hpp"
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

  // The scratch directory is named after the SHA-1 of the input bitcode, so it
  // is content-derived and not run-derived: analysing the same input twice
  // resolves to the same name. A plain mkdir over an existing directory fails
  // silently and leaves whatever a previous, possibly aborted, run left behind
  // -- including map2check_property. Now that a recorded violation survives a
  // budget expiry, a stale property file would be read as a real result and
  // fabricate a FALSE. Start from an empty directory.
  std::ostringstream createTempDir;
  createTempDir.str("");
  createTempDir << "rm -rf " << programHash << " && mkdir " << programHash;
  system(createTempDir.str().c_str());

  std::ostringstream moveProgram;
  moveProgram << "cp " << bc_program_path << " " << programHash;
  system(moveProgram.str().c_str());

  Map2Check::Log::Debug("Changing current dir");
  currentPath = std::filesystem::current_path().string();
  std::filesystem::current_path(currentPath + "/" + programHash);
  Map2Check::Log::Debug("Current path: " +
                        std::filesystem::current_path().string());
}

namespace {
/** Wall-clock start of the PROCESS, not of this Caller.
 *
 * The hybrid rebuilds the Caller once per engine, so a per-object start time
 * would reset the clock at every phase and defeat the whole point. Function
 * static: initialised on the first call, which happens before any engine runs.
 */
std::chrono::steady_clock::time_point processStart() {
  static const std::chrono::steady_clock::time_point start =
      std::chrono::steady_clock::now();
  return start;
}
}  // namespace

unsigned Caller::remainingSeconds() const {
  const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::steady_clock::now() - processStart())
                           .count();
  if (elapsed < 0) return this->timeout;
  const long long left = static_cast<long long>(this->timeout) - elapsed;
  // Never zero. A phase given no time at all is a phase that cannot even
  // report that it had none, and the caller has no way to tell that apart
  // from a crash.
  return left < 1 ? 1u : static_cast<unsigned>(left);
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

unsigned Caller::exportKleeVectorsAsSeeds() {
  std::error_code error;
  std::filesystem::create_directories(Caller::seedDirectory, error);

  std::vector<std::vector<std::string>> ignored;
  unsigned written = 0;
  unsigned index = 0;
  for (const auto &entry : std::filesystem::directory_iterator(
           Map2Check::kleeOutputDir, error)) {
    if (entry.path().extension() != ".ktest") continue;
    std::vector<Map2Check::KtestObject> objects =
        Map2Check::readKtestFile(entry.path().string());
    if (objects.empty()) continue;

    std::vector<uint8_t> bytes = Map2Check::ktestToFuzzerBytes(objects);
    if (bytes.empty()) continue;

    // Named by index rather than by content hash: AFL++ renames what it
    // keeps to its own hash anyway, so a second one here buys nothing.
    std::ostringstream name;
    name << Caller::seedDirectory << "/klee-" << index++;
    std::ofstream out(name.str(), std::ios::binary);
    if (!out.is_open()) continue;
    out.write(reinterpret_cast<const char *>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
    if (out.good()) ++written;
  }
  if (written > 0) {
    Map2Check::Log::Info("Seeded the fuzzer corpus with " +
                         std::to_string(written) + " vectors from KLEE");
  }
  return written;
}

std::string Caller::exportFuzzerVectorAsKtest() {
  std::vector<Map2Check::KtestObject> objects =
      Map2Check::readNonDetLogAsObjects(Map2Check::kleeLogCSV);
  if (objects.empty()) return "";

  std::error_code error;
  std::filesystem::create_directories(Caller::seedDirectory, error);
  const std::string path =
      std::string(Caller::seedDirectory) + "/from-fuzzer.ktest";
  if (!Map2Check::writeKtestFile(path, objects)) return "";
  Map2Check::Log::Info("Seeding KLEE with the fuzzer's vector (" +
                       std::to_string(objects.size()) + " inputs)");
  return path;
}

bool Caller::runSlicer(const std::string &input, const std::string &output,
                       std::vector<std::string> primary, bool addRuntimeNames,
                       const std::string &entry, const std::string &label) {
  const std::string slicer = Map2Check::slicerBinary();
  if (!std::filesystem::exists(slicer)) {
    // Announced, not silently skipped. A slicer that is asked for and absent
    // must not leave the run quietly analysing the whole program: that is how
    // --add-invariants stayed dead through a full baseline (issue #54).
    Map2Check::Log::Warning(
        "slicing was requested but sbt-slicer is not installed at " + slicer +
        " -- analysing the unsliced program");
    return false;
  }
  if (!std::filesystem::exists(input)) return false;

  // Bounded, for the reason every other external step here is bounded: the
  // slicer builds a system dependence graph over the whole module, and on the
  // large programs that is not fast. A slice that does not finish is not a
  // loss -- the caller falls back to the whole program; overrunning the budget
  // loses the verdict instead.
  const double sliceBudget = std::max(
      1.0,
      std::min(0.2 * this->timeout,
               std::max(1.0, static_cast<double>(remainingSeconds()) - 5.0)));

  // The program's own names join the criteria: the nondet functions (no fixed
  // list knows every name a benchmark declares, and a missing one silently
  // shifts the suite), and for the memory properties every map2check_* call
  // the instrumentation inserted. Read from the textual IR -- the bitcode
  // string table packs names with no separator. If the disassembly fails, the
  // fixed nondet list stands.
  const std::string inputIR = input + ".ll";
  std::ostringstream disassemble;
  disassemble << Map2Check::optBinary << " -S " << input << " -o " << inputIR
              << " > /dev/null 2>&1";
  std::vector<std::string> programNondets;
  if (system(disassemble.str().c_str()) == 0) {
    std::ifstream irFile(inputIR);
    std::stringstream irText;
    irText << irFile.rdbuf();
    programNondets = Map2Check::nondetNamesInIR(irText.str());
    if (addRuntimeNames) {
      // The runtime checks decide the property; external calls can commit
      // the error themselves (see externalNamesInIR). Both are criteria.
      for (const std::string &name :
           Map2Check::runtimeNamesInIR(irText.str())) {
        primary.push_back(name);
      }
      for (const std::string &name :
           Map2Check::externalNamesInIR(irText.str())) {
        if (std::find(primary.begin(), primary.end(), name) ==
            primary.end()) {
          primary.push_back(name);
        }
      }
    }
  }

  // -cutoff-diverging=false: the cutoff rewrites every path that cannot reach
  // the criterion into exit(0) with no debug location, and once KLEE links
  // uClibc the verifier rejects the module ("Broken module found") -- KLEE
  // never ran on a sliced task with a cut path (tacasv2a spec, defect 1).
  // --statistics: counts before and after, logged below.
  std::ostringstream command;
  command << "timeout -k " << Map2Check::killGracePeriod << " "
          << static_cast<unsigned>(sliceBudget) << " " << slicer << " -c "
          << Map2Check::slicingCriteria(primary, programNondets)
          << " --entry=" << entry
          << " -cutoff-diverging=false --statistics -o " << output << " "
          << input << " > slicer.output 2>&1";
  Map2Check::Log::Debug(command.str());
  const int result = system(command.str().c_str());

  std::error_code error;
  const bool produced = std::filesystem::exists(output, error) &&
                        std::filesystem::file_size(output, error) > 0;
  if (result != 0 || !produced) {
    Map2Check::Log::Warning(
        "sbt-slicer produced no usable output -- analysing the unsliced "
        "program");
    return false;
  }

  // Reported, because a slice is not a neutral speed-up: it narrows the
  // question being answered, and this line is the only visible sign of how
  // much was dropped.
  const auto before = std::filesystem::file_size(input, error);
  const auto after = std::filesystem::file_size(output, error);
  std::ifstream slicerLog("slicer.output");
  std::stringstream slicerText;
  slicerText << slicerLog.rdbuf();
  Map2Check::Log::Info(Map2Check::describeSlice(
      label, Map2Check::parseSlicerStatistics(slicerText.str()), before,
      after));
  return true;
}

bool Caller::sliceWithRespectToTarget(const std::string &targetFunction,
                                      const std::vector<std::string> &criteria) {
  // The COMPILED bitcode, not the instrumented one: this runs before callPass
  // so that the instrumentation is applied to the slice rather than removed by
  // it. Entry is still plain main at this point, for the same reason.
  const std::string input = programHash + "-compiled.bc";
  const std::string output = programHash + "-sliced.bc";
  std::string label;
  for (const std::string &name : criteria) {
    label += (label.empty() ? "" : ",") + name;
  }
  if (!runSlicer(input, output, criteria, false, "main", label)) return false;
  std::error_code error;

  // sbt-slicer removes the body of the criterion function itself. reach_error
  // is where the slice ENDS -- nothing it does can influence whether it is
  // reached -- so the slicer keeps the call site and drops the definition.
  //
  // KLEE tolerates the resulting declaration. The native AFL++ link does
  // not: it fails with "undefined reference to reach_error", no *-fuzzed.out
  // is produced, and the fuzzer stage then does nothing at all.
  //
  // A WEAK definition restores the link without displacing a real one: where
  // the slice did keep the body, the strong definition still wins.
  const std::string stubSource = programHash + "-target-stub.c";
  const std::string stubBitcode = programHash + "-target-stub.bc";
  const std::string linked = programHash + "-sliced-linked.bc";
  {
    std::ofstream stub(stubSource);
    if (stub.is_open()) {
      stub << Map2Check::targetStubSource(targetFunction);
    }
  }
  std::ostringstream compileStub;
  compileStub << Map2Check::clangBinary << " -Wno-everything -c -emit-llvm -g"
              << " " << Caller::preOptimizationFlags() << " -o " << stubBitcode
              << " " << stubSource << " >> slicer.output 2>&1";
  std::ostringstream linkStub;
  linkStub << Map2Check::llvmLinkBinary << " " << output << " " << stubBitcode
           << " -o " << linked << " >> slicer.output 2>&1";
  if (system(compileStub.str().c_str()) == 0 &&
      system(linkStub.str().c_str()) == 0 &&
      std::filesystem::exists(linked, error) &&
      std::filesystem::file_size(linked, error) > 0) {
    std::filesystem::rename(linked, output, error);
  } else {
    // Not fatal: without the stub the fuzzer stage is lost, but KLEE still
    // runs on the slice. Say so rather than returning a half-configured run.
    Map2Check::Log::Warning(
        "could not restore a definition of " + targetFunction +
        " after slicing -- the AFL++ stage will not link");
  }

  std::filesystem::rename(output, input, error);
  return !error;
}

bool Caller::sliceInstrumented() {
  // Memory properties have no criterion in the user's program: the property
  // is decided by the runtime calls MemoryTrackPass inserted, so the slice is
  // taken after instrumentation with every map2check_* call as a criterion
  // (none can be dropped), and __map2check_main__ -- the renamed user main --
  // as the entry. tacasv2b spec, section 2.
  const std::string input = programHash + "-output.bc";
  const std::string output = programHash + "-sliced-instrumented.bc";
  if (!std::filesystem::exists(input)) return false;
  if (!runSlicer(input, output, {}, true, "__map2check_main__",
                 "map2check runtime")) {
    return false;
  }
  std::error_code error;
  std::filesystem::rename(output, input, error);
  return !error;
}

void Caller::cleanGarbage() {
  std::filesystem::current_path(currentPath);
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
    case (NonDetGenerator::AFLPlusPlus): {
      Map2Check::Log::Info("Instrumenting with AFL++");
      std::ostringstream command;
      command.str("");

      // Bounded, because it was not, and that is where the budget went.
      //
      // Both invocations run clang at -O2 over the whole instrumented module.
      // On the large ECA and Recursive programs that takes longer than the
      // entire budget, and nothing was stopping it: the run was still linking
      // its fuzzer binary when the harness's outer timeout killed it, so it
      // produced no verdict and scored ERROR. Measured on the v12 corpus:
      // 20 such tasks, every one at 87 to 89 seconds against a 60 second
      // budget, all of them dying at this exact step.
      //
      // Losing the fuzzer binary is a real cost, but a bounded one: KLEE still
      // gets its phase and the run still reaches a verdict. Spending the whole
      // budget here costs the verdict itself.
      const double compileBudget = std::max(
          1.0, std::min(0.25 * this->timeout,
                        std::max(1.0, static_cast<double>(remainingSeconds()) -
                                          5.0)));
      const std::string bound = "timeout -k " +
                                std::to_string(Map2Check::killGracePeriod) +
                                " " + std::to_string(static_cast<unsigned>(compileBudget)) + " ";

      command
          << bound << Map2Check::aflClangFastBinary()
          << "  -g " << Caller::postOptimizationFlags()
          << " -o " + programHash + "-fuzzed.out"
          << " " + programHash + "-result.bc";

      system(command.str().c_str());

      std::ostringstream commandWitness;
      commandWitness.str("");
      commandWitness << bound << Map2Check::aflClangFastBinary()
                     << "  -g "
                     << " -o " + programHash + "-witness-fuzzed.out"
                     << " " + programHash + "-witness-result.bc";

      system(commandWitness.str().c_str());

      // The CmpLog companion binary: the same program instrumented to log
      // the operands of comparisons, which afl-fuzz (-c) uses to solve
      // magic-value guards such as `x == 123456` by input-to-state
      // substitution. It is AFL++'s counterpart of the value profile the
      // previous fuzzer ran with (-use_value_profile=1); without it the
      // tacasv1 comparison would pit an unarmed AFL++ against an armed
      // LibFuzzer. Optional: if it does not build, the fuzzer runs without.
      std::ostringstream commandCmplog;
      commandCmplog << "AFL_LLVM_CMPLOG=1 " << bound
                    << Map2Check::aflClangFastBinary() << "  -g "
                    << Caller::postOptimizationFlags()
                    << " -o " + programHash + "-cmplog.out"
                    << " " + programHash + "-result.bc";
      system(commandCmplog.str().c_str());

      // Announced rather than discovered later as a silent no-op -- the same
      // failure mode the sliced arm spent a whole campaign in.
      std::error_code fuzzErr;
      if (!std::filesystem::exists(programHash + "-fuzzed.out", fuzzErr)) {
        Map2Check::Log::Warning(
            "the AFL++ binary did not build within " +
            std::to_string(static_cast<int>(compileBudget)) +
            "s -- skipping the fuzzer phase and leaving the budget to KLEE");
      } else if (!std::filesystem::exists(programHash + "-cmplog.out",
                                          fuzzErr)) {
        Map2Check::Log::Warning(
            "the AFL++ CmpLog binary did not build -- fuzzing without "
            "comparison solving");
      }
      break;
    }
  }
}

int Caller::callPass(std::string target_function, bool sv_comp) {
  std::ostringstream transformCommand;
  transformCommand.str("");
  transformCommand << Map2Check::optBinary;

  // --- New Pass Manager: use -load-pass-plugin + -passes= ---
  std::string nonDetPlugin = "${MAP2CHECK_PATH}/lib/libNonDetPass";

  Map2Check::Log::Info("Adding nondet pass");
  transformCommand << " -tailcallopt";
  transformCommand << " -load-pass-plugin=" << nonDetPlugin << getLibSuffix();

  // Build the passes pipeline
  std::ostringstream passesArg;
  passesArg << "nondet-pass";

  bool loadsMemoryTrackPass = false;
  switch (map2checkMode) {
    case Map2CheckMode::MEMTRACK_MODE: {
      Map2Check::Log::Info("Adding memtrack pass");
      std::string memPlugin = "${MAP2CHECK_PATH}/lib/libMemoryTrackPass";
      transformCommand << " -load-pass-plugin=" << memPlugin << getLibSuffix();
      passesArg << ",memory-track";
      loadsMemoryTrackPass = true;
      break;
    }
    case Map2CheckMode::MEMCLEANUP_MODE: {
      Map2Check::Log::Info("Adding memcleanup pass");
      std::string memPlugin = "${MAP2CHECK_PATH}/lib/libMemoryTrackPass";
      transformCommand << " -load-pass-plugin=" << memPlugin << getLibSuffix();
      passesArg << ",memory-track";
      loadsMemoryTrackPass = true;
      break;
    }
    case Map2CheckMode::OVERFLOW_MODE: {
      std::string overflowPlugin = "${MAP2CHECK_PATH}/lib/libOverflowPass";
      transformCommand << " -load-pass-plugin=" << overflowPlugin
                       << getLibSuffix();
      passesArg << ",overflow-pass";
      break;
    }
    case Map2CheckMode::COVER_BRANCHES_MODE: {
      // Nothing beyond nondet-pass. The inputs have to be recorded, so that
      // pass stays; there is no property to instrument for, so nothing else
      // does. This is also the leanest pipeline the tool has, which is the
      // point: fewer instrumented calls means KLEE explores more paths in the
      // same budget, and paths are what a branch suite is made of.
      Map2Check::Log::Info("Running cover-branches mode (no property check)");
      break;
    }
    case Map2CheckMode::REACHABILITY_MODE: {
      Map2Check::Log::Info("Running reachability mode");
      Map2Check::Log::Debug("Target function: " + target_function);
      std::string targetPlugin = "${MAP2CHECK_PATH}/lib/libTargetPass";
      transformCommand << " -load-pass-plugin=" << targetPlugin
                       << getLibSuffix();
      // Pass target function name via cl::opt flag to opt
      transformCommand << " -function-name=" << target_function;
      passesArg << ",target-pass";
      break;
    }
    case Map2CheckMode::ASSERT_MODE: {
      Map2Check::Log::Info("Running assert mode");
      std::string assertPlugin = "${MAP2CHECK_PATH}/lib/libAssertPass";
      transformCommand << " -load-pass-plugin=" << assertPlugin
                       << getLibSuffix();
      passesArg << ",assert-pass";
      break;
    }
    default: { break; }
  }

  Map2Check::Log::Info("Adding map2check pass");
  std::string map2checkPlugin = "${MAP2CHECK_PATH}/lib/libMap2CheckLibrary";
  transformCommand << " -load-pass-plugin=" << map2checkPlugin
                   << getLibSuffix();
  passesArg << ",map2check-library";

  transformCommand << " -entry-function=" << this->entryFunction;
  if (loadsMemoryTrackPass) {
    transformCommand << " -m2c-entry-function=" << this->entryFunction;
  }
  if (this->wasmMode) {
    transformCommand << " -wasm-mode";
  }

  transformCommand << " -passes='" << passesArg.str() << "'";

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
    case Map2CheckMode::COVER_BRANCHES_MODE: {
      // No property, so no analysis -- the run exists to explore and record.
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
    case (NonDetGenerator::AFLPlusPlus): {
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorAFL.bc";
      break;
    }
  }

  if (this->wasmMode) {
    linkCommand << " ${MAP2CHECK_PATH}/lib/WasmRuntimeStubs.bc";
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

std::string Caller::generateWasmWrapperStatic(const std::string& wasmOutHeaderPath,
                                               const std::string& entryPointName) {
  // Parse entry point name to extract module prefix
  // e.g., w2c_0x24test__array0x2Ewasm_0x5Fstart → module is 0x24test__array0x2Ewasm
  std::string moduleName;
  std::string typeName = entryPointName;
  if (entryPointName.size() > 4 && entryPointName.substr(0, 4) == "w2c_") {
    size_t pos = entryPointName.rfind("_0x5Fstart");
    if (pos != std::string::npos) {
      moduleName = entryPointName.substr(4, pos - 4);
      typeName = "w2c_" + moduleName;
    }
  }

  // Use a temp file for the wrapper C source
  char tmpPath[] = "/tmp/m2c_wasm_wrapper_XXXXXX";
  int fd = mkstemp(tmpPath);
  if (fd < 0) return "";
  close(fd);
  std::string wrapperPath = std::string(tmpPath) + ".c";
  std::string wrapperBcPath = std::string(tmpPath) + ".bc";

  std::ostringstream wrapper;
  wrapper << "#include \"" << wasmOutHeaderPath << "\"\n"
          << "#include <stdlib.h>\n"
          << "int main() {\n"
          << "    " << typeName << " instance;\n"
          << "    wasm2c_" << moduleName << "_instantiate(&instance, NULL);\n"
          << "    " << entryPointName << "(&instance);\n"
          << "    wasm2c_" << moduleName << "_free(&instance);\n"
          << "    return 0;\n"
          << "}\n";

  std::ofstream outFile(wrapperPath);
  outFile << wrapper.str();
  outFile.close();

  std::string headerDir = wasmOutHeaderPath.substr(0, wasmOutHeaderPath.find_last_of("/"));
  std::string wasmIncludePath;
  struct stat st;
  if (stat("/opt/wabt-1.0.41/include", &st) == 0) {
    wasmIncludePath = "/opt/wabt-1.0.41/include";
  } else if (stat("/opt/wabt/include", &st) == 0) {
    wasmIncludePath = "/opt/wabt/include";
  } else {
    wasmIncludePath = "/usr/include";
  }
  std::ostringstream compileCmd;
  compileCmd << Map2Check::clangBinary << " -c -emit-llvm"
             << " -I" << wasmIncludePath
             << " -I" << headerDir
             << " " << wrapperPath << " -o " << wrapperBcPath;
  system(compileCmd.str().c_str());

  return wrapperBcPath;
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
      // -k: KLEE installs a SIGTERM handler that tries to shut down gracefully,
      // but that handler never runs while the solver is wedged. Plain `timeout`
      // then waits forever for a child that will not die, and map2check hangs
      // past its own budget. The grace period escalates to SIGKILL so the
      // budget is actually enforced.
      // Reserve a few seconds for what happens AFTER the engine: reading the
      // property file, recovering the input vector, writing the suite. KLEE
      // holding the budget to its last second is what turned a decided run
      // into an ERROR.
      constexpr double kPostEngineReserve = 5.0;
      const double kleeBudget = std::max(
          1.0, std::min(0.8 * this->timeout,
                        this->remainingSeconds() - kPostEngineReserve));
      kleeCommand << "timeout -k " << Map2Check::killGracePeriod << " "
                  << static_cast<unsigned>(kleeBudget) << " ";
      kleeCommand << Map2Check::kleeBinary;

      // KLEE's own deadline, set BELOW the external one so it is KLEE that
      // stops, not the kill.
      //
      // Without it every path explored up to the budget is thrown away.
      // Measured: a program with twelve nondeterministic reads explored 3982
      // paths and produced ZERO .ktest files, because `timeout` killed KLEE
      // before it wrote any -- and those files are where a Cover-Branches
      // suite comes from, and where a Cover-Error suite now recovers its
      // input vector. Reaching the budget is the NORMAL case in a competition
      // run, so this was not an edge: it was the common path discarding all
      // of its work.
      //
      // The external timeout above stays as the backstop for the case its
      // comment describes, a solver wedged so deep that KLEE's own deadline
      // never gets a turn.
      // INTEGER seconds. KLEE parses --max-time with a duration parser that
      // rejects a fractional value outright:
      //
      //     KLEE: ERROR: Illegal number format: 36.75s
      //
      // and exits 256 without running a single instruction. The old expression
      // 0.7*timeout happened to be whole for every budget in use, so this
      // never showed; 0.875*kleeBudget is whole only when kleeBudget is a
      // multiple of 8, and kleeBudget now derives from the time REMAINING,
      // which is whatever the clock says.
      //
      // The cost of getting this wrong is total and silent: no KLEE phase at
      // all, so nothing can be proved safe. It collapsed 464 Juliet TN
      // verdicts to 1.
      kleeCommand << " --max-time="
                  << std::max(1u, static_cast<unsigned>(0.875 * kleeBudget))
                  << "s";


      // Halting on the first error is right when there is a property to
      // decide -- the answer is known, and more exploration is waste. It is
      // wrong for Cover-Branches, where there is no property and the paths ARE
      // the product: stopping at the first error throws away every path not
      // yet explored, and with it the test cases they would have produced.
      //
      // Measured before this: the same twelve-branch program produced 1020
      // .ktest files on one run and zero on the next, the difference being
      // whether an error happened to be hit early. A suite that depends on
      // that is not a suite.
      // A starting point from whatever ran before, when there is one. KLEE
      // replays the seed and then explores around it, instead of rediscovering
      // from nothing a path the fuzzer already walked -- which under a fixed
      // budget is not merely faster, it is depth the run would not otherwise
      // have reached.
      std::string seedFlag;
      if (this->seedExchange) {
        const std::string seed = exportFuzzerVectorAsKtest();
        if (!seed.empty()) seedFlag = " --seed-file=" + seed;
      }

      // Depth-first for Cover-Branches, and the reason is about what survives
      // the deadline rather than about search quality.
      //
      // KLEE's default search keeps thousands of states alive at once. Each
      // writes its .ktest only when it terminates, and the ones still live
      // when the budget expires are dumped at halt -- where writing them
      // fails wholesale: "unable to write output test case, losing it", 3982
      // times in one measured run, for a total of zero test cases from 3982
      // explored paths. Depth-first finishes states one after another, so
      // each one's test is on disk long before the deadline matters.
      //
      // For the property modes the default search stays: there the goal is to
      // find one violating path quickly, not to harvest many.
      std::string searchPolicy =
          (map2checkMode == Map2CheckMode::COVER_BRANCHES_MODE)
              ? " --search=dfs"
              : "";

      std::string stopPolicy =
          (map2checkMode == Map2CheckMode::COVER_BRANCHES_MODE)
              ? ""
              : " --exit-on-error-type=Abort";

      std::vector<std::string> kleebackendsolver = {"z3", "stp"};
      std::vector<std::string> kleemetasolver = {"btor", "yices2"};


      if ( std::count(kleebackendsolver.begin(), kleebackendsolver.end(), solvername) ) {
        // Checkout solver adopted, if is z3 or stp
        // in KLEE add -solver-backend option

        Map2Check::Log::Info("Solver backend caller: " + solvername);
        //  --allow-external-sym-calls
        //  -use-cache
        kleeCommand << " --external-calls=all"
                    << stopPolicy << searchPolicy << seedFlag
                    << " --optimize"
                    << " --use-cex-cache"
                    << " --solver-backend=" + solvername + " "
                    << " --libc=uclibc"
                    << " ./" + programHash + "-witness-result.bc"
                    << "  > ExecutionOutput.log";
      } else if ( std::count(kleemetasolver.begin(), kleemetasolver.end(), solvername) ) {
        // Checkout solver adopted, if is btor (Boolector) or yices (Yices)
        // in KLEE add - option
        Map2Check::Log::Info("Solver metaSMT caller: " + solvername);

        kleeCommand << " --external-calls=all"
                    << stopPolicy << searchPolicy << seedFlag
                    << " --optimize"
                    << " --use-cex-cache"
                    << " --solver-backend=metasmt "
                    << " --metasmt-backend=" + solvername + " "
                    << " --libc=uclibc"
                    << " ./" + programHash + "-witness-result.bc"
                    << "  > ExecutionOutput.log";
      }

      Map2Check::Log::Debug(kleeCommand.str());
      int result = system(kleeCommand.str().c_str());
      if (this->seedExchange) {
        // Written after KLEE rather than before the next phase, because the
        // .ktest files are in the scratch directory that cleanGarbage() will
        // remove -- and because a later alternation, or a resumed run, should
        // find them already there.
        exportKleeVectorsAsSeeds();
      }
      Map2Check::Log::Warning("Exited klee with " + std::to_string(result));
      if (result == 31744)  // Timeout
        gotTimeout = true;
      // KLEE stopping on its own --max-time exits 0, like a run that explored
      // every path, but it proves nothing. Treated as the timeout it is: a
      // violation already recorded is kept, anything else is UNKNOWN -- never
      // the TRUE a short path's NONE in the property file would otherwise make
      // it (a reachable null dereference came back TRUE).
      if (Map2Check::kleeHaltedOnTimer(Map2Check::kleeOutputDir)) {
        Map2Check::Log::Warning(
            "KLEE halted on its timer with states left -- not a complete "
            "exploration");
        gotTimeout = true;
      }

      break;
    }
    case (NonDetGenerator::AFLPlusPlus): {
      std::error_code fuzzErr;
      const bool hasFuzzer =
          std::filesystem::exists(programHash + "-fuzzed.out", fuzzErr);
      if (fuzzErr) {
        Map2Check::Log::Warning(
            "could not check whether the AFL++ binary is available: " +
            fuzzErr.message());
        break;
      }
      if (!hasFuzzer) {
        Map2Check::Log::Warning(
            "the AFL++ binary is unavailable -- skipping the fuzzer phase");
        break;
      }
      Map2Check::Log::Info("Executing AFL++ with map2check");
      std::ostringstream command;
      command.str("");
      // Against what is LEFT, not against the nominal budget -- see
      // Caller::remainingSeconds.
      const double fuzzerBudget =
          std::min(0.2 * this->timeout,
                   static_cast<double>(this->remainingSeconds()));
      // afl-fuzz needs a non-empty -i dir and a -o dir that does not already
      // exist (the hybrid may run the fuzzer phase twice).
      //
      // The input dir is the shared seed corpus only under --seed-exchange,
      // as it was for the previous fuzzer; otherwise a private one, so a run
      // without the exchange leaves no seeds/ behind. Either way it gets one
      // placeholder input when empty, because afl-fuzz refuses to start
      // without one (the previous fuzzer could start from nothing).
      std::error_code seedErr;
      const std::string inputDir =
          this->seedExchange ? std::string(Caller::seedDirectory) : "afl-in";
      std::filesystem::create_directories(inputDir, seedErr);
      if (std::filesystem::is_empty(inputDir, seedErr)) {
        std::ofstream seed(inputDir + "/seed");
        seed << "A";
        if (!seed.good())
          Map2Check::Log::Warning("could not write the AFL++ placeholder seed");
      }
      if (seedErr)
        Map2Check::Log::Warning("could not prepare the AFL++ input dir " +
                                inputDir + ": " + seedErr.message());
      std::filesystem::remove_all("afl-out", seedErr);
      // The AFL_* settings go on the command line, not only into the dev
      // image's ENV, so that a release install or a benchmark host outside
      // the image does not trip afl-fuzz's UI, CPU-affinity, cpufreq and
      // core_pattern checks and exit before fuzzing anything.
      //   AFL_CRASHING_SEEDS_AS_NEW_CRASH: a seed that already reaches the
      //     violation is recorded as a crash instead of being skipped -- the
      //     previous fuzzer reported that case too.
      //   AFL_BENCH_UNTIL_CRASH: stop at the first crash, as the previous
      //     fuzzer did, and hand the rest of the budget back.
      command << "AFL_NO_UI=1 AFL_NO_AFFINITY=1 AFL_SKIP_CPUFREQ=1"
              << " AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1"
              << " AFL_CRASHING_SEEDS_AS_NEW_CRASH=1"
              << " AFL_BENCH_UNTIL_CRASH=1 ";
      // Bounded by `timeout` alone, as the previous fuzzer was. Not also by
      // afl-fuzz -V: that one compares wall-clock (gettimeofday) readings,
      // and a clock stepped backwards -- measured at over a second under
      // WSL2 -- underflows the difference and ends the run after a few
      // hundred executions. `timeout` uses a relative timer. At least 1s,
      // since `timeout 0` would mean no limit at all.
      command << "timeout -k " << Map2Check::killGracePeriod << " "
              << std::max(1u, static_cast<unsigned>(fuzzerBudget)) << " ";
      std::error_code cmplogErr;
      const bool hasCmplog =
          std::filesystem::exists(programHash + "-cmplog.out", cmplogErr);
      command << Map2Check::aflFuzzBinary()
              << " -i " << inputDir
              << " -o afl-out";
      if (hasCmplog) command << " -c ./" << programHash << "-cmplog.out";
      command << " -- ./" << programHash << "-fuzzed.out"
              << " > fuzzer.output 2>&1";

      int result = system(command.str().c_str());
      Map2Check::Log::Warning("Exited fuzzer with " + std::to_string(result));
      if (result == 31744)  // Timeout
        gotTimeout = true;

      // A single instance without -M/-S is named "default" by afl-fuzz, and
      // its findings live under afl-out/default/, not afl-out/.
      const std::string aflFindings = "afl-out/default";

      // Replay crashes with the witness binary to confirm a real violation.
      // Standalone, the persistent binary reads its input from stdin (see
      // NonDetGeneratorAFL.c), so the crash file is redirected in; the names
      // AFL++ gives them contain ':' and ',', hence the quoting. Stop at the
      // first confirmed one: every replay rewrites the recorded property, and
      // a later one that does not reproduce would overwrite the violation.
      // Each replay is capped: a crash that does not reproduce from a fresh
      // process may loop instead, and must not eat what is left for KLEE.
      // Files are selected by what they are, not by AFL++'s "id:" naming,
      // which AFL_SHA1_FILENAMES or a SIMPLE_FILES build would change.
      const unsigned replayBudget = std::min(
          this->remainingSeconds(),
          std::max(5u, static_cast<unsigned>(0.1 * this->timeout)));
      std::error_code crashErr;
      for (const auto &entry : std::filesystem::directory_iterator(
               aflFindings + "/crashes", crashErr)) {
        if (!entry.is_regular_file(crashErr)) continue;
        if (entry.path().filename() == "README.txt") continue;
        std::ostringstream commandWitness;
        commandWitness << "timeout -k " << Map2Check::killGracePeriod << " "
                       << replayBudget << " ./" << programHash
                       << "-witness-fuzzed.out < '" << entry.path().string()
                       << "'";
        system(commandWitness.str().c_str());
        if (isWitnessFileCreated()) break;
      }

      // afl-fuzz never writes back into -i, so under --seed-exchange its
      // discoveries are copied into seeds/ -- the previous fuzzer grew that
      // directory in place. Inputs tagged ",orig:" are the seeds it started
      // from, already there.
      //
      // Caveat, inherited unchanged from v15: seeds/ lives in the scratch
      // directory, which the next phase's Caller wipes on construction, so
      // this corpus does not yet reach the following KLEE or fuzzer phase.
      // Making it survive changes what the hybrid measures, and belongs to
      // the smart-seeds work, not to the engine swap.
      if (this->seedExchange) {
        std::error_code queueErr;
        for (const auto &entry : std::filesystem::directory_iterator(
                 aflFindings + "/queue", queueErr)) {
          const std::string name = entry.path().filename().string();
          if (!entry.is_regular_file(queueErr)) continue;
          if (name.find(",orig:") != std::string::npos) continue;
          std::filesystem::copy_file(
              entry.path(),
              std::string(Caller::seedDirectory) + "/afl-" + name,
              std::filesystem::copy_options::skip_existing, queueErr);
        }
      }
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
  if (!is_llvm_bc) {
    Map2Check::Log::Info("Compiling " + this->pathprogram);

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

    // (2) Generate .bc file from code
    // TODO(hbgit): -Winteger-overflow should be called only if is on overflow
    // mode
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
  } else {
    std::string compiledFile = programHash + "-compiled.bc";
    std::ostringstream command;
    command.str("");
    command << " cp " << this->pathprogram << " " << compiledFile;
    system(command.str().c_str());
    this->pathprogram = compiledFile;
  }

  // TODO(hbgit): (3) Check for overflow errors on compilation
}

void Caller::compileWithClam() {
  Map2Check::Log::Info("Compiling with Clam (invariants) in " + this->pathprogram);

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

  // Clam's shared libraries live under its own installation root, not under
  // the Map2Check prefix: it is an independent tool with its own LLVM-versioned
  // build. The old path pointed at ${MAP2CHECK_PATH}/bin/crabllvm/lib, which
  // has not existed since the LLVM 6 era.
  const char* clamDirEnv = getenv("CLAM_DIR");
  std::string clamRoot =
      clamDirEnv != nullptr ? std::string(clamDirEnv)
                            : std::string(Map2Check::clamDefaultRoot);

  const char* ldPathEnv = getenv("LD_LIBRARY_PATH");
  std::ostringstream getPathLibClamCommand;
  getPathLibClamCommand << "LD_LIBRARY_PATH="
                        << (ldPathEnv != nullptr ? ldPathEnv : "") << ":"
                        << clamRoot << "/lib";

  std::string tmp_gplibcc = getPathLibClamCommand.str();
  char* c_gplibcc = new char[tmp_gplibcc.length() + 1];
  std::copy(tmp_gplibcc.c_str(), tmp_gplibcc.c_str() + tmp_gplibcc.length() + 1,
            c_gplibcc);
  putenv(c_gplibcc);

  std::string compiledFile = programHash + "-compiled.bc";
  std::ostringstream command;
  command.str("");

  // This flag list is measured, not translated. Clam renamed one option
  //   crab-llvm: --crab-add-invariants=block-entry
  //   Clam:      --crab-opt=add-invariants --crab-opt-invariants-loc=block-entry
  // but two others in the legacy call actively defeat the purpose, and were
  // dropped after counting verifier.assume calls in the emitted bitcode on a
  // loop program (5 injected by the baseline flags below):
  //
  //   --llvm-pp-loops        5 -> 0. Its loop preprocessing runs after the
  //                          invariants would be placed, and nothing survives.
  //   --crab-promote-assume  5 -> 1. It rewrites verifier.assume into the
  //                          llvm.assume intrinsic (lib/Transforms/PromoteAssume.cc),
  //                          which is precisely the symbol NonDetPass.cpp:95
  //                          matches on -- promoting makes the invariants
  //                          invisible to Map2Check.
  //
  // So the legacy invocation would have injected nothing even if crab-llvm had
  // still built: the capability was dead twice over. --crab-inter and -m 64 -g
  // were verified harmless (still 5). --crab-disable-warnings and
  // --disable-lower-gv no longer exist in Clam's option set.
  //
  // The emitted verifier.assume is rewritten by NonDetPass into
  // map2check_crab_assume, which the runtime forwards to klee_assume -- so
  // nothing downstream needs to change.
  // See docs/reports/2026-08-16-crabllvm-review.md.
  command << Map2Check::clamBinary() << " -o " << compiledFile << " -m 64 -g"
          << " --crab-inter"
          << " --crab-track=num"
          << " --crab-opt=add-invariants"
          << " --crab-opt-invariants-loc=block-entry"
          << " " << programHash << "-preprocessed.c ";

  Map2Check::Log::Debug(command.str());
  system(command.str().c_str());

  this->pathprogram = compiledFile;
}

}  // namespace Map2Check
