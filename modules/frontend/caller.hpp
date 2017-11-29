#ifndef MODULES_FRONTEND_CALLER_HPP_
#define MODULES_FRONTEND_CALLER_HPP_

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

/** Map2Check verification modes */
enum class Map2CheckMode {
  MEMTRACK_MODE, /**< Check memory errors (memtrack, deref, free) */
  REACHABILITY_MODE, /**< Check if a target function can be executed */
  OVERFLOW_MODE /**< Check for signed integer overflows */
};

/** Class used as inteface between the C program, LLVM and Klee */
class Caller {
 protected:
  std::string pathprogram;  //!< Path for the .bc program */
  /** Get optmization flags for original C file
   *  @return Flags for clang */
  static std::string preOptimizationFlags();
  /** Get optmization flags for final bytecode
   *  @return Flags for opt */
  static std::string postOptimizationFlags();
  /** Iterate over clang compilation messages (if any)
   *  and check for erors */  
  std::vector<int> processClangOutput();

 public:
  /** @brief Default constuctor */
  Caller() {}
  /** @brief Constructor if .bc file already exists
   *  @param bcprogam_path Path for the file */
  explicit Caller(std::string bcprogram_path);

  std::string cprogram_fullpath;  //!< Path for the oiginal c program */

  /** @brief Function to compile original C file removing external memory operations calls 
   *  @param cprogam_path Path for the file
   *  @return name of the generated .bc file */
  static std::string compileCFile(std::string cprogram_path);

  /** Parse the input LLVM IR file into a module */
  void parseIrFile();

  // TODO(rafa.sa.xp@gmail.com) Refacto genByteCodeFile for better name
  /** Wites a file (named output.bc) that contains the LLVM IR
   *  after pass execution  */
  void genByteCodeFile();

  /** @brief Function to call pass for current verification mode
   *  (REACHABLITY should use the overloaded method)
   *  @param mode Mode of the current execution
   *  @param sv_comp boolean representing if should use sv-comp rules */
  int callPass(Map2CheckMode mode, bool sv_comp = false);

  /** @brief Function to call pass for current verification mode
   *  (for REACHABLITY mode)
   *  @param mode Mode of the current execution
   *  @param target_function Function to be verified
   *  @param sv_comp boolean representing if should use sv-comp rules */
  int callPass(Map2CheckMode mode, std::string target_function,
               bool sv_comp = false);

  // TODO(rafa.sa.xp@gmail.com) Remove optimizations from this function
  /** Link functions called after executing the passes */
  void linkLLVM();

  /** Executes Klee with the generated LLVM IR */
  void callKlee();

  /** Remove generated files for verification */
  void cleanGarbage();
};

class CallerException : public std::runtime_error {
 public:
  explicit CallerException(std::string message) : runtime_error(message) {}
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
#endif //MODULES_FRONTEND_CALLER_HPP_
