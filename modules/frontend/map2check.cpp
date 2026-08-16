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
 * a manual run without --property-file still produces a valid suite. */
std::string resolveSpecification(const std::string &propertyFile,
                                 Map2Check::Map2CheckMode mode) {
  if (!propertyFile.empty()) {
    std::ifstream in(propertyFile);
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
    Map2Check::Log::Warning("could not read property file: " + propertyFile);
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
void emitTestSuite(const std::string &outputDir, const std::string &programFile,
                   const std::string &entryFunction,
                   const std::string &architecture,
                   const std::string &specification, bool coversError) {
  std::vector<std::string> inputs =
      Map2Check::readNonDetLog(Map2Check::kleeLogCSV);

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
  if (!writer.writeTestCase(inputs, coversError)) {
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

  std::string ld_env_var("$LD_LIBRARY_PATH:");
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

  // HACK: Fix this!!!
  if (caller->isTimeout()) {
    Map2Check::Log::Warning("Note: Forcing timeout");
    propertyViolated = Map2Check::PropertyViolated::UNKNOWN;
  } else if (!caller->isVerified() &&
             (generator == Map2Check::NonDetGenerator::LibFuzzer)) {
    Map2Check::Log::Warning("Note: Could not replicate error");
    propertyViolated = Map2Check::PropertyViolated::UNKNOWN;
  } else {
    propertyViolated = counterExample->getProperty();
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
    if (generator == Map2Check::NonDetGenerator::Klee) {
      Map2Check::Log::Info("Unable to prove or falsify the program.");
      Map2Check::Log::Info("VERIFICATION UNKNOWN");
      if (args.debugMode) counterExample->generateTestCase();
    }
  } else {
    Map2Check::Log::Info("Started counter example generation");
    counterExample->printCounterExample();
    foundViolation = true;
    if (args.generateTestCase) counterExample->generateTestCase();
    if (args.generateWitness)
      generate_witness(args.inputFile, propertyViolated, args.spectTrue);
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
                    resolveSpecification(args.propertyFile, args.mode), true);
    }
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
      abort();
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
        map2check_execution(args);
        if (!foundViolation) {
          args.generator = Map2Check::NonDetGenerator::Klee;
          map2check_execution(args);
        }
      }
      else {
        map2check_execution(args);
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return SUCCESS;
}
