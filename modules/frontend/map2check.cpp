/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * Map2Check -> GPL-2.0
 * CLANG     -> Apache-2.0
 * KLEE      -> NCSA
 * CRAB-LLVM -> Apache-2.0
 * STP,Z3    -> MIT
 * BOOST     -> BSL-1.0
 *
 * SPDX-License-Identifier: (GPL-2.0 AND Apache-2.0 AND NCSA AND MIT AND BSL-1.0)
 *
 **/

#include "map2check.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "caller.hpp"
#include "counter_example/counter_example.hpp"
#include "test_suite/ktest_reader.hpp"
#include "test_suite/test_suite.hpp"
#include "utils/gen_crypto_hash.hpp"
#include "utils/log.hpp"
#include "utils/sha256.hpp"
#include "witness/witness_include.hpp"
#include "wasm_lifter.hpp"

namespace po = boost::program_options;
namespace fs = std::filesystem;
#include "map2check_version.hpp"

// TODO(hbgit): should get preprocessor flags from CMake

namespace {

const size_t SUCCESS = 0;
const size_t ERROR_IN_COMMAND_LINE = 1;
/** A syntactically valid option asked for a capability this build does not
 * have. Distinct from ERROR_IN_COMMAND_LINE so callers can tell "you typed it
 * wrong" from "this binary cannot do that", and act differently. */
const size_t ERROR_UNAVAILABLE_CAPABILITY = 3;
/** --expected-result recorded a verdict different from the one asked for.
 *
 * This used to be signalled with abort(), i.e. SIGABRT (exit 134). A signal
 * tells BenchExec "the tool crashed", which is the opposite of the truth: the
 * tool ran to a definitive verdict and that verdict disagreed with the
 * harness's expectation. A distinct exit code lets the harness distinguish
 * "wrong answer" from "broken run". Kept at 4 so it never collides with the
 * command-line (1) and capability (3) codes, and never looks like success. */
const size_t ERROR_EXPECTED_RESULT = 4;
// A helper function to simplify the main part.
template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
  return os;
}

/** Whether an invariant generator is installed alongside this build.
 *
 * Clam (formerly crab-llvm) is an external tool invoked as a subprocess, so
 * this is a filesystem question rather than a compile-time one: a binary built
 * with -DENABLE_CLAM=ON still has to find the installed driver at run time.
 * Both conditions are checked, because neither implies the other. */
bool invariantGeneratorAvailable() {
#ifndef MAP2CHECK_ENABLE_CLAM
  return false;
#else
  // Same resolver the invocation uses, so the check and the command can never
  // disagree about where Clam is.
  return fs::exists(Map2Check::clamBinary());
#endif
}

/** The <specification> field of the test-suite metadata.
 *
 * BenchExec always hands the tool a property file, and copying it verbatim is
 * what keeps the metadata correct when the competition revises a property
 * string -- guessing it here would silently drift. The fallbacks exist only so
 * a manual run without --property-file still produces a valid suite.
 *
 * `invocationDir` is not optional decoration. This runs AFTER the pipeline has
 * chdir'd into the scratch directory, so a relative --property-file -- which is
 * what BenchExec and every harness here pass -- resolved against the wrong
 * directory and silently fell through to the guess below. Measured on the
 * Test-Comp corpus: "could not read property file: prop.prp" on every task.
 *
 * It went unnoticed because the guess HAPPENS to match the real property text
 * for both categories today, so no output differed and no test could see it.
 * The drift the comment above warns about was therefore already in place. */
std::string resolveSpecification(const std::string &propertyFile,
                                 const std::string &invocationDir,
                                 Map2Check::Map2CheckMode mode) {
  if (!propertyFile.empty()) {
    std::string path = propertyFile;
    if (!fs::path(path).is_absolute() && !invocationDir.empty()) {
      path = invocationDir + "/" + path;
    }
    std::ifstream in(path);
    if (in.is_open()) {
      std::ostringstream buffer;
      buffer << in.rdbuf();
      std::string text = buffer.str();
      // Property files end with a newline that does not belong in an element.
      while (!text.empty() && (text.back() == '\n' || text.back() == '\r')) {
        text.pop_back();
      }
      if (!text.empty()) return text;
    }
    Map2Check::Log::Warning("could not read property file: " + path);
  }
  if (mode == Map2Check::Map2CheckMode::REACHABILITY_MODE) {
    return "COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )";
  }
  return "COVER( init(main()), FQL(COVER EDGES(@DECISIONEDGE)) )";
}

/** Serializes the violating execution's nondet log as a Test-Comp test suite.
 *
 * Must run before Caller::cleanGarbage(): klee_log.csv lives in the scratch
 * directory that cleanGarbage() deletes, and the suite must not. */
/** Upper bound on a Cover-Branches suite.
 *
 * KLEE can terminate tens of thousands of paths, and every test case costs the
 * validator a compile-and-run. The competition scores coverage, not volume, so
 * past a few dozen vectors the marginal branch is rare and the validation cost
 * is not. The bound is here rather than in the runtime because this is the
 * side that knows what the suite is for.
 *
 * Lowered from 500 after measuring what 500 does: of 116 tasks whose
 * validation failed outright on the Test-Comp corpus, 115 had at least 100
 * test cases and the median was exactly 500 -- suites so large that TestCov
 * could not finish validating them, so the extra vectors scored nothing and
 * cost everything. Generating them was work spent to make the result
 * unmeasurable. */
constexpr size_t kMaxBranchTestCases = 50;

void emitTestSuite(const std::string &outputDir, const std::string &programFile,
                   const std::string &entryFunction,
                   const std::string &architecture,
                   const std::string &specification, bool foundViolation,
                   bool coverBranches, Map2Check::Map2CheckMode mode) {
  Map2Check::TestSuiteMetadata metadata;
  metadata.producer = std::string("Map2Check ") + Map2CheckVersion;
  metadata.specification = specification;
  metadata.programFile = programFile;
  metadata.programHash = Map2Check::sha256File(programFile);
  metadata.entryFunction = entryFunction;
  metadata.architecture = architecture;
  metadata.creationTime = Map2Check::isoUtcNow();

  Map2Check::TestSuiteWriter writer(outputDir);
  if (!writer.writeMetadata(metadata)) {
    Map2Check::Log::Warning("could not write test-suite metadata to " +
                            outputDir);
    return;
  }
  // Cover-Branches: one test case per path KLEE explored, taken from its own
  // .ktest output. Nothing is asked of the instrumented program -- measured,
  // when it was: making it write a log per state turned a 1-second run that
  // answered FALSE into a 100-second run that exhausted its budget and
  // answered TRUE, because each write is an external call KLEE executes
  // concretely. Reading afterwards costs the finished search nothing.
  //
  // coversError is false throughout: these vectors are paths, not violations.
  // The violating one, when there is one, is still in klee_log.csv and still
  // goes out under Cover-Error.
  if (coverBranches) {
    std::vector<std::vector<std::string>> vectors =
        Map2Check::readKtestVectors(Map2Check::kleeOutputDir,
                                    kMaxBranchTestCases);
    for (const std::vector<std::string> &inputs : vectors) {
      if (!writer.writeTestCase(inputs, false)) {
        Map2Check::Log::Warning("could not write test case to " + outputDir);
        return;
      }
    }
    Map2Check::Log::Info("Test suite written to " + outputDir + " (" +
                         std::to_string(vectors.size()) +
                         " test cases from KLEE paths)");
    return;
  }

  // Last chance to find a vector before declaring the suite empty.
  //
  // A run can reach the target and still not record it: the state that reaches
  // it aborts, an aborted KLEE state runs no exit handler, and
  // map2check_property is written by that handler. So foundViolation being
  // false does not mean nothing was found -- it means nothing was WRITTEN
  // DOWN. Measured: twelve tasks whose suites covered the error came back with
  // no verdict at all.
  //
  // Safe to consult now, and only now, because NonDetPass rewrites the
  // abort-based assumptions into path pruning: an abort.err can no longer be
  // an assumption failure, so its presence in reachability mode is evidence
  // the target really was hit.
  if (!foundViolation && mode == Map2Check::Map2CheckMode::REACHABILITY_MODE) {
    std::vector<std::string> recovered =
        Map2Check::readViolatingKtest(Map2Check::kleeOutputDir);
    if (!recovered.empty()) {
      Map2Check::Log::Warning(
          "the runtime recorded no violation, but KLEE reported an aborting "
          "path -- emitting its input vector (" +
          std::to_string(recovered.size()) + " inputs)");
      if (writer.writeTestCase(recovered, true)) {
        Map2Check::Log::Info("Test suite written to " + outputDir + " (" +
                             std::to_string(recovered.size()) + " inputs)");
        return;
      }
    }
  }

  // No violation means no test case, but the suite still has to exist. A
  // missing test-suite/ directory reads to the competition harness as a tool
  // that crashed; a suite carrying metadata and zero test cases says the tool
  // ran and found nothing, which is a legitimate and scoreable outcome.
  if (!foundViolation) {
    Map2Check::Log::Info("Test suite written to " + outputDir +
                         " (no violation found -- 0 test cases)");
    return;
  }

  std::vector<std::string> inputs =
      Map2Check::readNonDetLog(Map2Check::kleeLogCSV);

  // Fall back to KLEE's record of the failing path when the runtime's log is
  // empty, which is the common case rather than the exception. The state that
  // reaches the target aborts; an aborted state runs no exit handler; the
  // flush that writes klee_log.csv never happens. Measured on the Test-Comp
  // corpus: 290 of 376 runs that reported FAILED emitted a test case with zero
  // <input> elements, and TestCov scored every one of them as covering
  // nothing -- the tool had found the bug and could not prove it.
  //
  // The log stays the first choice where it exists: it records values in the
  // order the program consumed them, straight from the instrumented run, while
  // the .ktest is KLEE's view of the same path.
  //
  // Deliberately NOT gated on the runtime having recorded the violation, and
  // that took two measurements to get right.
  //
  // The danger this fallback had was picking up an assumption failure: the
  // sv-benchmarks idiom is `void assume_abort_if_not(int c){ if(!c) abort(); }`
  // and an aborting assumption leaves an abort.err indistinguishable from a
  // real one. A suite built from it carried 25 inputs and covered 0.0%.
  //
  // The first attempt at a defence was to require foundViolation. It cost
  // more than it saved: measured pairwise against the previous run, twelve
  // tasks that HAD been covered stopped being covered, every one of them a
  // case where the target really was reached and the runtime simply never
  // recorded it -- the state that reaches the target aborts, and an aborted
  // state runs no exit handler, so map2check_property stays empty exactly
  // when it matters most.
  //
  // The real defence is upstream: NonDetPass now rewrites the abort-based
  // assumptions into path pruning, so an abort.err can no longer come from
  // one. With the source of false aborts removed, requiring foundViolation
  // only blocks legitimate recoveries.
  if (inputs.empty()) {
    inputs = Map2Check::readViolatingKtest(Map2Check::kleeOutputDir);
    if (!inputs.empty()) {
      Map2Check::Log::Info(
          "nondet log was empty; recovered the violating input vector from "
          "KLEE's test output (" + std::to_string(inputs.size()) + " inputs)");
    }
  }

  if (!writer.writeTestCase(inputs, true)) {
    Map2Check::Log::Warning("could not write test case to " + outputDir);
    return;
  }
  Map2Check::Log::Info("Test suite written to " + outputDir + " (" +
                       std::to_string(inputs.size()) + " inputs)");
}

inline void help_msg() {
  std::cout << std::endl;
  std::cout << "> > > \t  " << Map2CheckVersion << " \t < < <" << std::endl;
  std::cout << std::endl;
  std::cout << "Usage: map2check [options] file.[i|c|bc]\n";
  std::cout << std::endl;
}

inline int MIN(int a, int b) {
  if (a > b) {
    return b;
  }
  return a;
}

inline void fixPath(char *map2check_bin_string) {
  Map2Check::Log::Debug("Fixing Map2Check path");

  const int kSZLength = 32;
  char szTmp[kSZLength];
  const int kBufferLength = 500;
  char pBuf[kBufferLength];
  snprintf(szTmp, kSZLength, "/proc/%d/exe", getpid());
  // TODO(hbgit): fix implicit conversion from bytes
  ssize_t bytes = MIN(readlink(szTmp, pBuf, kBufferLength), kBufferLength - 1);
  std::string map2check_bin(map2check_bin_string);
  int deleteSpace = 0;
  if (map2check_bin.size() > 9) {
    deleteSpace = 10;
  } else {
    deleteSpace = 9;
  }

  if (bytes >= 0) {
    pBuf[bytes - deleteSpace] = '\0';
  } else {
  }

  // setenv copies its arguments, avoiding the strcpy/putenv pattern
  // (CWE-119) and the manually leaked buffers it required
  setenv("MAP2CHECK_PATH", pBuf, 1);
  // Map2Check::Log::Debug(pBuf);

  std::string klee_env_var(pBuf);
  klee_env_var += "/lib/klee/runtime";
  setenv("KLEE_RUNTIME_LIBRARY_PATH", klee_env_var.c_str(), 1);

  // The previous value has to be read, not spelled. This was
  //
  //     std::string ld_env_var("$LD_LIBRARY_PATH:");
  //
  // and setenv does no shell expansion, so the environment ended up with the
  // literal seven-plus characters "$LD_LIBRARY_PATH" as its first component --
  // a path that does not exist. Everything the image had put there was gone
  // for every child process map2check spawns.
  //
  // It went unnoticed because the install prefix appended below carries the
  // libraries the tool itself needs, so nothing map2check owns ever missed
  // them. It surfaced the moment an external tool was added: sbt-slicer links
  // dg's shared libraries from /opt/dg/lib, which the image puts on the path,
  // and every invocation died with "libdganalysis.so: cannot open shared
  // object file".
  std::string ld_env_var;
  const char *previous_ld_path = getenv("LD_LIBRARY_PATH");
  if (previous_ld_path != nullptr && previous_ld_path[0] != '\0') {
    ld_env_var = std::string(previous_ld_path) + ":";
  }
  ld_env_var += pBuf;
  ld_env_var += "/lib/";
  setenv("LD_LIBRARY_PATH", ld_env_var.c_str(), 1);
}
}  // namespace

// TODO(hbgit): add support to reachability (check old version of map), maybe
// this should be handled by caller
void generate_witness(std::string pathfile,
                      Map2Check::PropertyViolated propertyViolated,
                      std::string specTrue) {
  Map2Check::Log::Info("Generating witness.");

  GenHash genhashkey;
  // BUG: we should check if path is relative or absolute
  genhashkey.setFilePath(pathfile);
  genhashkey.generate_sha1_hash_for_file();

  Map2Check::Log::Debug("Generated hash");

  if ((propertyViolated != Map2Check::PropertyViolated::NONE) &&
      (propertyViolated != Map2Check::PropertyViolated::UNKNOWN)) {
    Map2Check::Log::Info("Starting Error Automata Generation");
    Map2Check::SVCompWitness svcomp(pathfile,
                                    genhashkey.getOutputSha1HashFile());
    svcomp.Testify();

  } else if (propertyViolated == Map2Check::PropertyViolated::NONE) {
    // Correctness witness
    Map2Check::Log::Info("Starting Correctness Automata Generation");
    Map2Check::SVCompWitness svcomp(
        pathfile, genhashkey.getOutputSha1HashFile(), "", specTrue);
    svcomp.Testify();
  }
}

/** Struct to hold some of map2check arguments */
struct map2check_args {
  unsigned timeout = 0;
  std::string inputFile;
  std::string function;
  std::string entryFunction = "main";
  std::string solvername;
  std::string expectedResult = "";
  bool wasmMode = false;
  Map2Check::Map2CheckMode mode;
  bool generateWitness = false;
  bool debugMode = false;
  bool generateTestCase = false;
  bool printCounterExample = false;
  bool btree = false;
  bool invCrabLlvm = false;
  bool generateTestSuite = false;
  bool coverBranches = false;
  bool seedExchange = false;
  bool sliceProgram = false;
  std::string testSuiteDir = "test-suite";
  std::string propertyFile;
  std::string architecture = "64bit";
  Map2Check::NonDetGenerator generator;
  std::string spectTrue = "safetyMemory";
};

bool foundViolation = false;
int map2check_execution(map2check_args args) {
  Map2Check::Log::Info("Started Map2Check");
  // TODO(rafa.sa.xp@gmail.com): Check current mode

  auto generator = args.generator;
  bool is_llvmir_in = false;

  /**
   * Start Map2Check algorithm
   * (1) Compile file and check for compiler warnings
   * (2) Instrument functions for current mode
   * (3) Apply nondeterministic mode and execute analysis
   * (4) Retrieve results
   * (5) Generate witness (if analysis generated a result)
   * (6) Clean map2check execution (folders and temp files)
   **/
  // (1) Compile file and check for compiler warnings
  // Check if input file is supported
  std::string extension = fs::path(args.inputFile).extension().string();
  if (extension.compare(".c") && extension.compare(".i") &&
      extension.compare(".bc") && extension.compare(".wasm")) {
    help_msg();
    return ERROR_IN_COMMAND_LINE;
  } else if (extension.compare(".bc") == 0) {
    is_llvmir_in = true;
  }

  if (args.wasmMode) {
    Map2Check::Log::Info("WASM mode: lifting " + args.inputFile);
    Map2Check::WasmLifterConfig lifterCfg;
    lifterCfg.wasm2cPath = "wasm2c";
    lifterCfg.clangPath = "/usr/bin/clang-16";
    lifterCfg.wasmRtIncludePath = "/opt/wabt-1.0.41/include";
    // Fallback for CI: try /opt/wabt/include, then system /usr/include
    struct stat st;
    if (stat("/opt/wabt-1.0.41/include", &st) != 0) {
      if (stat("/opt/wabt/include", &st) == 0) {
        lifterCfg.wasmRtIncludePath = "/opt/wabt/include";
      } else {
        lifterCfg.wasmRtIncludePath = "/usr/include";
      }
    }
    lifterCfg.keepIntermediate = true;
    Map2Check::WasmLifter lifter(lifterCfg);
    Map2Check::WasmLifterResult result = lifter.lift(args.inputFile);
    args.inputFile = result.bitcodePath;
    args.entryFunction = result.entryPointName;
    // Generate wrapper that provides main() → calls wasm entry point properly
    std::string wrapperBc = Map2Check::Caller::generateWasmWrapperStatic(
        result.headerPath, result.entryPointName);
    // Link wrapper with lifted bitcode
    std::ostringstream linkCmd;
    linkCmd << Map2Check::llvmLinkBinary << " "
            << result.bitcodePath << " " << wrapperBc
            << " -o " << result.bitcodePath;
    system(linkCmd.str().c_str());
    args.entryFunction = "main";
    is_llvmir_in = true;
  }

  std::unique_ptr<Map2Check::Caller> caller;
  caller = std::make_unique<Map2Check::Caller>(args.inputFile, args.mode,
                                                  generator);
  caller->c_program_fullpath = args.inputFile;
  caller->seedExchange = args.seedExchange;
  caller->sliceProgram = args.sliceProgram;
  caller->setTimeout(args.timeout);
  caller->entryFunction = args.entryFunction;
  caller->wasmMode = args.wasmMode;

  // Availability was settled in main(), which refuses the run outright when
  // the generator is missing. Reaching here with invCrabLlvm set means the
  // generator is installed. The old code branched on !is_llvmir_in and so
  // never even looked at the flag for bitcode input -- how the baselines, the
  // CASTLE harness and the BenchExec wrappers all invoke the tool -- which is
  // why it was ignored without a trace (issue #54).
  if (!is_llvmir_in && args.invCrabLlvm) {
    caller->compileWithClam();
  } else {
    caller->compileCFile(is_llvmir_in);
  }

  if (args.btree) {
    caller->useBTree();
  }

  // (2) Instrument functions for current mode
  // BEFORE instrumentation, and the order was learned the hard way.
  //
  // Slicing after callPass fatally removes the instrumentation: the slicer
  // keeps what the target depends on, and the runtime calls that RECORD the
  // violation do not influence whether the target is reachable, so they are
  // exactly what gets dropped. Measured -- two tasks went from FAILED to
  // UNKNOWN while the bitcode shrank by 70% and 38%: the analysis still
  // reached the bug and no longer had the machinery to say so.
  //
  // Slicing the user's program first and instrumenting the slice keeps both
  // properties: the search space is smaller and the instrumentation is added
  // to what survives.
  //
  // Reachability only. Slicing needs a criterion, and Cover-Branches has none
  // -- every branch is the goal. Asking elsewhere is refused, not ignored.
  if (args.sliceProgram) {
    if (args.mode == Map2Check::Map2CheckMode::REACHABILITY_MODE) {
      caller->sliceWithRespectToTarget(args.function);
    } else {
      Map2Check::Log::Warning(
          "--slice applies to reachability only: there is no criterion to "
          "slice towards when the goal is coverage or a memory property. "
          "Analysing the whole program.");
    }
  }

  caller->callPass(args.function);
  caller->linkLLVM();

  // (3) Apply nondeterministic mode and execute analysis
  caller->applyNonDetGenerator();
  caller->executeAnalysis(args.solvername);

  // (4) Retrieve results
  // TODO(hbgit): create methods to generate counter example
  std::unique_ptr<Map2Check::CounterExample> counterExample =
      std::make_unique<Map2Check::CounterExample>(std::string(args.inputFile),
                                                    is_llvmir_in);

  Map2Check::PropertyViolated propertyViolated;

  // What the analysis actually recorded on disk, in map2check_property.
  Map2Check::PropertyViolated recorded = counterExample->getProperty();
  bool recordedAViolation =
      (recorded != Map2Check::PropertyViolated::NONE) &&
      (recorded != Map2Check::PropertyViolated::UNKNOWN);

  // A violation that was found and written down survives the budget expiring.
  //
  // KLEE keeps exploring other states after recording an error, so it is
  // routinely killed by `timeout` on a run that already succeeded. The timeout
  // check used to come first and overwrite the verdict, discarding a FALSE
  // whose counterexample was sitting in map2check_property. The budget running
  // out AFTER the tool did its job is not "unable to decide" -- it is a
  // decision plus a slow shutdown. See finding K in
  // docs/reports/2026-08-12-castle-juliet-findings.md.
  //
  // Scope is deliberately narrow, and the narrowing has to be spelled out in
  // the CONDITION and not merely in a comment: the timeout branch runs before
  // the LibFuzzer arm, so without this guard a LibFuzzer run whose crash could
  // not be replayed would have its property file trusted anyway -- the exact
  // evidence that should not be trusted. That is not hypothetical: under the
  // hybrid default every case runs LibFuzzer first with 0.2x the budget, and
  // "Forcing timeout" appears in 2031 of the 2526 raw logs of the v5 Juliet
  // baseline.
  bool evidenceIsTrustworthy =
      recordedAViolation &&
      (generator != Map2Check::NonDetGenerator::LibFuzzer ||
       caller->isVerified());

  if (evidenceIsTrustworthy && caller->isTimeout()) {
    Map2Check::Log::Warning(
        "Note: budget expired after a violation was already found -- keeping "
        "the violation");
    propertyViolated = recorded;
  } else if (caller->isTimeout()) {
    Map2Check::Log::Warning("Note: Forcing timeout");
    propertyViolated = Map2Check::PropertyViolated::UNKNOWN;
  } else if (!caller->isVerified() &&
             (generator == Map2Check::NonDetGenerator::LibFuzzer)) {
    Map2Check::Log::Warning("Note: Could not replicate error");
    propertyViolated = Map2Check::PropertyViolated::UNKNOWN;
  } else {
    propertyViolated = recorded;
  }

  // A violation nobody can demonstrate is not a violation worth reporting.
  //
  // KLEE can burn its whole budget, halt on the timer, and still leave
  // TARGET-REACHED in map2check_property: some state wrote the file through an
  // external call and was then terminated by --dump-states-on-halt without
  // ever running to the abort. The verdict logic above trusts that file, so
  // the run answers FAILED while emitting a suite with zero <input> elements.
  //
  // Measured on reducercommutativity/rangesum05.i with --nondet-generator
  // symex: 1446 paths explored, HaltTimer, zero .err files, klee_log.csv
  // empty, verdict FAILED, suite empty, TestCov UNKNOWN. This is where the
  // FAILED-but-NOT_COVERED cases come from -- 21% of the control arm's FAILED
  // verdicts in the v11 factorial.
  //
  // Downgrading costs nothing that was ever scored. Test-Comp scores the
  // SUITE, not the verdict, and a suite with no inputs covers nothing whether
  // the tool called it FAILED or UNKNOWN. What it buys is a precision number
  // that means something.
  //
  // Gated on generating a suite at all, and on the two sources emitTestSuite
  // itself consults -- so the check can never disagree with what gets written.
  if (args.generateTestSuite && !args.coverBranches &&
      args.mode == Map2Check::Map2CheckMode::REACHABILITY_MODE &&
      generator == Map2Check::NonDetGenerator::Klee &&
      propertyViolated != Map2Check::PropertyViolated::NONE &&
      propertyViolated != Map2Check::PropertyViolated::UNKNOWN) {
    const bool haveVector =
        !Map2Check::readNonDetLog(Map2Check::kleeLogCSV).empty() ||
        !Map2Check::readViolatingKtest(Map2Check::kleeOutputDir).empty();
    if (!haveVector) {
      Map2Check::Log::Warning(
          "the property file records a violation but no input vector could be "
          "recovered -- reporting UNKNOWN rather than a FAILED nothing can "
          "reproduce");
      propertyViolated = Map2Check::PropertyViolated::UNKNOWN;
    }
  }

  if (propertyViolated ==
      Map2Check::PropertyViolated::NONE) {  // This means that result was TRUE
    if (generator == Map2Check::NonDetGenerator::Klee) {
      // Map2Check::Log::Info("");
      // Map2Check::Log::Info("VERIFICATION SUCCEEDED");
      // if (args.generateWitness)
      // generate_witness(args.inputFile, propertyViolated, args.spectTrue);
      // TODO(hbgit): Fix this hack!!!
      if (caller->isVerified()) {
        Map2Check::Log::Info("Unable to prove or falsify the program.");
        Map2Check::Log::Info("VERIFICATION UNKNOWN");
      } else {
        Map2Check::Log::Info("");
        Map2Check::Log::Info("VERIFICATION SUCCEEDED");
        if (args.generateWitness)
          generate_witness(args.inputFile, propertyViolated, args.spectTrue);
      }
    }

  } else if (propertyViolated == Map2Check::PropertyViolated::UNKNOWN) {
    // Printed for every generator, not just KLEE. Guarded on Klee, an
    // undecided LibFuzzer run ended with NO verdict line at all, and a caller
    // that parses stdout for one -- every harness here, and the BenchExec
    // tool-info -- reads that silence as the tool having crashed. It is the
    // same defect as the discarded exit code (finding G), one layer up: the
    // analysis reached a conclusion and did not say so.
    //
    // Visible in the numbers: the fuzzer arm of the engine comparison recorded
    // ERROR for whole categories that had simply come back undecided.
    Map2Check::Log::Info("Unable to prove or falsify the program.");
    Map2Check::Log::Info("VERIFICATION UNKNOWN");
    if (args.debugMode && generator == Map2Check::NonDetGenerator::Klee) {
      counterExample->generateTestCase();
    }
  } else {
    Map2Check::Log::Info("Started counter example generation");
    counterExample->printCounterExample();
    foundViolation = true;
    if (args.generateTestCase) counterExample->generateTestCase();
    if (args.generateWitness)
      generate_witness(args.inputFile, propertyViolated, args.spectTrue);
  }

  // Emitted for every outcome, not only for a violation. Test-Comp scores the
  // suite, and a run that decides nothing still has to hand one over: the
  // competition harness reads an absent test-suite/ as a crashed tool rather
  // than as an empty result. The `foundViolation` flag decides whether the
  // suite carries a test case, not whether the suite exists.
  if (args.generateTestSuite) {
    // Relative paths resolve against the directory map2check was invoked
    // from, not the scratch directory the pipeline chdir'd into -- the suite
    // has to outlive cleanGarbage().
    std::string outputDir = args.testSuiteDir;
    if (!fs::path(outputDir).is_absolute()) {
      outputDir = caller->getOriginalPath() + "/" + outputDir;
    }
    emitTestSuite(outputDir, caller->c_program_fullpath, args.entryFunction,
                  args.architecture,
                  resolveSpecification(args.propertyFile,
                                       caller->getOriginalPath(), args.mode),
                  foundViolation, args.coverBranches, args.mode);
  }

  // (6) Clean map2check execution (folders and temp files)
  // Kept under --debug: every intermediate artefact of a run lives in that
  // scratch directory -- the compiled bitcode, the instrumented bitcode, the
  // nondet log -- and deleting it unconditionally makes the pipeline
  // impossible to inspect after the fact. Debug runs are already opting into
  // verbosity and disk use.
  if (args.debugMode) {
    Map2Check::Log::Info("Debug mode: keeping temp files in " +
                         caller->getScratchDir());
  } else {
    Map2Check::Log::Debug("Removing temp files");
    caller->cleanGarbage();
  }

  if (args.expectedResult != "") {
    if (args.expectedResult != counterExample->getViolatedProperty()) {
      Map2Check::Log::Fatal("Expected result failed");
      return ERROR_EXPECTED_RESULT;
    }
  }

  return SUCCESS;
}

int main(int argc, char **argv) {
  fixPath(argv[0]);
  try {
    // Define and parse the program options
    po::options_description desc("Options");
    desc.add_options()("help", "\tshow help")
        ("version", "\tprints map2check version")
        ("debug", "\tdebug mode")
        ("input-file", po::value<std::vector<std::string>>(),
                      "\tspecifies the files")
        ("nondet-generator", po::value<std::string>(),
                      R"(specifies the nondet-generator, valid values are fuzzer (libFuzzer),
symex (Klee))")
        ("smt-solver", po::value<std::string>()->default_value("z3"),
                      R"(specifies the smt-solver, valid values are stp (STP),
z3 (Z3 is default), btor (Boolector), and yices2 (Yices))")
        ("timeout", po::value<unsigned>(),
                      "\ttimeout for map2check execution")
        ("target-function", "\tchecks wether <target-functions> is reachable")
        ("target-function-name", po::value<std::string>()->default_value("__VERIFIER_error"),
                      R"(define the function name to be searched)")
        ("generate-testcase", "\tcreates c program with fail testcase (experimental)")
        ("memtrack", "\tcheck for memory errors")
        ("print-counter", "\tprint counterexample")
        ("memcleanup-property", "\tanalyze program for memcleanup errors")
        ("check-overflow", "\tanalyze program for overflow failures")
        ("check-asserts", "\tanalyze program and verify assert failures")
        ("add-invariants", "\tadding program invariants adopting Crab-LLVM")
        ("generate-witness", "\tgenerates witness file")
        ("generate-test-suite",
         "\temits a Test-Comp test suite reproducing the violation found")
        ("test-suite-dir", po::value<std::string>()->default_value("test-suite"),
         "\tdirectory to write the test suite into")
        ("slice",
         "\tslice the program with respect to the target before analysing it "
         "(reachability only; needs sbt-slicer)")
        ("seed-exchange",
         "\tlet the two engines hand each other input vectors through a shared "
         "seed corpus (hybrid runs; off by default)")
        ("cover-branches",
         "\temit one test case per path KLEE explored, from its .ktest output, "
         "instead of the single violating vector (Test-Comp Cover-Branches)")
        ("property-file", po::value<std::string>(),
         "\tproperty file whose contents go verbatim into <specification>")
        ("architecture", po::value<std::string>()->default_value("64bit"),
         "\tmachine model recorded in the test-suite metadata")
        ("expected-result", po::value<string>(), "\tspecifies type of violation expected")
        ("btree", "\tuses btree structure to hold information (experimental, use this "
        "if you are having memory problems)")
        ("wasm", "\tverify a WebAssembly (.wasm) binary via wasm2c lifting")
        ("entry-function", po::value<std::string>()->default_value("main"),
                      R"(define the entry function name (default: main))");

    po::positional_options_description p;
    p.add("input-file", -1);
    po::variables_map vm;
    // po::store(po::parse_command_line(argc, argv, desc), vm);
    po::store(
        po::command_line_parser(argc, argv).options(desc).positional(p).run(),
        vm);
    po::notify(vm);

    // cout << vm.count("input-file") << endl;

    map2check_args args;
    // Default mode
    args.mode = Map2Check::Map2CheckMode::MEMTRACK_MODE;
    // Handling with the options
    if (vm.count("version")) {
      std::cout << Map2CheckVersion << "\n";
      return SUCCESS;
    }
    if (vm.count("help") == 0 && vm.count("input-file") == 0) {
      help_msg();
      std::cout << desc;
      return ERROR_IN_COMMAND_LINE;
    }
    if (vm.count("help")) {
      help_msg();
      std::cout << desc;
      return SUCCESS;
    }
    if (vm.count("smt-solver")) {
      string solvername = vm["smt-solver"].as<string>();
      std::transform(solvername.begin(), solvername.end(),
                     solvername.begin(), [](unsigned char c){
                     return std::tolower(c); });

      std::vector<std::string> vSolver = {"z3", "stp", "btor", "yices2"};

      if ( !std::count(vSolver.begin(), vSolver.end(), solvername) ) {
        std::cout << "Solver not supported.\n\n";
        std::cout << desc;
        return ERROR_IN_COMMAND_LINE;
      } else {
        std::cout << "Adopting " + solvername + " solver... \n";
        args.solvername = solvername;
        // return SUCCESS;
      }
    }
    if (vm.count("expected-result")) {
      string expected = vm["expected-result"].as<string>();
      args.expectedResult = expected;
    }
    if (vm.count("timeout")) {
      unsigned timeout = vm["timeout"].as<unsigned>();
      args.timeout = timeout;
    }
    if (vm.count("target-function")) {
      string function = vm["target-function-name"].as<string>();
      args.function = function;
      args.mode = Map2Check::Map2CheckMode::REACHABILITY_MODE;
      args.spectTrue = "target-function";
    }
    if (vm.count("check-overflow")) {
      args.mode = Map2Check::Map2CheckMode::OVERFLOW_MODE;
      args.spectTrue = "overflow";
    }
    if (vm.count("check-asserts")) {
      args.mode = Map2Check::Map2CheckMode::ASSERT_MODE;
    }
    if (vm.count("memcleanup-property")) {
      args.mode = Map2Check::Map2CheckMode::MEMCLEANUP_MODE;
    }
    if (vm.count("btree")) {
      args.btree = true;
    }
    if (vm.count("add-invariants")) {
      args.invCrabLlvm = true;
    }
    if (vm.count("wasm")) {
      args.wasmMode = true;
      args.mode = Map2Check::Map2CheckMode::MEMTRACK_MODE;
      args.entryFunction = vm["entry-function"].as<std::string>();
    }

    if (vm.count("print-counter")) {
      args.printCounterExample = true;
    }
    if (vm.count("generate-witness")) {
      args.generateWitness = true;
    }
    if (vm.count("generate-test-suite")) {
      args.generateTestSuite = true;
    }
    if (vm.count("test-suite-dir")) {
      args.testSuiteDir = vm["test-suite-dir"].as<std::string>();
    }
    if (vm.count("slice")) {
      args.sliceProgram = true;
    }
    if (vm.count("seed-exchange")) {
      args.seedExchange = true;
    }
    if (vm.count("cover-branches")) {
      args.coverBranches = true;
      // The mode has to change too, not just the emitter. Without this the run
      // falls through to the MEMTRACK default and instruments memory tracking
      // for a task that checks no property -- which on the Test-Comp corpus
      // produced a broken module and an empty suite on all 110 ProductLines
      // tasks. Set here rather than beside the other mode flags because
      // --cover-branches is a goal, and the other flags are properties; this
      // is the one goal that implies the absence of a property.
      args.mode = Map2Check::Map2CheckMode::COVER_BRANCHES_MODE;
    }
    if (vm.count("property-file")) {
      args.propertyFile = vm["property-file"].as<std::string>();
    }
    if (vm.count("architecture")) {
      args.architecture = vm["architecture"].as<std::string>();
    }
    if (vm.count("generate-testcase")) {
      args.generateTestCase = true;
    }
    if (vm.count("debug")) {
      Map2Check::Log::ActivateDebugMode();
      args.debugMode = true;
      Map2Check::Log::Debug("Current path:");
      system("echo $MAP2CHECK_PATH");
    }
    if (vm.count("nondet-generator")) {
      string generatorname = vm["nondet-generator"].as<string>();
      std::transform(generatorname.begin(), generatorname.end(),
                     generatorname.begin(), [](unsigned char c){
                     return std::tolower(c); });

      std::vector<std::string> available_generators = {"fuzzer", "symex"};

      if ( !std::count(available_generators.begin(), available_generators.end(), generatorname) ) {
        std::cout << "Selected generator don't exist, available: ";
        for(auto &s : available_generators) {
          std::cout << " " << s << " ";
        }
        std::cout << desc << "\n";
        return ERROR_IN_COMMAND_LINE;
      } else {
        std::cout << "Adopting " + generatorname + " nondet-generator... \n";
        if(generatorname == available_generators[0])
          args.generator = Map2Check::NonDetGenerator::LibFuzzer;
        if(generatorname == available_generators[1])
          args.generator = Map2Check::NonDetGenerator::Klee;
      }
    } else {
      args.generator = Map2Check::NonDetGenerator::None;
    }
    // Validated here, next to the other option checks, rather than inside
    // map2check_execution: main() is the only place whose return value becomes
    // the process exit code, and asking for a capability the build does not
    // have is an argument problem, not an analysis outcome. Doing it once here
    // also avoids repeating it per generator on the hybrid path below.
    //
    // The exit code matters more than the message. A warning on stderr is what
    // this used to be, and on a BenchExec cluster it disappears into the run
    // log -- which is how --add-invariants stayed dead and unnoticed through
    // an entire baseline (issue #54).
    if (args.invCrabLlvm && !invariantGeneratorAvailable()) {
      Map2Check::Log::Error(
          "--add-invariants was requested but no invariant generator is "
          "installed. Crab-LLVM was renamed to Clam and this build does not "
          "ship it; rebuild with -DENABLE_CLAM=ON, or drop the flag. Refusing "
          "to continue rather than silently analysing without invariants.");
      return ERROR_UNAVAILABLE_CAPABILITY;
    }

    if (vm.count("input-file")) {
      std::string pathfile;
      pathfile = accumulate(
          std::begin(vm["input-file"].as<std::vector<std::string>>()),
          std::end(vm["input-file"].as<std::vector<std::string>>()),
          pathfile);

      // std::cout << pathfile << std::endl;
      fs::path absolute_path = fs::absolute(pathfile);
      args.inputFile = absolute_path.string();
      if(args.generator == Map2Check::NonDetGenerator::None) {
        args.generator = Map2Check::NonDetGenerator::LibFuzzer;
        int result = map2check_execution(args);
        if (result != SUCCESS) {
          return result;
        }
        if (!foundViolation) {
          args.generator = Map2Check::NonDetGenerator::Klee;
          result = map2check_execution(args);
          if (result != SUCCESS) {
            return result;
          }
        }
        // A third phase, and it is what closes the exchange loop.
        //
        // The order is fuzzer then KLEE, so KLEE's vectors -- written into the
        // seed corpus at the end of its phase -- have no consumer inside the
        // same run. Without this the exchange only ever paid off on a LATER
        // run. Handing them straight back to the fuzzer is what turns two
        // engines running in sequence into two engines that cooperate: KLEE
        // solves the guard the fuzzer could not reach by mutation, and the
        // fuzzer mutates outward from there far faster than KLEE can fork.
        //
        // Behind the flag: the hybrid was measured at 45% covered over 372
        // tasks in its current shape, and that number should keep meaning what
        // it means until this one is measured beside it.
        if (args.seedExchange && !foundViolation) {
          args.generator = Map2Check::NonDetGenerator::LibFuzzer;
          result = map2check_execution(args);
          if (result != SUCCESS) {
            return result;
          }
        }
      }
      else {
        int result = map2check_execution(args);
        if (result != SUCCESS) {
          return result;
        }
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return SUCCESS;
}
