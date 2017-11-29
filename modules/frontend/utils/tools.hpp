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
  const string ktestBinary("${MAP2CHECK_PATH}/bin/ktest-tool");
  /** Path to clang binary (from llvm) */
  const string clangBinary("${MAP2CHECK_PATH}/bin/clang");
  /** Path to clang include folder (usually $(PATH_TO_CLANG)/lib/clang/$(LLVM_VERSION)/include) */
  const string clangIncludeFolder("${MAP2CHECK_PATH}/include/clang");
  /** Path to generated list log file (check MemoryUtils implementation) */
  const string listLogCSV("list_log.csv");
  /** Path to klee binary */
  const string kleeBinary("${MAP2CHECK_PATH}/bin/klee");
  /** Path to generated klee log file (check MemoryUtils implementation) */
  const string kleeLogCSV("klee_log.csv");
  /** Path to generated Correctness log file (check MemoryUtils implementation) */
  const string stateTrueLogCSV("automata_list_log.st");
  /** Path to generated Correctness log file (check MemoryUtils implementation) */
  const string trackBBLogCSV("track_bb_log.st");  
  /** Path to generated map2check_property file (check MemoryUtils implementation) */
  const string propertyViolationFile("map2check_property");
  /** Path to generated klee results (it is created where klee is called) */
  const string kleeResultFolder("./klee-last");
  /** Path to opt binary (from llvm) */
  const string optBinary("${MAP2CHECK_PATH}/bin/opt");
    /** Path to llvm-link binary (from llvm) */
  const string llvmLinkBinary("${MAP2CHECK_PATH}/bin/llvm-link");

  /** Represents what kind of property was violated */
   enum class PropertyViolated { 
    FALSE_OVERFLOW,
    TARGET_REACHED,
    FALSE_FREE,
    FALSE_DEREF,
    FALSE_MEMTRACK,
    UNKNOWN,
    NONE    
  };

  /** Class used to check violated property */
  struct CheckViolatedProperty {
    /** Current violated property */
    PropertyViolated propertyViolated;
    /** Line where property was violated */
    unsigned line;
    /** Name of the function where the property was violated */
    string function_name;
    string path_name;
    /**
     * Reads file and initializes the object
     * @param path File describing the property
     */
    CheckViolatedProperty(std::string path);


    // operator std::string() const {
    //     std::ostringstream cnvt;
    //     cnvt.str("");
    //     // cnvt <<"Function: " << this->function_name;
    //     switch(this->propertyViolated) {
    //       case(PropertyViolated::FALSE_FREE):
    //         cnvt << "FALSE-FREE: Operand of free must have zero pointer offset";
    //         break;
    //       case(PropertyViolated::TARGET_REACHED):
    //         //TODO: Add message for target reached
    //         cnvt << "FALSE: Target Reached";
    //         break;
    //       case(PropertyViolated::NONE) {
    //         cnvt << "TRUE";
    //         break;
    //       }
    //     }
    //     return cnvt.str();
    // }

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
        for (unsigned i = 0; i < this->cFileLines.size(); i++) {
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

    std::string getFilePath();
    std::string substituteWithResult(int line, std::string old_token, std::string result);
    // void changeTokenFromLine(int line, std::string old_token, std::string new_token);
  private:
    std::string path;
    std::vector<std::string> cFileLines;
  };

  enum class KleeLogType {
    INTEGER,
    CHAR,    
    POINTER,
    USHORT,
    LONG,
    UNSIGNED
  };
    

  struct KleeLogRow {
    string id;
    KleeLogType type;
    
    string line;
    string scope;
    string functionName;
    string step;
    string value;

    std::string counterExampleHelper() {
      std::ostringstream cnvt;
      cnvt.str("");
      cnvt << "Line: " << line << " :: ";
      cnvt << "Function: " << functionName << " :: ";
      cnvt << "Value: " << value;
      return cnvt.str();
    }

    operator std::string() const {
        std::ostringstream cnvt;
        cnvt.str("");        
	switch(type) {
	case KleeLogType::INTEGER:
	  cnvt << "  Call Function  : " << "__VERIFIER_nondet_int()"<< "\n";
	  break;
	case KleeLogType::CHAR:
	  cnvt << "  Call Function  : " << "__VERIFIER_nondet_char()"<< "\n";
	  break;
	case KleeLogType::POINTER:
	  cnvt << "  Call Function  : " << "__VERIFIER_nondet_pointer()"<< "\n";
	  break;
	case KleeLogType::USHORT:
	  cnvt << "  Call Function  : " << "__VERIFIER_nondet_ushort()"<< "\n";
	  break;
	case KleeLogType::LONG:
	  cnvt << "  Call Function  : " << "__VERIFIER_nondet_long()"<< "\n";
	  break;
	case KleeLogType::UNSIGNED:
	  cnvt << "  Call Function  : " << "__VERIFIER_nondet_uint()"<< "\n";
	  break;
	}        
	cnvt << "  Value          : " << this->value << "\n";        
        cnvt << "  Line Number    : " << this->line << "\n";
        cnvt << "  Function Scope : " << this->functionName << "\n";

        return cnvt.str();
    }
  };

  /** Class used to get all KleeLogRow from a CSV file */
  class KleeLogHelper {
  public:
    /**
     * Reads a CSV file and returns a vector of KleeLogRow
     * @param path CSV file path
     * @return     vector of KleeLogRow
     */
    static vector<KleeLogRow> getListLogFromCSV(string path);
    /**
     * Reads a CSV file (from default path) and returns a vector of KleeLogRow
     * @return     vector of KleeLogRow
     */
    static vector<KleeLogRow> getListLogFromCSV() {
      return KleeLogHelper::getListLogFromCSV(kleeLogCSV);
    }
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
    string lineNumber;
    string functionName;
    string step;

    std::string counterExampleHelper() {
      std::ostringstream cnvt;
      cnvt.str("");
      cnvt << varName << " = " << pointsTo << "; ";
      // cnvt << "Function: " << functionName << " :: ";
      cnvt << "Is Free: " << isFree << " :: ";
      cnvt << "Is Dynamic: " << isDynamic;
      return cnvt.str();
    }

    operator std::string() const {
        std::ostringstream cnvt;
        cnvt.str("");        
        cnvt << "  Address        : " << this->memoryAddress << "\n";
        cnvt << "  PointsTo       : " << this->pointsTo << "\n";
        cnvt << "  Is Free        : " << (this->isFree == "1" ? "TRUE" : "FALSE") << "\n";
        cnvt << "  Is Dynamic     : " << (this->isDynamic == "1" ? "TRUE" : "FALSE") << "\n";
        cnvt << "  Var Name       : " << this->varName << "\n";
        cnvt << "  Line Number    : " << this->lineNumber << "\n";
        cnvt << "  Function Scope : " << this->functionName << "\n";
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
  
  /** Struct used to represent all rows from state true automata log CSV */
  struct StateTrueLogRow {    
    string functionName;
    string numLineBeginBB;
    string numLineStart;
    string sourceCode;
    string controlCode;
    string hasControlCode;
    string numLineControlTrue;
    string numLineControlFalse;
    string isEntryPoint;  
    string isErrorLabel;
  };
  
  /** Class used to get all StateTrueLogRow from a CSV file */
  class StateTrueLogHelper {
  public:
    /**
     * Reads a CSV file and returns a vector of StateTrueLogRow
     * @param path CSV file path
     * @return     vector of StateTrueLogRow
     */
    static vector<StateTrueLogRow> getListLogFromCSV(string path);
    /**
     * Reads a CSV file (from default path) and returns a vector of StateTrueLogRow
     * @return     vector of StateTrueLogRow
     */
    static vector<StateTrueLogRow> getListLogFromCSV() {
      return StateTrueLogHelper::getListLogFromCSV(stateTrueLogCSV);
    }
  };
  
  /** Struct used to represent all rows from track_bb_log.st for true automata log CSV */
  struct TrackBBLogRow {        
    string numLineInBB;
    string functionName;      
  };
  
  /** Class used to get all TrackBBLogRow from a CSV file */
  class TrackBBLogHelper {
  public:
    /**
     * Reads a CSV file and returns a vector of TrackBBLogRow
     * @param path CSV file path
     * @return     vector of TrackBBLogRow
     */
    static vector<TrackBBLogRow> getListLogFromCSV(string path);
    /**
     * Reads a CSV file (from default path) and returns a vector of TrackBBLogRow
     * @return     vector of TrackBBLogRow
     */
    static vector<TrackBBLogRow> getListLogFromCSV() {
      return TrackBBLogHelper::getListLogFromCSV(trackBBLogCSV);
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
