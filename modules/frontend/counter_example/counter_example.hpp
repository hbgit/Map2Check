/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_FRONTEND_COUNTER_EXAMPLE_COUNTER_EXAMPLE_HPP_
#define MODULES_FRONTEND_COUNTER_EXAMPLE_COUNTER_EXAMPLE_HPP_

#include <boost/make_unique.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "../utils/log.hpp"
#include "../utils/tools.hpp"

namespace Tools = Map2Check;
namespace Map2Check {

class CounterExampleRow {
 protected:
  int step;
  int state;
  int ref;
  virtual std::string convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "CounterExampleRow";
    return cnvt.str();
  }
  std::string fileName;

 public:
  CounterExampleRow(int step, int state, std::string fileName, int ref)
      : step(step), state(state), fileName(fileName), ref(ref) {}
  void setState(int state) { this->state = state; }

  virtual int getState() { return this->state; }

  operator std::string() { return this->convertToString(); }

  bool operator<(const CounterExampleRow &row) const {
    return (this->step >= row.step);
  }
};

class CounterExampleListLogRow : public CounterExampleRow {
 protected:
  Tools::ListLogRow row;
  std::string lineC;

  virtual std::string convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");

    cnvt << "State " << this->state << ": ";
    cnvt << "file " << this->fileName << "\n";
    cnvt << "------------------------------------------------------------\n";
    cnvt << ">>Memory list log\n\n";

    cnvt << "  Line content   : " << this->lineC << "\n";
    cnvt << (std::string)this->row;

    return cnvt.str();
  }

 public:
  CounterExampleListLogRow(Tools::ListLogRow row, int step, int state,
                           std::string fileName, int ref, std::string lineC)
      : lineC(lineC), row(row), CounterExampleRow(step, state, fileName, ref) {}
};

class CounterExampleKleeRow : public CounterExampleRow {
 protected:
  Tools::KleeLogRow row;
  std::string lineC;

  virtual std::string convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "State " << this->state << ": ";
    cnvt << "file " << this->fileName << "\n";
    cnvt << "------------------------------------------------------------\n";
    cnvt << ">Symbolic log\n";

    //      cnvt << "  Line content   : " << this->lineC << "\n";
    cnvt << (std::string)this->row;
    return cnvt.str();
  }

 public:
  CounterExampleKleeRow(Tools::KleeLogRow row, int step, int state,
                        std::string fileName, int ref, std::string lineC)
      : lineC(lineC), row(row), CounterExampleRow(step, state, fileName, ref) {}

  virtual int getState() { return this->state; }
};

class CounterExampleProperty : public CounterExampleRow {
 protected:
  int lineNumber;
  std::string functionName;
  Tools::PropertyViolated propertyViolated;

  virtual std::string convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "----------------------------------------------------\n";
    switch (this->propertyViolated) {
      case (Tools::PropertyViolated::FALSE_FREE):
        cnvt << "Violated property:\n";
        cnvt << "\tfile " << this->fileName << " ";
        cnvt << "line " << this->lineNumber << " ";
        cnvt << "function " << this->functionName << "\n";
        cnvt << "\tFALSE-FREE: Operand of free must have zero pointer "
                "offset\n\n";
        cnvt << "VERIFICATION FAILED";
        break;
      case (Tools::PropertyViolated::TARGET_REACHED):
        // TODO(hbgit): Add message for target reached
        cnvt << "Violated property:\n";
        cnvt << "\tfile " << this->fileName << " ";
        cnvt << "line " << this->lineNumber << " ";
        cnvt << "function " << this->functionName << "\n";
        cnvt << "\tFALSE: Target Reached\n\n";
        cnvt << "VERIFICATION FAILED";
        break;
      case (Tools::PropertyViolated::FALSE_DEREF):
        // TODO(hbgit): Add message for target reached
        cnvt << "Violated property:\n";
        cnvt << "\tfile " << this->fileName << " ";
        cnvt << "line " << this->lineNumber << " ";
        cnvt << "function " << this->functionName << "\n";
        cnvt << "\tFALSE-DEREF: Reference to pointer was lost\n\n";
        cnvt << "VERIFICATION FAILED";
        break;
      case (Tools::PropertyViolated::FALSE_OVERFLOW):
        // TODO(hbgit): Add message for target reached
        cnvt << "Violated property:\n";
        cnvt << "\tfile " << this->fileName << " ";
        cnvt << "line " << this->lineNumber << " ";
        cnvt << "function " << this->functionName << "\n";
        cnvt << "\tOVERFLOW\n\n";
        cnvt << "VERIFICATION FAILED";
        break;
      case (Tools::PropertyViolated::FALSE_MEMTRACK):
        // TODO(hbgit): Add message for target reached
        cnvt << "Violated property:\n";
        cnvt << "\tFALSE-MEMTRACK\n\n";
        cnvt << "VERIFICATION FAILED";
        break;
      case (Tools::PropertyViolated::FALSE_MEMCLEANUP):
        // TODO(hbgit): Add message for target reached
        cnvt << "Violated property:\n";
        cnvt << "\tFALSE-MEMCLEANUP\n\n";
        cnvt << "VERIFICATION FAILED";
        break;
      case (Tools::PropertyViolated::ASSERT):
        // TODO(hbgit): Add message for target reached
        cnvt << "Violated property:\n";
        cnvt << "\tfile " << this->fileName << " ";
        cnvt << "line " << this->lineNumber << " ";
        cnvt << "function " << this->functionName << "\n";
        cnvt << "\tFALSE-ASSERT\n\n";
        cnvt << "VERIFICATION FAILED";
        break;
      case (Tools::PropertyViolated::NONE):
        cnvt << "VERIFICATION SUCCEEDED";
        break;
    }

    return cnvt.str();
  }

 public:
  CounterExampleProperty(int step, int state, std::string fileName, int ref,
                         Tools::PropertyViolated property, int lineNumber,
                         std::string functionName)
      : propertyViolated(property),
        lineNumber(lineNumber),
        functionName(functionName),
        CounterExampleRow(step, state, fileName, ref) {}
};

class CounterExample {
 public:
  explicit CounterExample(std::string path, bool no_source = false);
  std::string getViolatedProperty();
  Tools::PropertyViolated getProperty() { return this->property; }
  Tools::PropertyViolated setProperty(Tools::PropertyViolated newProperty) {
    this->property = newProperty;
  }
  void generateTestCase();

  void printCounterExample(bool printListLog = false);

 private:
  Tools::PropertyViolated property;
  std::vector<std::unique_ptr<CounterExampleRow>> counterExampleRows;
  std::unique_ptr<Tools::SourceCodeHelper> sourceCodeHelper;
  std::vector<Tools::KleeLogRow> kleeLogRows;
  bool noSource;
  void processKleeLog();
  void processListLog();
  void processProperty();
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_COUNTER_EXAMPLE_COUNTER_EXAMPLE_HPP_
