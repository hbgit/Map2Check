
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/make_unique.hpp>
#include <boost/filesystem.hpp>


#include "caller.hpp"
#include "utils/log.hpp"

#define Map2CheckVersion "Map2Check 7.2-Fuzzer : Mon May 28 21:44:38 UTC 2018"

namespace {

const size_t SUCCESS = 0;
const size_t ERROR_IN_COMMAND_LINE = 1;
// const size_t ERROR_UNHANDLED_EXCEPTION = 2;

// A helper function to simplify the main part.
template<class T>
  std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

inline void help_msg() {
    std::cout << std::endl;
    std::cout << "> > > \t  "<< Map2CheckVersion << " \t < < <" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: map2check [options] file.[i|c]\n";
    std::cout << std::endl;
}
    /**inline int MIN(int a, int b) {
      if (a > b) {
      return b;
      }
      return a;
      }**/

inline void fixPath(char* map2check_bin_string) {
  // namespace fs = boost::filesystem;
  // const int kSZLength = 32;
  // char szTmp[kSZLength];
  // const int kBufferLength = 500;
  // char pBuf[kBufferLength];
  // snprintf(szTmp, kSZLength, "/proc/%d/exe", getpid());
  // int bytes = MIN(readlink(szTmp, pBuf, kBufferLength), kBufferLength - 1);
  // std::string map2check_bin(map2check_bin_string);
  // int deleteSpace = 0;
  // if (map2check_bin.size() > 9) {
  //   deleteSpace = 10;
  // } else {
  //   deleteSpace = 9;
  // }

  // if (bytes >= 0) {
  //   pBuf[bytes - deleteSpace] = '\0';
  // } else {
  //   // throw error
  // }

// std::string map2check_env_var("MAP2CHECK_PATH=");
// map2check_env_var += pBuf;

// char *map2check_env_array = new char[map2check_env_var.length() + 1];
// strcpy(map2check_env_array, map2check_env_var.c_str());
// putenv(map2check_env_array);
}
}  // namespace

int map2check_execution(std::string inputFile) {
  Map2Check::Log::Info("Started Map2Check");
  /**
   * Start Map2Check algorithm
   * (1) Compile file and check for compiler warnings
   * (2) Instrument functions for current mode
   * (3) Execute with AFL
   * (4) Retrieve results
   * (5) Generate witness (if analysis generated a result)
   **/
  // (1) Compile file and check for compiler warnings
  // Check if input file is supported
  std::string extension = boost::filesystem::extension(inputFile);
  if (extension.compare(".bc") &&
      extension.compare(".c") &&
      extension.compare(".i")) {
    help_msg();
    return ERROR_IN_COMMAND_LINE;
  }
  std::unique_ptr<Map2Check::Caller> caller;
  // Check if compiling will be needed
  if (extension.compare(".bc")) {
    // Compile C file
    caller = boost::make_unique<Map2Check::Caller>
      (Map2Check::Caller::compileCFile(inputFile));
  } else {
    // C file already compiled
    caller = boost::make_unique<Map2Check::Caller>(inputFile);
  }
  caller->cprogram_fullpath = inputFile;
  // (2) Instrument functions for current mode
  // TODO(rafa.sa.xp@gmail.com): Check current mode
  // caller->callPass(Map2Check::Map2CheckMode::MEMTRACK_MODE);
  // caller->linkLLVM();
  return SUCCESS;
}

// Small test, assumes that entry.bc exists and MAP2CHECK_PATH is configured
void test_map() {
  Map2Check::Caller caller("asd");
  caller.callPass(Map2Check::Map2CheckMode::MEMTRACK_MODE);
  caller.linkLLVM();
}

int main(int argc, char** argv) {
  try {
    // Define and parse the program options
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "\tshow help")
      ("version,v", "\tprints map2check version")
      ("input-file,i",
       po::value< std::vector<std::string> >(),
       "\tspecifies the files, also works with <file.bc>");

    po::positional_options_description p;
    p.add("input-file", -1);
    po::variables_map vm;
    // po::store(po::parse_command_line(argc, argv, desc), vm);
    po::store(po::command_line_parser(argc, argv)
            .options(desc).positional(p).run(), vm);
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
    if (vm.count("input-file")) {
      std::string pathfile;
      pathfile = accumulate(boost::begin(vm["input-file"]
        .as< std::vector<std::string> >()),
         boost::end(vm["input-file"]
        .as< std::vector<std::string> >
         ()),
         pathfile);

      std::cout << pathfile << std::endl;
      return map2check_execution(pathfile);
    }
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return SUCCESS;
}
