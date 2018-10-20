#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <boost/filesystem.hpp>
#include <boost/make_unique.hpp>

#include "caller.hpp"
#include "counter_example/counter_example.hpp"
#include "utils/gen_crypto_hash.hpp"
#include "utils/log.hpp"
#include "witness/witness_include.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
#define Map2CheckVersion "Map2Check 7.2-Fuzzer : Mon May 28 21:44:38 UTC 2018"

// TODO: should get preprocessor flags from CMake

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
  std::cout << "Usage: map2check [options] file.[i|c]\n";
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
  // TODO: fix implicit conversion from bytes
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

  std::string map2check_env_var("MAP2CHECK_PATH=");
  map2check_env_var += pBuf;

  char *map2check_env_array = new char[map2check_env_var.length() + 1];
  strcpy(map2check_env_array, map2check_env_var.c_str());
  putenv(map2check_env_array);
  // Map2Check::Log::Debug(map2check_env_var);

  std::string klee_env_var("KLEE_RUNTIME_LIBRARY_PATH=");
  klee_env_var += pBuf;
  klee_env_var += "/lib/klee/runtime";

  char *klee_env_array = new char[klee_env_var.length() + 1];
  strcpy(klee_env_array, klee_env_var.c_str());
  putenv(klee_env_array);


  std::string ld_env_var("LD_LIBRARY_PATH=");
  ld_env_var += "$LD_LIBRARY_PATH:";
  ld_env_var += pBuf;
  ld_env_var += "/lib/";

  char *ld_env_array = new char[ld_env_var.length() + 1];
  strcpy(ld_env_array, ld_env_var.c_str());
  putenv(ld_env_array);
}
}  // namespace

// TODO: add support to reachability (check old version of map), maybe this
// should be handled by caller
void generate_witness(std::string pathfile,
                      Map2Check::PropertyViolated propertyViolated) {
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
        pathfile, genhashkey.getOutputSha1HashFile(), "", "safetyMemory");
    svcomp.Testify();
  }
}

/** Struct to hold some of map2check arguments */
struct map2check_args {
  unsigned timeout = 0;
  std::string inputFile;
  std::string function;
  std::string expectedResult = "";
  Map2Check::Map2CheckMode mode;
  bool generateWitness = false;
  bool debugMode = false;
  bool generateTestCase = false;
  bool printCounterExample = false;
  bool btree = false;
  bool invCrabLlvm = false;
  Map2Check::NonDetGenerator generator;
};

bool foundViolation = false;
int map2check_execution(map2check_args args) {
  Map2Check::Log::Info("Started Map2Check");
  // TODO(rafa.sa.xp@gmail.com): Check current mode

  auto generator = args.generator;

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
  std::string extension = boost::filesystem::extension(args.inputFile);
  if (extension.compare(".c") && extension.compare(".i")) {
    help_msg();
    return ERROR_IN_COMMAND_LINE;
  }

  std::unique_ptr<Map2Check::Caller> caller;
  caller = boost::make_unique<Map2Check::Caller>(args.inputFile, args.mode,
                                                 generator);
  caller->c_program_fullpath = args.inputFile;
  if (args.invCrabLlvm)
  {
      //cout << "crab  \n";
      caller->compileToCrabLlvm();
  }else{
      caller->compileCFile();
  }
  caller->setTimeout(args.timeout);
  if (args.btree) {
    caller->useBTree();
  }

  // (2) Instrument functions for current mode
  caller->callPass(args.function);
  caller->linkLLVM();

  // (3) Apply nondeterministic mode and execute analysis
  caller->applyNonDetGenerator();
  caller->executeAnalysis();

  // (4) Retrieve results
  // TODO: create methods to generate counter example
  std::unique_ptr<Map2Check::CounterExample> counterExample =
      boost::make_unique<Map2Check::CounterExample>(
          std::string(args.inputFile));

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
      // TODO: Fix this hack!!!
      if (caller->isVerified()) {
        Map2Check::Log::Info("Unable to prove or falsify the program.");
        Map2Check::Log::Info("VERIFICATION UNKNOWN");
      } else {
        Map2Check::Log::Info("");
        Map2Check::Log::Info("VERIFICATION SUCCEEDED");
        if (args.generateWitness)
          generate_witness(args.inputFile, propertyViolated);
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
      generate_witness(args.inputFile, propertyViolated);
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
    desc.add_options()("help,h", "\tshow help")(
        "version,v", "\tprints map2check version")("debug,d", "\t Debug mode")(
        "input-file,i", po::value<std::vector<std::string>>(),
        "\tspecifies the files")("timeout,t", po::value<unsigned>(),
                                 "\tTimeout for map2check execution")(
        "target-function,f", "\tSearches for __VERIFIER_error is reachable")(
        "generate-testcase,g", "\tCreates c program with fail testcase")(
        "memtrack,m", "\tCheck for memory errors")
("print-counter,p",
                                                   "\tPrint Counterexample")(
        "check-overflow,o", "\tAnalyze program for overflow failures")(
        "check-asserts,c", "\tAnalyze program and verify assert failures")(
        "add-invariants,a", "\tAdding program invariants adopting Crab-LLVM")(
  "generate-witness,w", "\tGenerates witness file")(
        "expected-result,e", po::value<string>(),
        "\tSpecifies type of violation expected")(
        "btree,b",
        "\t Uses btree structure to hold information (experimental, use this "
        "if you are having memory problems)");

    po::positional_options_description p;
    p.add("input-file", -1);
    po::variables_map vm;
    // po::store(po::parse_command_line(argc, argv, desc), vm);
    po::store(
        po::command_line_parser(argc, argv).options(desc).positional(p).run(),
        vm);
    po::notify(vm);

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
    if (vm.count("expected-result")) {
      string expected = vm["expected-result"].as<string>();
      args.expectedResult = expected;
    }
    if (vm.count("timeout")) {
      unsigned timeout = vm["timeout"].as<unsigned>();
      args.timeout = timeout;
    }
    if (vm.count("target-function")) {
      string function = "__VERIFIER_error";
      args.function = function;
      args.mode = Map2Check::Map2CheckMode::REACHABILITY_MODE;
    }
    if (vm.count("check-overflow")) {
      args.mode = Map2Check::Map2CheckMode::OVERFLOW_MODE;
    }
    if (vm.count("check-asserts")) {
      args.mode = Map2Check::Map2CheckMode::ASSERT_MODE;
    }
    if (vm.count("btree")) {
      args.btree = true;
    }
    if (vm.count("add-invariants")) {
      args.invCrabLlvm = true;
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
    if (vm.count("input-file")) {
      std::string pathfile;
      pathfile = accumulate(
          boost::begin(vm["input-file"].as<std::vector<std::string>>()),
          boost::end(vm["input-file"].as<std::vector<std::string>>()),
          pathfile);

      std::cout << pathfile << std::endl;
      fs::path absolute_path = fs::absolute(pathfile);
      args.inputFile = absolute_path.string();
      args.generator = Map2Check::NonDetGenerator::LibFuzzer;
      map2check_execution(args);
      if (!foundViolation) {
        args.generator = Map2Check::NonDetGenerator::Klee;
        map2check_execution(args);
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return SUCCESS;
}
