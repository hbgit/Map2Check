#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <sstream>
#include <cstdlib>
#include <memory>
using namespace std;


#define Map2CheckVersion "Map2Check 7.1 : Wed Nov 22 22:30:11 -04 2017"

namespace {
    const size_t SUCCESS = 0;
    const size_t ERROR_IN_COMMAND_LINE = 1;
    const size_t ERROR_UNHANDLED_EXCEPTION = 2;

}  // namespace
// A helper function to simplify the main part.
    template<class T>
ostream& operator<<(ostream& os, const std::vector<T>& v) {
    copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
    return os;
}

void help_msg() {
    cout << endl;
    cout << "> > > \t  "<< Map2CheckVersion << " \t < < <" << endl;
    cout << endl;
    cout << "Usage: map2check [options] file.[i|c]\n";
    cout << endl;
}

int MIN(int a, int b) {
    if (a > b) {
        return b;
    }
    return a;
}


int main(int argc, char** argv) {
  try {
    /** Define and parse the program options
     */
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "\tshow help")
        ("version,v", "\tprints map2check version")
        ("input-file,i",
         po::value< std::vector<string> >(),
         "\tspecifies the files, also works only with <file.bc>");

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
  }
  catch(std::exception& e) {
    std::cerr << "Unhandled Exception reached the top of main: "
              << e.what() << ", application will now exit" << std::endl;
    return ERROR_UNHANDLED_EXCEPTION;
  }

  return SUCCESS;
}
