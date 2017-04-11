#include "counter_example.hpp"
#include "tools.h"
#include <memory>
#include <algorithm>
using namespace Map2Check;

CounterExample::CounterExample(std::string path) {
  this->sourceCodeHelper = new Tools::SourceCodeHelper(path);
  this->processKleeLog();
  this->processListLog();
  this->processProperty();

  std::sort(this->counterExampleRows.begin(), this->counterExampleRows.end());
  Log::Info("*** COUNTER EXAMPLE ***");
  for(int i =0; i < this->counterExampleRows.size(); i++) {
    Log::Info(this->counterExampleRows[i]);
  }

}

void CounterExample::processProperty() {
  Tools::CheckViolatedProperty violated;
  CounterExampleRow row;
  row.step = this->counterExampleRows.size();
  row.lineC = (std::string) violated;
  row.lineNumber = violated.line;
  this->counterExampleRows.push_back(row);
}

// TODO: Add Support to other non det type
void CounterExample::processKleeLog() {
  std::vector<Tools::KleeLogRow> kleeLogRows = Tools::KleeLogHelper::getListLogFromCSV();

  for(int i = 0; i < kleeLogRows.size(); i++) {
    CounterExampleRow row;
    row.lineNumber = std::stoi(kleeLogRows[i].line);
    row.step = std::stoi(kleeLogRows[i].step);
    row.lineC = this->sourceCodeHelper->substituteWithResult(row.lineNumber,
      "__VERIFIER_nondet_int()", kleeLogRows[i].value );

    this->counterExampleRows.push_back(row);
    // Log::Debug(row);
  }
}

void CounterExample::processListLog() {
  std::vector<Tools::ListLogRow> listLogRows = Tools::ListLogHelper::getListLogFromCSV();

  for(int i = 0; i < listLogRows.size(); i++) {
    CounterExampleRow row;
    row.lineNumber = std::stoi(listLogRows[i].lineNumber);
    row.step = std::stoi(listLogRows[i].step);
    row.lineC = listLogRows[i].counterExampleHelper();
    this->counterExampleRows.push_back(row);
    // Log::Debug(row);
  }
}
