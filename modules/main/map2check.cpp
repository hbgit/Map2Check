
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
#include <cstdlib>
#include <memory>
using namespace std;

#include "caller.h"
// #include "verifier.h"
#include "witness.hpp"
#include "log.h"
#include "exceptions.hpp"
#include "tools.h"
#include "counter_example.hpp"
#include "witness.hpp"
#include "gen_crypto_hash.hpp"

namespace
{
  const size_t SUCCESS = 0;
  const size_t ERROR_IN_COMMAND_LINE = 1;
  const size_t ERROR_UNHANDLED_EXCEPTION = 2;

} // namespace

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const std::vector<T>& v)
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
  Map2Check::Log::Info("Started Map2Check");
  // TODO: Use unique ptr for caller
  std::unique_ptr<Caller> caller;
  namespace fs = boost::filesystem;

  // TODO: put those vars in a anonimous namespace
  fs::path p("lib/klee/runtime");
  fs::path klee_lib_path = fs::complete(p); // complete == absolute
  std::string klee_env_var = "KLEE_RUNTIME_LIBRARY_PATH=" + klee_lib_path.string();

  putenv((char*) klee_env_var.c_str() );

  try
    {
      /** Define and parse the program options
       */
      po::options_description desc("Options");
      desc.add_options()
	("help,h", "\tshow help")
	("input-file,i", po::value< std::vector<string> >(), "\tspecifies the files, also works only with <file.bc>")
	("target-function,f", po::value< string >(), "\tchecks if function can be executed")
	("expected-result,e", po::value< string >(), "\tspecifies what output should be, used for tests")
	("print-list-log,p", po::value< string >(), "\tprints list log durint counter example")
	;

      po::positional_options_description p;
      p.add("input-file", -1);

      po::variables_map vm;
      try
	{
	  po::store(po::command_line_parser(argc, argv).
		    options(desc).positional(p).run(), vm); // can throw

	  if ( vm.count("help") == 0 && vm.count("input-file") == 0) {
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
	    // TODO: Refactor for better reading
	    // cout << "Input file: "<< vm["input-file"].as< std::vector<string> >() << "\n";
	    std::string pathfile;
	    pathfile = accumulate(boost::begin(vm["input-file"]
					       .as< std::vector<string> >
					       ()),
				  boost::end(vm["input-file"]
					     .as< std::vector<string> >
					     ()), pathfile);

	    string extension = boost::filesystem::extension(pathfile);
	    if(extension.compare(".bc") && extension.compare(".c")){
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

	    if(extension.compare(".bc")) {
	      caller = make_unique<Caller>(Caller::compileCFile(pathfile));
	    } else {
	      caller  =  make_unique<Caller>(pathfile);
	    }

	    caller->parseIrFile();

	    Map2Check::Log::Info("Applying instrumentation");
	    if (vm.count("target-function")) {
					string function = vm["target-function"].as< string >();
					Map2Check::Log::Debug("Starting pass with function " + function );
					caller->callPass(function);
	      }
	    else {
	      caller->callPass();
	    }

	    caller->genByteCodeFile();
	    caller->linkLLVM();

	    Map2Check::Log::Info("Started klee execution");
	    caller->callKlee();

        Map2Check::Log::Info("Started counter example generation");
        std::unique_ptr<Map2Check::CounterExample> counterExample = make_unique<Map2Check::CounterExample>(std::string(pathfile));
	    counterExample->printCounterExample();
        namespace tools = Map2Check::Tools;
        tools::PropertyViolated propertyViolated = counterExample->getProperty();
        
        //Generating hash key to the witness
        GenHash genhashkey;
        genhashkey.setFilePath(pathfile);
        genhashkey.generate_sha1_hash_for_file();
        
        if ((propertyViolated != tools::PropertyViolated::NONE) && (propertyViolated != tools::PropertyViolated::UNKNOWN)){
            if (vm.count("target-function")) {
                string function = vm["target-function"].as< string >();
                Map2Check::SVCompWitness svcomp(pathfile, genhashkey.getOutputSha1HashFile(), function);
                svcomp.Testify();
            }
            else {
                Map2Check::SVCompWitness svcomp(pathfile, genhashkey.getOutputSha1HashFile());
                svcomp.Testify();
            }
        }


			if (vm.count("expected-result")) {
					string function = vm["expected-result"].as< string >();
					Map2Check::Log::Debug("Expected result: " + function );
					
					if (counterExample->getViolatedProperty() == function) {
                        caller->cleanGarbage();
						return SUCCESS;
					}
					else {
						 throw Map2Check::Exceptions::Map2CheckException("Unexpected result");
					}
			} 
			
            caller->cleanGarbage();
	    return SUCCESS;
	  }


	  po::notify(vm); // throws on error, so do after help in case
	  // there are any problems
	}
      catch(po::error& e)
	{
	  Map2Check::Log::Fatal(e.what());
	  std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
	  std::cerr << desc << std::endl;
	  return ERROR_IN_COMMAND_LINE;
	}

      // application code here //

    }
  catch(std::exception& e)
    {
      Map2Check::Log::Fatal(e.what());
      std::cerr << "Unhandled Exception reached the top of main: "
		<< e.what() << ", application will now exit" << std::endl;
      return ERROR_UNHANDLED_EXCEPTION;

    }

  return SUCCESS;

}
