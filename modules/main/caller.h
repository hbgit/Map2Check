#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include "log.h"
#include "tools.h"
using namespace std;

class Caller {

 protected:
  std::string pathprogram;

 public:
  Caller( std::string bcprogram_path );
  static string compileCFile(std::string cprogram_path);
  void printdata();
  int parseIrFile();
  void genByteCodeFile();
  int callPass();
  int callPass(std::string target_function);
  void linkLLVM();
  void callKlee();
};

class CallerException : public runtime_error {
 public:
 CallerException(string message) : runtime_error(message) {}
  virtual const char* what() const throw();
};

class InvalidClangBinaryException : public CallerException {
 public:
 InvalidClangBinaryException() : CallerException("") {}
  virtual const char* what() const throw();
};

class InvalidClangIncludeException : public CallerException {
 public:
 InvalidClangIncludeException() : CallerException("") {}
  virtual const char* what() const throw();
};

class ErrorCompilingCProgramException : public CallerException {
 public:
 ErrorCompilingCProgramException() : CallerException("") {}
  virtual const char* what() const throw();
};
