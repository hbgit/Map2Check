#include "tools.h"
#include <iostream>
#include "log.h"
#include <vector>
#include <memory>
#include <sstream>

#pragma once
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
    CounterExampleRow(int step, int state, std::string fileName, int ref) : step(step), state(state), fileName(fileName), ref(ref) {}
    void setState(int state) {
      this->state = state;
    }

    virtual int getState() {
      return this->state;
    }

    operator std::string() {
      return this->convertToString();
    }

    bool operator < (const CounterExampleRow& row) const
    {
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

        cnvt << "State " << this->state << " ";
        cnvt << "file " << this->fileName << " ";
        cnvt << "Line content " << this->lineC << "\n";

        cnvt << (std::string) this->row;

        return cnvt.str();

      }

    public:
      CounterExampleListLogRow(Tools::ListLogRow row, int step, int state, std::string fileName, int ref, std::string lineC) :
      lineC(lineC), row(row), CounterExampleRow(step,state,fileName,ref) {}
  }; 



  class CounterExampleKleeRow : public CounterExampleRow {
  protected:
    Tools::KleeLogRow row;
    std::string lineC;

    virtual std::string convertToString() {
      std::ostringstream cnvt;
      cnvt.str("");
      cnvt << "State " << this->state << " ";
      cnvt << "file " << this->fileName << " ";
      cnvt << "line " << this->row.line << " ";
      cnvt << "function " << this->row.functionName << " ";

      // TODO: Check thread
      cnvt << "thread 0\n";

      cnvt << "c::" << this->row.functionName << "\n";
      cnvt << "----------------------------------------------------\n";
      cnvt << "c::$(tmp)::value_nondet_int$" << this->row.id << "="<< this->row.value << " (" << this->ref << ")\n";

      cnvt << "\n";
      cnvt << "State " << this->state + 1 << " ";
      cnvt << "file " << this->fileName << " ";
      cnvt << "line " << this->row.line << " ";
      cnvt << "function " << this->row.functionName << " ";

      // TODO: Check thread
      cnvt << "thread 0\n";

      cnvt << "c::" << this->row.functionName << "\n";
      cnvt << "----------------------------------------------------\n";
      cnvt << "c::" << this->row.functionName << "::" << this->lineC << " (" << this->ref << ")\n";
      return cnvt.str();
    }
  public:
    CounterExampleKleeRow(Tools::KleeLogRow row, int step, int state, std::string fileName, int ref, std::string lineC) :
      lineC(lineC), row(row), CounterExampleRow(step,state,fileName,ref) {}

      virtual int getState() {
        return this->state + 1;
      }
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
      switch(this->propertyViolated) {
            case(Tools::PropertyViolated::FALSE_FREE):
              cnvt << "Violated property:\n";
              cnvt << "\tfile " << this->fileName << " ";
              cnvt << "line " << this->lineNumber << " ";
              cnvt << "function " << this->functionName << "\n";
              cnvt << "\tFALSE-FREE: Operand of free must have zero pointer offset\n\n";
              cnvt << "VERIFICATION FAILED";
              break;
            case(Tools::PropertyViolated::TARGET_REACHED):
              //TODO: Add message for target reached
              cnvt << "Violated property:\n";
              cnvt << "\tfile " << this->fileName << " ";
              cnvt << "line " << this->lineNumber << " ";
              cnvt << "function " << this->functionName << "\n";
              cnvt << "\tFALSE: Target Reached\n\n";
              cnvt << "VERIFICATION FAILED";
              break;
            case(Tools::PropertyViolated::FALSE_DEREF):
              //TODO: Add message for target reached
              cnvt << "Violated property:\n";
              cnvt << "\tfile " << this->fileName << " ";
              cnvt << "line " << this->lineNumber << " ";
              cnvt << "function " << this->functionName << "\n";
              cnvt << "\tFALSE-DEREF: Reference to pointer was lost\n\n";
              cnvt << "VERIFICATION FAILED";
              break;  
            case(Tools::PropertyViolated::FALSE_MEMTRACK):
              //TODO: Add message for target reached
              cnvt << "Violated property:\n";
              cnvt << "\tFALSE-MEMTRACK\n\n";
              cnvt << "VERIFICATION FAILED";
              break;   
            case(Tools::PropertyViolated::NONE):
              cnvt << "VERIFICATION SUCCEDED";
              break;
      }

      return cnvt.str();
    }

  public:
      CounterExampleProperty(int step, int state, std::string fileName, int ref, Tools::PropertyViolated property, int lineNumber, std::string functionName) :
        propertyViolated(property), lineNumber(lineNumber), functionName(functionName), CounterExampleRow(step,state,fileName,ref) {}
  };

  // class CounterExampleVarAttrRow : public CounterExampleRow {
  // protected:
  //   std::string attribuition;
  //
  // public:
  //   CounterExampleVarAttrRow(std::string attr, int step, int state, std::string fileName, int ref) :
  //     attribuition(attr), row(row), CounterExampleRow(step,state,fileName,ref) {}
  // };
  //

  class CounterExample {  
  public:
    CounterExample(std::string path);    
    std::string getViolatedProperty();
    Tools::PropertyViolated getProperty() {
        return this->property;
    }

    void printCounterExample(bool printListLog = false);
  private:
    Tools::PropertyViolated property;
    std::vector<std::unique_ptr<CounterExampleRow> > counterExampleRows;
    std::unique_ptr<Tools::SourceCodeHelper> sourceCodeHelper;
    void processKleeLog();
    void processListLog();
    void processProperty();

  };

}
