#include "tools.h"
#include "log.h"

#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <boost/algorithm/string.hpp>
#include <sstream>

namespace Tools = Map2Check::Tools;

std::vector<Tools::ListLogRow> Tools::ListLogHelper::getListLogFromCSV(string path) {
  std::vector<Tools::ListLogRow> listLog;
  Map2Check::Log::Debug("Started reading file: " + path );

  using namespace boost;
  using namespace std   ;

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
