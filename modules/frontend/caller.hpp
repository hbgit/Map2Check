#pragma once

#include <string>
#include <vector>

namespace Map2Check {

/** Map2Check verification modes */
// TODO: Add support to custom mode
enum class Map2CheckMode {
  MEMTRACK_MODE,     /**< Check memory errors (memtrack, deref, free) */
  REACHABILITY_MODE, /**< Check if a target function can be executed */
  OVERFLOW_MODE,     /**< Check for signed integer overflows */
  ASSERT_MODE        /**< Check for asserts (__VERIFIER_assert) */
};

/** NonDet generators */
// TODO: Add suport to other nondet like: klee, afl, afl+klee, LibFuzzer+afl
enum class NonDetGenerator {
  None,      /**< Do not generate any input */
  LibFuzzer, /**< LibFuzzer from LLVM */
  Klee,      /**< Use klee for symbolic analysis */
};

/** Data Structure */
enum class DataStructure { Array, BTree };

/** This class is responsible for calling all external and system programs */
class Caller {
 protected:
  std::string pathprogram;  //!< Path for the .bc program */
                            /** Get optimization flags for original C file
                             *  @return Flags for clang */
  static std::string preOptimizationFlags();
  /** Get optimization flags for final bytecode
   *  @return Flags for opt */
  static std::string postOptimizationFlags();
  /** Iterate over clang compilation messages (if any)
   *  and check for errors */
  std::vector<int> processClangOutput();
  Map2CheckMode map2checkMode;
  NonDetGenerator nonDetGenerator;
  DataStructure dataStructure = DataStructure::Array;
  std::string programHash;
  std::string currentPath;
  unsigned timeout;
  bool gotTimeout = false;
  bool witnessVerified = false;

 public:
  /** @brief Constructor if .bc file already exists
   *  @param bc_progam_path Path for the file */
  Caller(std::string bc_program_path, Map2CheckMode mode,
         NonDetGenerator generator);

  std::string c_program_fullpath;  //!< Path for the original c program */
  void setTimeout(unsigned timeout) { this->timeout = timeout; }
  /** @brief Function to compile original C file removing external memory
   * operations calls */
  void compileCFile();

  void compileToCrabLlvm();

  /** @brief Function to call pass for current verification mode
   *  (for REACHABILITY mode)
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

  /** Use btree mode */
  void useBTree() { this->dataStructure = DataStructure::BTree; }

  bool isTimeout() { return gotTimeout; }
  bool isVerified() { return witnessVerified; }
};

}  // namespace Map2Check
