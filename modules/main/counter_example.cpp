#include "counter_example.hpp"
#include "tools.h"
#include <memory>
#include <algorithm>
using namespace Map2Check;

CounterExample::CounterExample(std::string path) {
  this->sourceCodeHelper = std::make_unique<Tools::SourceCodeHelper>(Tools::SourceCodeHelper(path));
  int currentState = 0;
  this->processKleeLog();
  this->processListLog();
  this->processProperty();

  std::sort(this->counterExampleRows.begin(), this->counterExampleRows.end());
  Log::Info("Counter-example:\n");
  for(int i =0; i < this->counterExampleRows.size(); i++) {
    this->counterExampleRows[i]->setState(currentState);
    currentState = this->counterExampleRows[i]->getState() + 1;
    Log::Info(*this->counterExampleRows[i]);
  }

}


void CounterExample::processProperty() {
  Tools::CheckViolatedProperty violated;
  // CounterExampleRow row;
  // row.step = this->counterExampleRows.size();
  // row.lineC = (std::string) violated;
  // row.lineNumber = violated.line  ;
  // this->counterExampleRows.push_back(row);
}

// TODO: Add Support to other non det type
void CounterExample::processKleeLog() {
  std::vector<Tools::KleeLogRow> kleeLogRows = Tools::KleeLogHelper::getListLogFromCSV();
  int ref = 0;
  for(int i = 0; i < kleeLogRows.size(); i++) {
    int step = std::stoi(kleeLogRows[i].step);
    std::string path = this->sourceCodeHelper->getFilePath();

    int state = 0;

    int lineNumber =  std::stoi(kleeLogRows[i].line);
    std::string lineC = this->sourceCodeHelper->substituteWithResult(lineNumber,
      "__VERIFIER_nondet_int()", kleeLogRows[i].value );


    std::unique_ptr<CounterExampleRow> row = std::make_unique<CounterExampleKleeRow>(kleeLogRows[i], step, state, path, ref, lineC);
      // Log::Info(row);
    this->counterExampleRows.push_back(std::move(row));
    ref++;
  }
}

void CounterExample::processListLog() {
  std::vector<Tools::ListLogRow> listLogRows = Tools::ListLogHelper::getListLogFromCSV();

  // for(int i = 0; i < listLogRows.size(); i++) {
  //   CounterExampleRow row;
  //   row.lineNumber = std::stoi(listLogRows[i].lineNumber);
  //   row.step = std::stoi(listLogRows[i].step);
  //   row.lineC = listLogRows[i].counterExampleHelper();
  //   this->counterExampleRows.push_back(row);
  //   // Log::Debug(row);
  // }
}
