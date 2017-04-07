#include "tools.h"
#include "log.h"
#include "exceptions.hpp"

#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <sstream>

#include <regex>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace Tools = Map2Check::Tools;

Tools::SourceCodeHelper::SourceCodeHelper(std::string pathToCSource) {
  Map2Check::Log::Debug("Reading C File");
  std::ifstream sourceFile;
  std::string line;
  sourceFile.open(pathToCSource.c_str());

  if(sourceFile.is_open()) {
    while(std::getline (sourceFile,line))  {
      this->cFileLines.push_back(line);
    }
    sourceFile.close();

  }
  else {
    throw Map2Check::Exceptions::ErrorOpeningFileException(pathToCSource);
  }

  Map2Check::Log::Debug(*this);

}

std::string Tools::SourceCodeHelper::substituteWithResult(int line, std::string old_token, std::string result) {
  Map2Check::Log::Debug("Replacing '" + old_token + "' with '" + result "'");
  string toReplace = this->getLine(line);
  boost::replace_all(toReplace, old_token, new_token);
}

std::string Tools::SourceCodeHelper::getLine(unsigned line) {
  // precondition: line >= 1 AND line < this->cFileLines.size();
  if (line < 1 || line >= cFileLines.size()) {
    throw Map2Check::Exceptions::OutOfBounds();
  }

  return this->cFileLines[line - 1];
}

std::vector<Tools::KleeResult> Tools::KleeResultHelper::GetKleeResults(std::string path) {
  KleeResultHelper kleeResultHelper;
  kleeResultHelper.genKleeResultsFromFolder(path);
  for(int i = 0; i < kleeResultHelper.kleeResults.size(); i++) {
    Map2Check::Log::Debug("Converting klee file to text file");
    Tools::KleeResultHelper::convertKleeFileToTextFileAndGetValues(&kleeResultHelper.kleeResults[i]);
  }
  return kleeResultHelper.kleeResults;
}

std::vector<Tools::KleeResult> Tools::KleeResultHelper::GetKleeResults() {
  return Tools::KleeResultHelper::GetKleeResults(Tools::kleeResultFolder);
}

void Tools::KleeResultHelper::convertKleeFileToTextFileAndGetValues(Tools::KleeResult* kleeResult) {
  Map2Check::Log::Debug("Converting ktest to test file");

  std::ostringstream command;
  command.str("");
  command << Map2Check::Tools::ktestBinary << " --write-ints ";
  command << kleeResult->name << ".ktest";
  command << " >> " << kleeResult->name << ".test";

  Map2Check::Log::Debug(command.str());
  system(command.str().c_str());

  std::regex reData("object .*: data: (.*)");
  // TODO: Use regex to determine number of objects
  //       and use fixed array list instead of vector

  // std::regex reNumObjects("num objects: (.*)");
  std::smatch match;
  std::string result;

  std::string line;
  std::ostringstream filename;
  filename.str("");
  filename  << kleeResult->name << ".test";

  std::ifstream testFile;
  testFile.open(filename.str().c_str());

  if(testFile.is_open()) {
    while ( std::getline (testFile,line) )
    {
      if (std::regex_search(line, match, reData) && match.size() > 1) {
	Map2Check::Log::Debug("Adding " + match.str(1) + " to " + kleeResult->name);
	int result = std::stoi(match.str(1));
	kleeResult->states.push_back(result);
      }
    }
    testFile.close();

  }
  else {
    throw Map2Check::Exceptions::ErrorOpeningFileException(filename.str() + ".test");
  }
}

void Tools::KleeResultHelper::genKleeResultsFromFolder(std::string kleeResultFolder) {
  Map2Check::Log::Debug("Obtaining files from folder: " + kleeResultFolder);

  // Checks if folder exists
  if (!fs::exists(kleeResultFolder) || !fs::is_directory(kleeResultFolder))
  {
    throw Map2Check::Exceptions::InvalidKleeFolderException(kleeResultFolder);
  }

  else {
    Map2Check::Log::Debug("Folder does exist, continuing");
    fs::directory_iterator end_iter;

    std::regex ktest("(.*)\.ktest");
    std::regex error("(.*)\.assert\.err");
    for( fs::directory_iterator dir_iter(kleeResultFolder) ; dir_iter != end_iter ; ++dir_iter){
      KleeResult result;
    	result.name = "";

      if (fs::is_regular_file(dir_iter->status())) {
        string current_file = dir_iter->path().string();
  	    // bool kleeError = false;

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
   	        string key = result.name;

            std::vector<KleeResult>::iterator it;
            it = std::find_if (kleeResults.begin(), kleeResults.end(), [key] (const KleeResult& o) -> bool {
              return o.name == key;
            });

            if (it != kleeResults.end()) {
              Map2Check::Log::Debug("Found old entry for " + it->name);
              // If two files with the same test name exists, it means that the test was an error test
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
}


Tools::CheckViolatedProperty::CheckViolatedProperty(string path) {
  Map2Check::Log::Debug("Started reading file: " + path );
  ifstream in(path.c_str());
  if (!in.is_open()) {
    throw Tools::CheckViolatedPropertyException("Could not open file");
  }

  string line;
  int fileLineNumber = 0;
  regex reLineNumber("Line: (.*)");
  regex reFunctionName("Function: (.*)");

  smatch match;
  string result;

  while (getline(in,line)) {
    switch (fileLineNumber) {
      case 0:
        if(line == "FALSE-FREE") {
          Map2Check::Log::Debug("FALSE-FREE found");
          this->propertyViolated = Tools::PropertyViolated::FALSE_FREE;
        } else if(line == "TARGET-REACHED") {
          Map2Check::Log::Debug("TARGET-REACHED found");
          this->propertyViolated = Tools::PropertyViolated::TARGET_REACHED;
        } else {
          throw Tools::CheckViolatedPropertyException("Invalid Property");
        }
        break;
      case 1:
        if (std::regex_search(line, match, reLineNumber) && match.size() > 1) {
  	  	    int result = std::stoi(match.str(1));
            this->line = result;
  	        Map2Check::Log::Debug("Line number: " + match.str(1));
        }
        else {
          throw Tools::CheckViolatedPropertyException("Could not find line number");
        }
        break;
      case 2:
        if (std::regex_search(line, match, reFunctionName) && match.size() > 1) {
            string result = match.str(1);
            this->function_name = result;
            Map2Check::Log::Debug("Function name: " + result);
        } else {
          throw Tools::CheckViolatedPropertyException("Could not find function name");
        }
        break;
    }
    fileLineNumber++;
  }

}

std::vector<Tools::ListLogRow> Tools::ListLogHelper::getListLogFromCSV(string path) {
  std::vector<Tools::ListLogRow> listLog;
  Map2Check::Log::Debug("Started reading file: " + path );

  using namespace boost;
  using namespace std   ;

  // Open file as READ mode
  ifstream in(path.c_str());
  if (!in.is_open()) {
    throw Tools::CouldNotOpenFileException();
  }

   string line;
   while (getline(in,line)) {

     // TODO: Check if CSV has valid arguments
     std::vector<std::string> tokens;
     boost::split(tokens, line, boost::is_any_of(";"));
     Tools::ListLogRow row;
     string id = tokens[0];
     string memoryAddress = tokens[1];
     string pointsTo = tokens[2];
     string scope = tokens[3];
     string isFree = tokens[4];
     string isDynamic = tokens[5];
     string varName = tokens[6];
     string functionName = tokens[7];

     row.id = id;
     row.memoryAddress = memoryAddress;
     row.pointsTo = pointsTo;
     row.scope = scope;
     row.isFree = isFree;
     row.isDynamic = isDynamic ;
     row.varName = varName;
     row.functionName = functionName;
     Map2Check::Log::Debug(row );
     listLog.push_back(row);
   }

  return listLog;
}


const char* Tools::CheckViolatedPropertyException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}

const char* Tools::CSVHelperException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}

const char* Tools::CouldNotOpenFileException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("Could not open input file");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}

const char* Tools::InvalidCSVException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("Invalid CSV content");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}
