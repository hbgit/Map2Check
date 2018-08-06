#pragma once

#include <string>
#include <vector>

namespace Map2Check {

/** Map2Check verification modes */
// TODO: Add support to custom mode
enum class Map2CheckMode {
  MEMTRACK_MODE,     /**< Check memory errors (memtrack, deref, free) */
  REACHABILITY_MODE, /**< Check if a target function can be executed */
  OVERFLOW_MODE      /**< Check for signed integer overflows */
};

/** NonDet generators */
// TODO: Add suport to other nondet like: klee, afl, afl+klee, libfuzzer+afl
enum class NonDetGenerator {
  None,     /**< Do not generate any input */
  LibFuzzer /**< LibFuzzer from LLVM */
};

/** This class is responsible for calling all external and system programs */
class Caller {
protected:
  std::string pathprogram; //!< Path for the .bc program */
                           /** Get optmization flags for original C file
                            *  @return Flags for clang */
  static std::string preOptimizationFlags();
  /** Get optmization flags for final bytecode
   *  @return Flags for opt */
  static std::string postOptimizationFlags();
  /** Iterate over clang compilation messages (if any)
   *  and check for erors */
  std::vector<int> processClangOutput();
  Map2CheckMode map2checkMode;
  NonDetGenerator nonDetGenerator;
  std::string programHash;
  unsigned timeout;

public:
  /** @brief Constructor if .bc file already exists
   *  @param bcprogam_path Path for the file */
  Caller(std::string bcprogram_path, Map2CheckMode mode,
         NonDetGenerator generator);

  std::string cprogram_fullpath; //!< Path for the oiginal c program */
  void setTimeout(unsigned timeout) { this->timeout = timeout; }
  /** @brief Function to compile original C file removing external memory
   * operations calls */
  void compileCFile();

  /** @brief Function to call pass for current verification mode
   *  (for REACHABLITY mode)
   *  @param mode Mode of the current execution
   *  @param target_function Function to be verified
   *  @param sv_comp boolean representing if should use sv-comp rules */
  int callPass(std::string target_function = "", bool sv_comp = false);

  /** Link functions called after executing the passes */
  void linkLLVM();

  /** Executes analysis with the generated LLVM IR */
  void executeAnalysis();

  /** Remove generated files for verification */
  void cleanGarbage();

  /** Instrument and execute nondeterministic generator */
  void applyNonDetGenerator();
};

} // namespace Map2Check
