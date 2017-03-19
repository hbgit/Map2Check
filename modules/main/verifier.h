#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdexcept>
	
using namespace std;

namespace Map2Check::Tools {
  const string ktestBinary("./bin/ktest-tool");
}
  
class Witness {
 public:   
  Witness(string kleeResultFolder);
  enum class KleeStatus {
    OK,
    ERROR  
  };
  struct KleeResult {
    KleeStatus kleeStatus;
    string name;
  };  
 private:  
  void genKleeFilesListFromFolder(string kleeResultFolder);  
  static void convertKleeFileToTextFile(KleeResult kleeResult); 
  vector<KleeResult> kleeResults;
  
};

class WitnessException : public runtime_error {
 public:
 WitnessException(string message) : runtime_error(message) {}
  virtual const char* what() const throw();
};

class InvalidKleeFolderException : public WitnessException {
 public:
 InvalidKleeFolderException(string folder) : WitnessException(""), folder(folder) {}
  virtual const char* what() const throw();

 private:
  string folder;
};

class InvalidKtestBinException : public WitnessException {
   public:
 InvalidKtestBinException() : WitnessException("") {}
  virtual const char* what() const throw();  
};
