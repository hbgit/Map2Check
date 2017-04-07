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
  /** Path to ktest-tool binary (from KLEE) */
  const string ktestBinary("./bin/ktest-tool");
  /** Path to clang binary (from llvm) */
  const string clangBinary("./bin/clang");
  /** Path to clang include folder (usually $(PATH_TO_CLANG)/lib/clang/$(LLVM_VERSION)/include) */
  const string clangIncludeFolder("./include/clang");
  /** Path to generated list log file (check MemoryUtils implementation) */
  const string listLogCSV("list_log.csv");
  /** Path to generated map2check_property file (check MemoryUtils implementation) */
  const string propertyViolationFile("map2check_property");
  /** Path to generated klee results (it is created where klee is called) */
  const string kleeResultFolder("./klee-last");

  /** Represents what kind of property was violated */
  enum class PropertyViolated {
    TARGET_REACHED,
    FALSE_FREE
  };

  /** Class used to check violated property */
  struct CheckViolatedProperty {
    /** Current violated property */
    PropertyViolated propertyViolated;
    /** Line where property was violated */
    unsigned line;
    /** Name of the function where the property was violated */
    string function_name;

    /**
     * Reads file and initializes the object
     * @param path File describing the property
     */
    CheckViolatedProperty(std::string path);

    /**
     * Reads default file and initializes the object
     */
    CheckViolatedProperty() : CheckViolatedProperty(propertyViolationFile) {}
  };

  /** Helper class to manipulate and transform code based on a C source file */
  class SourceCodeHelper {
  public:
    /**
     * Reads all lines from a C source file and adds to a vector structure
     * @param  c_src Path to C file
     */
    SourceCodeHelper(std::string pathToCSource);

    operator std::string() const {
        std::ostringstream cnvt;
        cnvt.str("");
        cnvt << "\n**** C SOURCE ****\n";
        for (auto i = 0; i < this->cFileLines.size(); i++) {
            cnvt << "\tLine " << i << ": " << this->cFileLines[i] << "\n";
        }
        return cnvt.str();
    }

    /**
     * Return line from c_file
     * @param  line line to be given
     * @return      Returns a string representing the line of the C file
     */
    std::string getLine(unsigned line);


    std::string substituteWithResult(int line, std::string old_token, std::string result);
    void changeTokenFromLine(int line, std::string old_token, std::string new_token);
  private:
    std::vector<std::string> cFileLines;
  };



  /** Represents the result from a Klee file */
  enum class KleeStatus {
    OK,
    ERROR
  };

  /** Helper class, used to read and process all klee results */
  // class KleeResultHelper;

  /** Represents the structure of a Klee file */
  struct KleeResult {
    friend class KleeResultHelper;
    /** Status of the result */
    KleeStatus kleeStatus;
    /** Generated values*/
    vector<int> states;
  private:
    /** Name of case test */
    string name;
  };

  class KleeResultHelper {
  public:
    /**
     * Reads all klee files from folder and generate a vector containing all KleeResults
     * @param path Path to klee results folder
     */
    static vector<KleeResult> GetKleeResults(std::string path);
    /**
     * Reads all klee files from default folder and generate a vector containing all KleeResults
     */
    static vector<KleeResult> GetKleeResults();
  private:
    KleeResultHelper() {}

    /**
     * Initialize all klee results with the Status and the Name of the Test
     * @param kleeResultFolder Path to klee results folder
     */
     void genKleeResultsFromFolder(string kleeResultFolder);
     /**
      * For the current result, convert the klee result from binary to text
      * and anotates the values and stores in kleeResult
      * @param kleeResult Pointer to kleeResult to be managed
      */
     static void convertKleeFileToTextFileAndGetValues(KleeResult* kleeResult);
     /** Vector structure containing the kleeResults */
     vector<KleeResult> kleeResults;
 };


  /** Struct used to represent all rows from list log CSV */
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


  class CheckViolatedPropertyException : public runtime_error {
    public:
      CheckViolatedPropertyException(string message) : runtime_error(message) {}
      virtual const char* what() const throw();
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
