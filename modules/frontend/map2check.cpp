

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
#define Map2CheckVersion "Map2Check 7.2-Fuzzer : Mon May 28 21:44:38 UTC 2018"

// TODO: should get preprocessor flags from CMake

namespace {

const size_t SUCCESS = 0;
const size_t ERROR_IN_COMMAND_LINE = 1;
// A helper function to simplify the main part.
template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
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

inline void fixPath(char* map2check_bin_string) {
  Map2Check::Log::Info("Fixing Map2Check path");
  namespace fs = boost::filesystem;
  const int kSZLength = 32;
  char szTmp[kSZLength];
  const int kBufferLength = 500;
  char pBuf[kBufferLength];
  snprintf(szTmp, kSZLength, "/proc/%d/exe", getpid());
  // TODO: fix implicit conversion from bytes
  int bytes = MIN(readlink(szTmp, pBuf, kBufferLength), kBufferLength - 1);
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

  char* map2check_env_array = new char[map2check_env_var.length() + 1];
  strcpy(map2check_env_array, map2check_env_var.c_str());
  putenv(map2check_env_array);
  Map2Check::Log::Debug(map2check_env_var);
}
}  // namespace

// TODO: add support to reachability (check old version of map), maybe this
// should be handled by caller
void generate_witness(std::string pathfile,
                      Map2Check::PropertyViolated propertyViolated) {
  Map2Check::Log::Info("Generating witness.");

  GenHash genhashkey;
  // BUG: we should check if path is relative or absolute
  genhashkey.setFilePath("../" + pathfile);
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

int map2check_execution(std::string inputFile) {
  Map2Check::Log::Info("Started Map2Check");
  // TODO(rafa.sa.xp@gmail.com): Check current mode
  auto mode = Map2Check::Map2CheckMode::MEMTRACK_MODE;
  auto generator = Map2Check::NonDetGenerator::LibFuzzer;

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
  std::string extension = boost::filesystem::extension(inputFile);
  if (extension.compare(".bc") && extension.compare(".c") &&
      extension.compare(".i")) {
    help_msg();
    return ERROR_IN_COMMAND_LINE;
  }

  std::unique_ptr<Map2Check::Caller> caller;
  // Check if compiling will be needed
  if (extension.compare(".bc")) {
    // Compile C file
    caller = boost::make_unique<Map2Check::Caller>(
        Map2Check::Caller::compileCFile(inputFile), mode, generator);
  } else {
    // C file already compiled
    caller = boost::make_unique<Map2Check::Caller>(inputFile, mode, generator);
  }
  caller->cprogram_fullpath = inputFile;

  // (2) Instrument functions for current mode
  caller->callPass();
  caller->linkLLVM();

  // (3) Apply nondeterministic mode and execute analysis
  caller->applyNonDetGenerator();
  caller->executeAnalysis();

  // (4) Retrieve results
  // TODO: create methods to generate counter example

  std::unique_ptr<Map2Check::CounterExample> counterExample =
      boost::make_unique<Map2Check::CounterExample>(std::string(inputFile));
  Map2Check::PropertyViolated propertyViolated = counterExample->getProperty();

  if (propertyViolated ==
      Map2Check::PropertyViolated::NONE) {  // This means that result was TRUE
    Map2Check::Log::Info("");
    Map2Check::Log::Info("VERIFICATION SUCCEDED");
  } else if (propertyViolated == Map2Check::PropertyViolated::UNKNOWN) {
    Map2Check::Log::Info("Unable to prove or falsify the program.");
    Map2Check::Log::Info("VERIFICATION UNKNOWN");
  } else {
    Map2Check::Log::Info("Started counter example generation");
    counterExample->printCounterExample();
  }

  // (5) Generate witness (if analysis generated a result)
  generate_witness(inputFile, propertyViolated);

  // (6) Clean map2check execution (folders and temp files)
  //caller->cleanGarbage();
  return SUCCESS;
}

int main(int argc, char** argv) {
  fixPath(argv[0]);
  try {
    // Define and parse the program options
    po::options_description desc("Options");
    desc.add_options()("help,h", "\tshow help")
      ("version,v", "\tprints map2check version")
      ("debug,d", "\t Debug mode")
      ("input-file,i", po::value<std::vector<std::string> >(),
        "\tspecifies the files, also works with <file.bc>");

    po::positional_options_description p;
    p.add("input-file", -1);
    po::variables_map vm;
    // po::store(po::parse_command_line(argc, argv, desc), vm);
    po::store(
        po::command_line_parser(argc, argv).options(desc).positional(p).run(),
        vm);
    po::notify(vm);
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
    if (vm.count("debug")) {
      Map2Check::Log::ActivateDebugMode();
    }
    if (vm.count("input-file")) {
      std::string pathfile;
      pathfile = accumulate(
          boost::begin(vm["input-file"].as<std::vector<std::string> >()),
          boost::end(vm["input-file"].as<std::vector<std::string> >()),
          pathfile);

      std::cout << pathfile << std::endl;
      return map2check_execution(pathfile);
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return SUCCESS;
}
