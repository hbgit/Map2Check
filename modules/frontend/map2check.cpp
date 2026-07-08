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
#include "utils/gen_crypto_hash.hpp"
#include "utils/log.hpp"
#include "witness/witness_include.hpp"
#include "wasm_lifter.hpp"

namespace po = boost::program_options;
namespace fs = std::filesystem;
#include "map2check_version.hpp"

// TODO(hbgit): should get preprocessor flags from CMake

namespace {

const size_t SUCCESS = 0;
const size_t ERROR_IN_COMMAND_LINE = 1;
// A helper function to simplify the main part.
template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
  return os;
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

  if (!is_llvmir_in) {
    if (args.invCrabLlvm) {
      // cout << "crab  \n";
      caller->compileToCrabLlvm();
    } else {
      caller->compileCFile(is_llvmir_in);
    }
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
  }

  // (6) Clean map2check execution (folders and temp files)
  Map2Check::Log::Debug("Removing temp files");
  caller->cleanGarbage();

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
