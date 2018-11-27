#include "counter_example.hpp"

#include <algorithm>
#include <boost/make_unique.hpp>
namespace Tools = Map2Check;
using namespace Map2Check;

CounterExample::CounterExample(std::string path) {
  this->sourceCodeHelper = boost::make_unique<Tools::SourceCodeHelper>(
      Tools::SourceCodeHelper(path));

  this->processKleeLog();
  // Log::Debug("started reading list log");
  this->processListLog();
  this->processProperty();
}

void CounterExample::printCounterExample(bool printListLog) {
  // std::sort(this->counterExampleRows.begin(),
  // this->counterExampleRows.end());
  Log::Info("Counter-example:\n");
  int currentState = 0;
  for (int i = 0; i < this->counterExampleRows.size(); i++) {
    this->counterExampleRows[i]->setState(currentState);
    currentState = this->counterExampleRows[i]->getState() + 1;
    Log::Info(*this->counterExampleRows[i]);
  }
}

void CounterExample::processProperty() {
  Tools::CheckViolatedProperty violated;
  int step = this->counterExampleRows.size();
  int state = 0;
  std::string path = violated.path_name;
  std::unique_ptr<CounterExampleRow> row =
      boost::make_unique<CounterExampleProperty>(
          step, state, path, 0, violated.propertyViolated, violated.line,
          violated.function_name);
  this->counterExampleRows.push_back(std::move(row));
  this->property = violated.propertyViolated;
}

std::string CounterExample::getViolatedProperty() {
  if (this->property == Tools::PropertyViolated::FALSE_FREE) {
    return "FALSE-FREE";
  }

  if (this->property == Tools::PropertyViolated::TARGET_REACHED) {
    return "TARGET-REACHED";
  }

  if (this->property == Tools::PropertyViolated::FALSE_DEREF) {
    return "FALSE-DEREF";
  }

  if (this->property == Tools::PropertyViolated::FALSE_MEMTRACK) {
    return "FALSE-MEMTRACK";
  }

  if (this->property == Tools::PropertyViolated::FALSE_MEMCLEANUP) {
    return "FALSE-MEMCLEANUP";
  }

  if (this->property == Tools::PropertyViolated::NONE) {
    return "TRUE";
  }

  if (this->property == Tools::PropertyViolated::UNKNOWN) {
    return "UNKNOWN";
  }

  if (this->property == Tools::PropertyViolated::FALSE_OVERFLOW) {
    return "OVERFLOW";
  }
  // FIXME: Throw exception
  return "";
}

// FIXME: Add Support to other non det type
void CounterExample::processKleeLog() {
  kleeLogRows = Tools::KleeLogHelper::getListLogFromCSV();

  int ref = 0;
  for (int i = 0; i < kleeLogRows.size(); i++) {
    int step = std::stoi(kleeLogRows[i].step);
    std::string path = this->sourceCodeHelper->getFilePath();

    int state = 0;

    int lineNumber = std::stoi(kleeLogRows[i].line);
    std::string lineC = this->sourceCodeHelper->substituteWithResult(
        lineNumber, "__VERIFIER_nondet_int()", kleeLogRows[i].value);

    std::unique_ptr<CounterExampleRow> row =
        boost::make_unique<CounterExampleKleeRow>(kleeLogRows[i], step, state,
                                                  path, ref, lineC);
    // Log::Info(*row);

    this->counterExampleRows.push_back(std::move(row));
    ref++;
  }
}

void CounterExample::processListLog() {
  std::vector<Tools::ListLogRow> listLogRows =
      Tools::ListLogHelper::getListLogFromCSV();
  // TODO: Add flag to map2check to print listlog
  int ref = 0;
  for (int i = 0; i < listLogRows.size(); i++) {
    int step = std::stoi(listLogRows[i].step);
    std::string path = this->sourceCodeHelper->getFilePath();
    int state = 0;
    int lineNumber = std::stoi(listLogRows[i].lineNumber);
    Log::Debug("Line: " + listLogRows[i].lineNumber);
    std::string lineC = this->sourceCodeHelper->getLine(lineNumber);
    std::unique_ptr<CounterExampleRow> row =
        boost::make_unique<CounterExampleListLogRow>(listLogRows[i], step,
                                                     state, path, ref, lineC);
    this->counterExampleRows.push_back(std::move(row));
    ref++;
  }
}

#include <fstream>
#include <iostream>
void CounterExample::generateTestCase() {
  std::string file = this->sourceCodeHelper->getFilePath();
  std::string testcase = file + ".testcase";

  std::string command = "cp " + file + " " + testcase;

  system(command.c_str());
  std::ofstream sourceFile;
  sourceFile.open(testcase, ios::out | ios::app);

  sourceFile << "\n"
             << "unsigned map2check_counter = 0;";
  sourceFile << "\n"
             << "long map2check_input_array[] = {";

  for (int i = 0; i < (this->kleeLogRows.size()); i++) {
    sourceFile << this->kleeLogRows[i].value;
    if (i != (this->kleeLogRows.size() - 1)) {
      sourceFile << ",";
    }
  }

  sourceFile << "};\n";

  sourceFile << "unsigned __VERIFIER_nondet_uint() { return "
                "(unsigned)map2check_input_array[map2check_counter++]; }\n";
  sourceFile << "int __VERIFIER_nondet_int() { return "
                "(int)map2check_input_array[map2check_counter++]; }\n";
  sourceFile << "char __VERIFIER_nondet_char() { return "
                "(char)map2check_input_array[map2check_counter++]; }\n";
  sourceFile << "void* __VERIFIER_nondet_void() { return "
                "(void*)(long)map2check_input_array[map2check_counter++]; }\n";
  sourceFile << "long __VERIFIER_nondet_long() { return "
                "(long)map2check_input_array[map2check_counter++]; }\n";

  sourceFile.close();
}
