
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

#include "caller.hpp"
// #include "verifier.h"
#include "utils/log.hpp"
#include "exceptions.hpp"
#include "utils/tools.hpp"
#include "counter_example/counter_example.hpp"
#include "witness/witness_include.hpp"
//#include "witness/witness.hpp"
#include "utils/gen_crypto_hash.hpp"

#define Map2CheckVersion "Map2Check 7.0a : Mon Oct  9 14:19:28 -04 2017"

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


int main(int argc, char** argv)
{
  std::unique_ptr<Caller> caller;
  namespace fs = boost::filesystem;

  
  #ifndef __unix__
    throw Map2Check::Exceptions::Map2CheckException("Map2Check only supports unix environments");
  #endif

  char szTmp[32];
  int len = 500;
  char pBuf[len];
  sprintf(szTmp, "/proc/%d/exe", getpid());
  int bytes = MIN(readlink(szTmp, pBuf, len), len - 1);
  std::string map2check_bin(argv[0]);
  int deleteSpace = 0;
  if(map2check_bin.size() > 9) {
    deleteSpace = 10;
  }
  else {
    deleteSpace = 9;
  }
  if(bytes >= 0) {
    pBuf[bytes - deleteSpace] = '\0';
  }
  else {
    // throw error
  }

  std::string map2check_env_var("MAP2CHECK_PATH=");
  map2check_env_var += pBuf; 

  
  
  putenv((char*) map2check_env_var.c_str() );

  // TODO: put those vars in a anonimous namespace
  // fs::path p("${MAP2CHECK_PATH}/lib/klee/runtime");
  // fs::path klee_lib_path = fs::complete(p); // complete == absolute
  std::string klee_env_var("KLEE_RUNTIME_LIBRARY_PATH=");
  klee_env_var += pBuf;
  klee_env_var += "/lib/klee/runtime";


  putenv((char*) klee_env_var.c_str() );
  //std::system("echo $KLEE_RUNTIME_LIBRARY_PATH");
  
  

  try
    {
      /** Define and parse the program options
       */
      po::options_description desc("Options");
      desc.add_options()
	("help,h", "\tshow help")
    ("version,v", "\tprints map2check version")
	("input-file,i", po::value< std::vector<string> >(), "\tspecifies the files, also works only with <file.bc>")
	("target-function,f", po::value< string >(), "\tchecks if function can be executed")
    ("check-overflow,o", "\tchecks if an integer overflow can occur")
	("expected-result,e", po::value< string >(), "\tspecifies what output should be, used for tests")
    ("print-list-log,p", "\tprints list log during counter example")
    ("generate-witness,w", "\tgenerate witness file")
    ("generate-instrumentated-only", "\tgenerate instrumentated file and stops")
    ("debug-info,d", "\tprints debug info")
    ("assume-malloc-true,m", "\tassumes that all mallocs will not fail")

	;

      po::positional_options_description p;
      p.add("input-file", -1);

      po::variables_map vm;
      try
	{
	  po::store(po::command_line_parser(argc, argv).
		    options(desc).positional(p).run(), vm); // can throw

      if ( vm.count("version")  )
        {
          cout << Map2CheckVersion << "\n";
          return SUCCESS;
        }
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



      if ( !vm.count("debug-info")  ) {
          Map2Check::Log::initLog();

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
        if(extension.compare(".bc") && extension.compare(".c") && extension.compare(".i")){
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
        Map2Check::Log::Info("Started Map2Check");
	    if(extension.compare(".bc")) {
	      caller = make_unique<Caller>(Caller::compileCFile(pathfile));
	    } else {
	      caller  =  make_unique<Caller>(pathfile);
	    }

		caller->cprogram_fullpath = pathfile;
		
	    caller->parseIrFile();
        bool sv_comp = false;
        if (vm.count("assume-malloc-true")) {
                    sv_comp = true;
        }

        /* Checks for which mode map2check will operate, can throw error if user calls multiple modes at once */
        // BEGIN CHECKING OPERATION MODE

        Map2CheckMode map2checkMode = Map2CheckMode::MEMTRACK_MODE;

        if (vm.count("target-function")) {
            if(map2checkMode != Map2CheckMode::MEMTRACK_MODE) {
                throw Map2Check::Exceptions::Map2CheckException("Map2Check does not support multiple analysis at the same time");
            }
            map2checkMode = Map2CheckMode::REACHABILITY_MODE;
        }

        if (vm.count("check-overflow")) {
            if(map2checkMode != Map2CheckMode::MEMTRACK_MODE) {
                throw Map2Check::Exceptions::Map2CheckException("Map2Check does not support multiple analysis at the same time");
            }
            map2checkMode = Map2CheckMode::OVERFLOW_MODE;
        }

        switch(map2checkMode) {
        case (Map2CheckMode::REACHABILITY_MODE):  {
            string function = vm["target-function"].as< string >();
            caller->callPass(map2checkMode, function, sv_comp);
            break;
        }
        default: {
            caller->callPass(map2checkMode, sv_comp);
        }


        }

        // END CHECKING OPERATION MODE
		
	    caller->genByteCodeFile();
        if ( vm.count("generate-instrumentated-only")  )
          {
            return SUCCESS;
          }
	    caller->linkLLVM();

	    Map2Check::Log::Info("Started klee execution");
	    caller->callKlee();

	//Check verification result [TRUE or FALSE]
	namespace tools = Map2Check::Tools;
	std::unique_ptr<Map2Check::CounterExample> counterExample = make_unique<Map2Check::CounterExample>(std::string(pathfile));
    tools::PropertyViolated propertyViolated = counterExample->getProperty();
	if(propertyViolated == tools::PropertyViolated::NONE){ // This means that result was TRUE	   
        Map2Check::Log::Info(" \n");
        Map2Check::Log::Info("VERIFICATION SUCCEDED \n");        
	   	   
	}else if(propertyViolated == tools::PropertyViolated::UNKNOWN)
	{
		Map2Check::Log::Info("Unable to prove or falsify the program. \n");
        Map2Check::Log::Info("VERIFICATION UNKNOWN \n"); 
	}else{
        
            Map2Check::Log::Info("Started counter example generation");
	    //std::unique_ptr<Map2Check::CounterExample> counterExample = make_unique<Map2Check::CounterExample>(std::string(pathfile));
	    counterExample->printCounterExample();
            //namespace tools = Map2Check::Tools;
            //tools::PropertyViolated propertyViolated = counterExample->getProperty();
	}

        
        if(vm.count("generate-witness")) {
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
            }//TODO: Add correctness witness

            return SUCCESS;
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
