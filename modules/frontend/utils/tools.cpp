/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * BOOST     -> BSL-1.0
 *
 * SPDX-License-Identifier: (GPL-2.0 AND BSL-1.0)
 **/

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

#include "log.hpp"
#include "tools.hpp"

namespace fs = boost::filesystem;
namespace Tools = Map2Check;

using std::ifstream;
using std::regex;
using std::smatch;

Tools::SourceCodeHelper::SourceCodeHelper(std::string pathToCSource) {
  Map2Check::Log::Debug("Reading C File");
  this->path = pathToCSource;
  std::ifstream sourceFile;
  std::string line;
  sourceFile.open(pathToCSource.c_str());

  if (sourceFile.is_open()) {
    while (std::getline(sourceFile, line)) {
      this->cFileLines.push_back(line);
    }
    sourceFile.close();

  } else {
    // throw Map2Check::Exceptions::ErrorOpeningFileException(pathToCSource);
  }

  // Map2Check::Log::Debug(*this);
}

std::string Tools::SourceCodeHelper::getFilePath() { return this->path; }

// TODO(hbgit): replace all should be replace first
std::string Tools::SourceCodeHelper::substituteWithResult(int line,
                                                          std::string old_token,
                                                          std::string result) {
  // Map2Check::Log::Debug("Replacing '" + old_token + "' with '" + result +
  // "'");
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
  Map2Check::Log::Debug("Started reading file: " + path);
  this->path_name = path;

  ifstream in(path.c_str());
  if (!in.is_open()) {
    Map2Check::Log::Debug(
        "Map2Check did not generate property file so it is unknown");
    this->propertyViolated = Tools::PropertyViolated::UNKNOWN;
    return;
  }

  string line;
  int fileLineNumber = 0;
  regex reLineNumber("Line: (.*)");
  regex reFunctionName("Function: (.*)");

  smatch match;
  string result;

  ifstream unknown("map2check_property_unknown");
  if (unknown.is_open()) {
    this->propertyViolated = Tools::PropertyViolated::UNKNOWN;
    return;
  }

  ifstream false_free("map2check_property_free");
  if (false_free.is_open()) {
    this->propertyViolated = Tools::PropertyViolated::FALSE_FREE;
    getline(false_free, line);
    getline(false_free, line);
    if (std::regex_search(line, match, reLineNumber) && match.size() > 1) {
      int result = std::stoi(match.str(1));
      this->line = result;
      Map2Check::Log::Debug("Line number: " + match.str(1));
    }
    getline(false_free, line);
    if (std::regex_search(line, match, reFunctionName) && match.size() > 1) {
      string result = match.str(1);
      this->function_name = result;
      Map2Check::Log::Debug("Function name: " + result);
    }

    return;
  }

  ifstream deref("map2check_property_deref");
  if (deref.is_open()) {
    this->propertyViolated = Tools::PropertyViolated::FALSE_DEREF;
    getline(deref, line);
    getline(deref, line);
    if (std::regex_search(line, match, reLineNumber) && match.size() > 1) {
      int result = std::stoi(match.str(1));
      this->line = result;
      Map2Check::Log::Debug("Line number: " + match.str(1));
    }
    getline(deref, line);
    if (std::regex_search(line, match, reFunctionName) && match.size() > 1) {
      string result = match.str(1);
      this->function_name = result;
      Map2Check::Log::Debug("Function name: " + result);
    }
    return;
  }

  ifstream assert_ifs("map2check_property_assert");
  if (assert_ifs.is_open()) {
    this->propertyViolated = Tools::PropertyViolated::ASSERT;
    Map2Check::Log::Debug("HI3");
    getline(assert_ifs, line);
    getline(assert_ifs, line);
    if (std::regex_search(line, match, reLineNumber) && match.size() > 1) {
      int result = std::stoi(match.str(1));
      this->line = result;
      Map2Check::Log::Debug("Line number: " + match.str(1));
    }
    getline(assert_ifs, line);
    if (std::regex_search(line, match, reFunctionName) && match.size() > 1) {
      string result = match.str(1);
      this->function_name = result;
      Map2Check::Log::Debug("Function name: " + result);
    }
    return;
  }

  ifstream overflowFile("map2check_property_overflow");
  if (overflowFile.is_open()) {
    this->propertyViolated = Tools::PropertyViolated::FALSE_OVERFLOW;
    getline(overflowFile, line);
    getline(overflowFile, line);
    if (std::regex_search(line, match, reLineNumber) && match.size() > 1) {
      int result = std::stoi(match.str(1));
      this->line = result;
      Map2Check::Log::Debug("Line number: " + match.str(1));
    }
    getline(overflowFile, line);
    if (std::regex_search(line, match, reFunctionName) && match.size() > 1) {
      string result = match.str(1);
      this->function_name = result;
      Map2Check::Log::Debug("Function name: " + result);
    }
    return;
  }

  ifstream memtrack("map2check_property_memtrack");
  if (memtrack.is_open()) {
    this->propertyViolated = Tools::PropertyViolated::FALSE_MEMTRACK;
    return;
  }

  ifstream memcleanup("map2check_property_memcleanup");
  if (memcleanup.is_open()) {
    this->propertyViolated = Tools::PropertyViolated::FALSE_MEMCLEANUP;
    return;
  }

  while (getline(in, line)) {
    switch (fileLineNumber) {
      case 0:
        if (line == "FALSE-FREE") {
          Map2Check::Log::Debug("FALSE-FREE found");
          this->propertyViolated = Tools::PropertyViolated::FALSE_FREE;
        } else if (line == "TARGET-REACHED") {
          Map2Check::Log::Debug("TARGET-REACHED found");
          this->propertyViolated = Tools::PropertyViolated::TARGET_REACHED;
        } else if (line == "FALSE-DEREF") {
          Map2Check::Log::Debug("FALSE-DEREF found");
          this->propertyViolated = Tools::PropertyViolated::FALSE_DEREF;
        } else if (line == "FALSE-MEMTRACK") {
          Map2Check::Log::Debug("FALSE-MEMTRACK found");
          this->propertyViolated = Tools::PropertyViolated::FALSE_MEMTRACK;
        } else if (line == "FALSE-MEMCLEANUP") {
          Map2Check::Log::Debug("FALSE-MEMCLEANUP found");
          this->propertyViolated = Tools::PropertyViolated::FALSE_MEMCLEANUP;
        } else if (line == "OVERFLOW") {
          Map2Check::Log::Debug("OVERFLOW found");
          this->propertyViolated = Tools::PropertyViolated::FALSE_OVERFLOW;
        } else if (line == "UNKNOWN") {
          Map2Check::Log::Debug("UNKNOWN found");
          this->propertyViolated = Tools::PropertyViolated::UNKNOWN;
        } else if (line == "NONE") {
          Map2Check::Log::Debug("NONE found");
          this->propertyViolated = Tools::PropertyViolated::NONE;
        } else if (line == "ASSERT") {
          Map2Check::Log::Debug("ASSERT found");
          this->propertyViolated = Tools::PropertyViolated::ASSERT;
        } else {
          // throw Tools::CheckViolatedPropertyException("Invalid Property");
        }
        break;
      case 1:
        if (std::regex_search(line, match, reLineNumber) && match.size() > 1) {
          int result = std::stoi(match.str(1));
          this->line = result;
          Map2Check::Log::Debug("Line number: " + match.str(1));
        } else {
          // throw Tools::CheckViolatedPropertyException(
          //  "Could not find line number");
        }
        break;
      case 2:
        if (std::regex_search(line, match, reFunctionName) &&
            match.size() > 1) {
          string result = match.str(1);
          this->function_name = result;
          Map2Check::Log::Debug("Function name: " + result);
        } else {
          // throw Tools::CheckViolatedPropertyException(
          //   "Could not find function name");
        }
        break;
    }
    fileLineNumber++;
  }
}

// int Tools::CheckViolatedProperty::getListLogFromCSV(string path) {

std::vector<Tools::KleeLogRow> Tools::KleeLogHelper::getListLogFromCSV(
    string path) {
  std::vector<Tools::KleeLogRow> listLog;
  Map2Check::Log::Debug("Started reading file: " + path);

  // using namespace boost;
  // using namespace std;
  using std::ifstream;
  using std::stoi;
  using std::string;
  using std::vector;

  // Open file as READ mode
  ifstream in(path.c_str());
  if (!in.is_open()) {
    // throw Tools::CouldNotOpenFileException();
  }

  string line;
  while (getline(in, line)) {
    // TODO(hbgit): Check if CSV has valid arguments
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of(";"));

    if (tokens.size() == 7) {
      Log::Debug("Started klee log");
      Tools::KleeLogRow row;
      string id = tokens[0];
      string lineNumber = tokens[1];
      string scope = tokens[2];
      string functionName = tokens[3];
      string step = tokens[4];
      string value = tokens[5];
      string type = tokens[6];
      Log::Debug(tokens[6]);
      switch (stoi(type)) {
        case 0:
          row.type = KleeLogType::INTEGER;
          break;
        case 1:
          row.type = KleeLogType::CHAR;
          break;
        case 2:
          row.type = KleeLogType::POINTER;
          break;
        case 3:
          row.type = KleeLogType::USHORT;
          break;
        case 4:
          row.type = KleeLogType::LONG;
          break;
        case 5:
          row.type = KleeLogType::UNSIGNED;
          break;
      }
      row.id = id;
      row.line = lineNumber;
      row.scope = scope;
      row.functionName = functionName;
      row.step = step;
      row.value = value;
      Map2Check::Log::Debug(row);
      listLog.push_back(row);
    }
  }

  return listLog;
}

std::vector<Tools::ListLogRow> Tools::ListLogHelper::getListLogFromCSV(
    string path) {
  std::vector<Tools::ListLogRow> listLog;
  Map2Check::Log::Debug("Started reading file: " + path);

  // using namespace boost;
  // using namespace std;

  // Open file as READ mode
  ifstream in(path.c_str());
  if (!in.is_open()) {
    return listLog;
  }

  string line;
  while (getline(in, line)) {
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of(";"));
    // TODO(hbgit): Check if CSV has valid arguments
    if (tokens.size() == 10) {
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
      row.isDynamic = isDynamic;
      row.varName = varName;
      row.lineNumber = lineNumber;
      row.functionName = functionName;
      row.step = step;
      Map2Check::Log::Debug(row);
      listLog.push_back(row);
    }
  }

  return listLog;
}

std::vector<Tools::StateTrueLogRow>
Tools::StateTrueLogHelper::getListLogFromCSV(string path) {
  std::vector<Tools::StateTrueLogRow> listLog;
  Map2Check::Log::Debug("Started reading file: " + path);

  // using namespace boost;
  // using namespace std;

  // Open file as READ mode
  ifstream in(path.c_str());
  if (!in.is_open()) {
    return listLog;
  }

  string line;
  while (getline(in, line)) {
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of("@"));

    if (tokens.size() == 10) {
      Tools::StateTrueLogRow row;
      string functionName = tokens[0];
      string numLineBeginBB = tokens[1];
      string numLineStart = tokens[2];
      string sourceCode = tokens[3];
      string controlCode = tokens[4];
      string hasControlCode = tokens[5];
      string numLineControlTrue = tokens[6];
      string numLineControlFalse = tokens[7];
      string isEntryPoint = tokens[8];
      string isErrorLabel = tokens[9];

      row.functionName = functionName;
      row.numLineBeginBB = numLineBeginBB;
      row.numLineStart = numLineStart;
      row.sourceCode = sourceCode;
      row.controlCode = controlCode;
      row.hasControlCode = hasControlCode;
      row.numLineControlTrue = numLineControlTrue;
      row.numLineControlFalse = numLineControlFalse;
      row.isEntryPoint = isEntryPoint;
      row.isErrorLabel = isErrorLabel;

      // Map2Check::Log::Debug(row );
      listLog.push_back(row);
    }
  }

  return listLog;
}

std::vector<Tools::TrackBBLogRow> Tools::TrackBBLogHelper::getListLogFromCSV(
    string path) {
  std::vector<Tools::TrackBBLogRow> listLog;
  Map2Check::Log::Debug("Started reading file: " + path);

  // using namespace boost;
  // using namespace std;

  // Open file as READ mode
  ifstream in(path.c_str());
  if (!in.is_open()) {
    return listLog;
  }

  string line;
  while (getline(in, line)) {
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of(";"));

    if (tokens.size() == 2) {
      Tools::TrackBBLogRow row;
      string numLineInBB = tokens[0];
      string functionName = tokens[1];

      row.numLineInBB = numLineInBB;
      row.functionName = functionName;

      // Map2Check::Log::Debug(row );
      listLog.push_back(row);
    }
  }

  return listLog;
}
