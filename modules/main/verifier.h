#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdexcept>
	

using namespace std;
  
class Witness {
 public:   
  Witness(string kleeResultFolder);
 private: 
  void genKleeFilesListFromFolder(string kleeResultFolder);
  void convertKleeFilesToTextFiles();
  enum class KleeStatus {
    OK,
    ERROR  
  };
  struct KleeResult {
    KleeStatus kleeStatus;
    string name;
    string path;
  };
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
