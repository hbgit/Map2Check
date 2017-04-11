#include "tools.h"
#include <iostream>
#include "log.h"
#include <vector>
#include <memory>
#include <sstream>

namespace Map2Check {

  struct CounterExampleRow {
    int lineNumber;
    std::string lineC;
    int step;

    operator std::string() const {
      std::ostringstream cnvt;
      cnvt.str("");
      cnvt << "LINE " << this->lineNumber << ": " << this->lineC;
      return cnvt.str();
    }

    bool operator < (const CounterExampleRow& row) const
    {
        return (this->step < row.step);
    }
  };

  class CounterExample {
  public:
    CounterExample(std::string path);
  private:
    std::vector<CounterExampleRow> counterExampleRows;
    Tools::SourceCodeHelper* sourceCodeHelper;
    void processKleeLog();
    void processListLog();
    void processProperty();

  };

}
