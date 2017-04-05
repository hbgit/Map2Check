#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

/**
 * Namespace for all Map2Check Paths and Helpers
 */
namespace Map2Check::Tools {
  const string ktestBinary("./bin/ktest-tool"); /** Path to ktest-tool binary (from KLEE) */
  const string clangBinary("./bin/clang"); /** Path to clang binary (from llvm) */
  const string clangIncludeFolder("./include/clang"); /** Path to clang include folder (usually $(PATH_TO_CLANG)/lib/clang/$(LLVM_VERSION)/include) */
  const string listLogCSV("list_log.csv"); /** Path to generated list log file (check MemoryUtils implementation) */

  /** Struct used to represend all rows from list log CSV */
  struct ListLogRow {
    string id;
    string memoryAddress;
    string pointsTo;
    string scope;
    string isFree;
    string isDynamic;
    string varName;
    string functionName;

    operator std::string() const {
        std::ostringstream cnvt;
        cnvt.str("");
        cnvt << "\nLIST LOG ROW\n";
        cnvt << "\tID: " << this->id << "\n";
        cnvt << "\tAddress: " << this->memoryAddress << "\n";
        cnvt << "\tPointsTo: " << this->pointsTo << "\n";
        cnvt << "\tIs Free: " << this->isFree << "\n";
        cnvt << "\tIs Dynamic: " << this->isDynamic << "\n";
        cnvt << "\tVar Name: " << this->varName << "\n";
        cnvt << "\tFunction Name: " << this->functionName << "\n";
        return cnvt.str();
    }
  };

  /** Class used to get all ListLogRow from a CSV file */
  class ListLogHelper {
  public:
    /**
     * Reads a CSV file and returns a vector of ListLogRow
     * @param path CSV file path
     * @return     vector of ListLogRow
     */
    static vector<ListLogRow> getListLogFromCSV(string path);
    /**
     * Reads a CSV file (from default path) and returns a vector of ListLogRow
     * @return     vector of ListLogRow
     */
    static vector<ListLogRow> getListLogFromCSV() {
      return ListLogHelper::getListLogFromCSV(listLogCSV);
    }
  };


  class CSVHelperException : public runtime_error {
    public:
      CSVHelperException(string message) : runtime_error(message) {}
      virtual const char* what() const throw();
  };

  class CouldNotOpenFileException : public CSVHelperException {
    public:
      CouldNotOpenFileException() : CSVHelperException("") {}
      virtual const char* what() const throw();
  };

  class InvalidCSVException : public CSVHelperException {
    public:
      InvalidCSVException() : CSVHelperException("") {}
      virtual const char* what() const throw();
  };

}
