#include "verifier.h"

#include <string>
#include <vector>


#include <boost/filesystem.hpp>
using namespace boost;
#include "log.h"



namespace fs = boost::filesystem;
// using namespace Map2Check;



std::vector<std::string> Witness::listFilesFromFolder(std::string kleeResultFolder) {
 
  
  Map2Check::Log::Debug("Obtaining files from folder: " + kleeResultFolder);

  // Checks if folder exists
  if (!fs::exists(kleeResultFolder))
  {
    Map2Check::Log::Fatal("Folder does not exist");
    throw WitnessException::INVALID_FILE;
  }
  else {
    Map2Check::Log::Debug("Folder does exist");


    
  }
}
