#include "verifier.h"
#include "log.h"
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

Witness::Witness(string kleeResultFolder) {
  genKleeFilesListFromFolder(kleeResultFolder);
}

void Witness::genKleeFilesListFromFolder(std::string kleeResultFolder) {
  Map2Check::Log::Debug("Obtaining files from folder: " + kleeResultFolder);

  // Checks if folder exists
  if (!fs::exists(kleeResultFolder) || !fs::is_directory(kleeResultFolder))
  {
    throw InvalidKleeFolderException(kleeResultFolder);   
  }
  else {
    Map2Check::Log::Debug("Folder does exist, continuing");
    fs::directory_iterator end_iter;

    std::regex ktest("(.*\).ktest");
    std::regex error("(.*)\.assert.err");
    for( fs::directory_iterator dir_iter(kleeResultFolder) ; dir_iter != end_iter ; ++dir_iter)
      {
	KleeResult result;
	result.name = "";
	
	if (fs::is_regular_file(dir_iter->status()) )
	  {	   
	    string current_file = dir_iter->path().string();
	    bool kleeError = false;
	    
	    if(std::regex_match (current_file, ktest)) {
	      Map2Check::Log::Debug("Found ktest file: " + current_file); 
	      std::size_t pos = current_file.find(".ktest");
	      result.name =  current_file.substr(0,pos);
	      result.kleeStatus = KleeStatus::OK;
	      
	    } else if (std::regex_match (current_file, error)) {
	      Map2Check::Log::Debug("Found err file: " + current_file);
	      std::size_t pos = current_file.find(".assert.err");
	      result.name =  current_file.substr(0,pos);
	      result.kleeStatus = KleeStatus::ERROR;
	      
	    }

	    if (result.name != "") {
	      bool notFound = true;
	      string key = result.name;
	      
	      std::vector<KleeResult>::iterator it;
	      it = std::find_if (kleeResults.begin(),
				 kleeResults.end(),
				 [key] (const KleeResult& o) -> bool {
				   return o.name == key;});

	      if (it != kleeResults.end()) {
		Map2Check::Log::Debug("Found old entry for " + it->name);
		it->kleeStatus   = KleeStatus::ERROR;
	      }
	      else {
		Map2Check::Log::Debug("Didn't found old entry for " + result.name);
		kleeResults.push_back(result);
		
	      }
	     
	    }
	  }
      }
  }

  if (kleeResults.size() == 0) {
    throw InvalidKleeFolderException(kleeResultFolder);  
  }
}

const char* WitnessException::what() const throw() {
  std::ostringstream cnvt; 
  cnvt.str("");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();  
}

const char* InvalidKleeFolderException::what() const throw() {
  std::ostringstream cnvt; 
  cnvt.str("");
  cnvt << "Folder " <<  this->folder << " does not contain Klee Results!";
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();  
}
