#include "counter_example.hpp"
#include "tools.h"
#include <memory>
#include <algorithm>
using namespace Map2Check;

CounterExample::CounterExample(std::string path) {
  this->sourceCodeHelper = std::make_unique<Tools::SourceCodeHelper>(Tools::SourceCodeHelper(path));

  this->processKleeLog();
    //Log::Debug("started reading list log");
  this->processListLog();
  this->processProperty();
}

void CounterExample::printCounterExample(bool printListLog) {
  // std::sort(this->counterExampleRows.begin(), this->counterExampleRows.end());
  Log::Info("Counter-example:\n");
  int currentState = 0;
  for(int i =0; i < this->counterExampleRows.size(); i++) {
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
    std::unique_ptr<CounterExampleRow> row = std::make_unique<CounterExampleProperty>(step, state, path, 0, violated.propertyViolated, violated.line, violated.function_name);
    this->counterExampleRows.push_back(std::move(row));
    this->property = violated.propertyViolated;
  
}

std::string CounterExample::getViolatedProperty() {
  if(this->property == Tools::PropertyViolated::FALSE_FREE) {
    return "FALSE-FREE";
  }

  if(this->property == Tools::PropertyViolated::TARGET_REACHED) {
    return "TARGET-REACHED";
  }

  if(this->property == Tools::PropertyViolated::FALSE_DEREF) {
    return "FALSE-DEREF";
  }

  if(this->property == Tools::PropertyViolated::FALSE_MEMTRACK) {
    return "FALSE-MEMTRACK";
  }

  if(this->property == Tools::PropertyViolated::NONE) {
    return "TRUE";
  }

  if(this->property == Tools::PropertyViolated::UNKNOWN) {
    return "UNKNOWN";
  }
  // FIXME: Throw exception
  return "";
}


// FIXME: Add Support to other non det type
void CounterExample::processKleeLog() {
  try{ 

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
      // Log::Info(*row);
      
      this->counterExampleRows.push_back(std::move(row));
      ref++;

    }
  }
  catch(Tools::CouldNotOpenFileException& e) {
    Log::Debug("Klee didnt generate any result");
  }

}

void CounterExample::processListLog() {

  std::vector<Tools::ListLogRow> listLogRows = Tools::ListLogHelper::getListLogFromCSV();
  //TODO: Add flag to map2check to print listlog
  int ref = 0;
  for(int i = 0; i < listLogRows.size(); i++) {
    int step = std::stoi(listLogRows[i].step);
    std::string path = this->sourceCodeHelper->getFilePath();
    int state = 0;
    int lineNumber =  std::stoi(listLogRows[i].lineNumber);
    Log::Debug("Line: " + listLogRows[i].lineNumber);
    std::string lineC = this->sourceCodeHelper->getLine(lineNumber);
     std::unique_ptr<CounterExampleRow> row = std::make_unique<CounterExampleListLogRow>(listLogRows[i], step, state, path, ref, lineC);
      this->counterExampleRows.push_back(std::move(row));
      ref++;

  }
}
