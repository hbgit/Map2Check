
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
using namespace boost;
namespace po = boost::program_options;

#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <sstream>
using namespace std;

#include "caller.h"

namespace
{
  const size_t ERROR_IN_COMMAND_LINE = 1;
  const size_t SUCCESS = 0;
  const size_t ERROR_UNHANDLED_EXCEPTION = 2;

} // namespace

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
    return os;
}

void help_msg(){
	cout << endl;
	cout << "> > > \t Map2Check v7 \t < < <" << endl;
	cout << endl;
	cout << "Usage: map2check [options] file.bc\n";
	cout << endl;
}


int main(int argc, char** argv)
{
	Caller* caller;

	try
	{
		/** Define and parse the program options
		 */
		po::options_description desc("Options");
		desc.add_options()
				("help,h", "\tshow help")
				("input-file,i", po::value< vector<string> >(), "\tspecifies the files, also works only with <file.bc>")
				("loop-pass,l", "\tanalisys code loops")
        ("func-pass,f", "\tconverts functions")
        ("output-file,o", "\tGenerates output file")
		;

		po::positional_options_description p;
		p.add("input-file", -1);

		po::variables_map vm;
		try
		{
			po::store(po::command_line_parser(argc, argv).
					  options(desc).positional(p).run(), vm); // can throw

			if (vm.count("input-file") == 0) {
				help_msg();
				cout << desc;
				return ERROR_IN_COMMAND_LINE;
			}

			/** --help option */
			if ( vm.count("help")  )
			{
				help_msg();
				cout << desc;
				return SUCCESS;
			}

			if (vm.count("input-file")) {
				cout << "Input file: "
                 << vm["input-file"].as< vector<string> >() << "\n";
                std::string pathfile;
				pathfile = accumulate(begin(vm["input-file"].as< vector<string> >()),
				               end(vm["input-file"].as< vector<string> >()), pathfile);
				string extension = boost::filesystem::extension(pathfile);
				if(extension.compare(".bc")){
					help_msg();
					cout << desc;
					return ERROR_IN_COMMAND_LINE;
				}
				/**
				 * Start Map2Check algorithm -- See SEFM'15 paper
				 * (1) Tracking variables
				 * (2) Generate values to nondet_* functions
				 * (3) Generate claims adopting ESBMC
				 * (4) Translating ESBMC claims into assertions
				 * (5) Added ESBMC claims in the analyzed code
				 * (6) Generating C code to execute the assertions
				 **/
				caller  = new Caller(pathfile);
				caller->parseIrFile();
				//return SUCCESS;
			}

			if (vm.count("loop-pass"))
			{
				caller->callPass();

				// return SUCCESS;
			}



      if (vm.count("output-file"))
			{
				caller->genByteCodeFile();

				return SUCCESS;
			}


			po::notify(vm); // throws on error, so do after help in case
						  // there are any problems
		}
		catch(po::error& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return ERROR_IN_COMMAND_LINE;
		}

		// application code here //

	}
	catch(std::exception& e)
	{
		std::cerr << "Unhandled Exception reached the top of main: "
			<< e.what() << ", application will now exit" << std::endl;
		return ERROR_UNHANDLED_EXCEPTION;

	}

	return SUCCESS;

}
