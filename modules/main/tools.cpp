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
  this->path = pathToCSource;
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

std::string Tools::SourceCodeHelper::getFilePath() {
  return this->path;
}

//TODO: replace all should be replace first
std::string Tools::SourceCodeHelper::substituteWithResult(int line, std::string old_token, std::string result) {
  // Map2Check::Log::Debug("Replacing '" + old_token + "' with '" + result + "'");
  string toReplace = this->getLine(line);
  boost::replace_all(toReplace, old_token, result);
  boost::replace_all(toReplace, "  ", "");
  return toReplace;
}

std::string Tools::SourceCodeHelper::getLine(unsigned line) {
  // precondition: line >= 1 AND line < this->cFileLines.size();
  // Map2Check::Log::Debug("Line:" + 7);
  if (line < 1 || line >= cFileLines.size()) {
    return "";
  }

  return this->cFileLines[line - 1];
}







Tools::CheckViolatedProperty::CheckViolatedProperty(string path) {
  Map2Check::Log::Debug("Started reading file: " + path );
  this->path_name = path;
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
        } else if(line == "FALSE-DEREF") {
          Map2Check::Log::Debug("FALSE-DEREF found");
          this->propertyViolated = Tools::PropertyViolated::FALSE_DEREF;
        } else if(line == "FALSE-MEMTRACK") {
          Map2Check::Log::Debug("FALSE-MEMTRACK found");
          this->propertyViolated = Tools::PropertyViolated::FALSE_MEMTRACK;
        }else if(line == "UNKNOWN") {
          Map2Check::Log::Debug("UNKNOWN found");
          this->propertyViolated = Tools::PropertyViolated::UNKNOWN;  
        }else if(line == "NONE") {
          Map2Check::Log::Debug("NONE found");
          this->propertyViolated = Tools::PropertyViolated::NONE;
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

std::vector<Tools::KleeLogRow> Tools::KleeLogHelper::getListLogFromCSV(string path) {
  std::vector<Tools::KleeLogRow> listLog;
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

     if(tokens.size() == 6){
         Log::Debug("started klee log");
         Tools::KleeLogRow row;
         string id = tokens[0];
         string lineNumber = tokens[1];
         string scope = tokens[2];
         string functionName = tokens[3];
         string step = tokens[4];
         string value = tokens[5];
         row.id = id;
         row.line = lineNumber;
         row.scope = scope;
         row.functionName = functionName;
         row.step = step;
         row.value = value;
         Map2Check::Log::Debug(row );
         listLog.push_back(row);
     }

   }

  return listLog;
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
     string lineNumber = tokens[7];
     string functionName = tokens[8];
     string step = tokens[9];

     row.id = id;
     row.memoryAddress = memoryAddress;
     row.pointsTo = pointsTo;
     row.scope = scope;
     row.isFree = isFree;
     row.isDynamic = isDynamic ;
     row.varName = varName;
     row.lineNumber = lineNumber;
     row.functionName = functionName;
     row.step = step;
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
