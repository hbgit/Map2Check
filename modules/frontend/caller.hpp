/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_CALLER_HPP_
#define MODULES_FRONTEND_CALLER_HPP_

#include <string>
#include <vector>

#include <filesystem>

namespace Map2Check {

/** Map2Check verification modes */
// TODO(hbgit): Add support to custom mode
enum class Map2CheckMode {
  MEMTRACK_MODE,     /**< Check memory errors (memtrack, deref, free) */
  REACHABILITY_MODE, /**< Check if a target function can be executed */
  OVERFLOW_MODE,     /**< Check for signed integer overflows */
  ASSERT_MODE,       /**< Check for asserts (__VERIFIER_assert) */
  MEMCLEANUP_MODE,   /**< Check for memcleanup errors */
  /** Explore paths and record their inputs; check no property.
   *
   * Test-Comp Cover-Branches asks for a suite that exercises branches, not for
   * a verdict, so every property check is overhead here -- and worse than
   * overhead. With no mode flag this used to fall through to MEMTRACK_MODE and
   * run the memory-tracking pass: on the Test-Comp corpus that meant 110 of
   * 110 ProductLines tasks producing an empty suite in two seconds, because
   * that pass emits a broken module on them. The same tasks work under
   * Cover-Error, which never loads it. */
  COVER_BRANCHES_MODE
};

/** NonDet generators */
// TODO(hbgit): Add suport to other nondet like: klee, afl, afl+klee,
// LibFuzzer+afl
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
  void compileCFile(bool is_llvm_bc);

  /** Compiles the input through Clam so the emitted bitcode carries
   * verifier.assume(invariant) calls. Requires Clam dev16 installed; callers
   * must have checked availability first (main() refuses the run otherwise). */
  void compileWithClam();

  /** @brief Function to call pass for current verification mode
   *  (for REACHABILITY mode)
   *  @param target_function Function to be verified
   *  @param sv_comp boolean representing if should use sv-comp rules */
  int callPass(std::string target_function = "", bool sv_comp = false);

  std::string entryFunction = "main";
  bool wasmMode = false;

  /** Link functions called after executing the passes */
  void linkLLVM();

  /** Executes analysis with the generated LLVM IR */
  void executeAnalysis(std::string solvername);

  /** Remove generated files for verification */
  void cleanGarbage();

  /** Slice the program with respect to the target before analysing it.
   *
   * Off by default. Slicing changes WHAT IS ANALYSED, not merely how fast: a
   * slice taken with respect to one error site can legitimately remove
   * another, so a run with this on answers a narrower question than a run
   * without it. That is right for a competition task with one property and
   * wrong for a baseline scoring precision per CWE, which is why it is a
   * decision the caller makes rather than a default. */
  bool sliceProgram = false;

  /** Runs sbt-slicer over the instrumented bitcode. Returns false if the
   * slicer is unavailable or produced nothing usable, leaving the original
   * bitcode in place. */
  bool sliceWithRespectToTarget(const std::string& targetFunction);

  /** Turns on the exchange of input vectors between the two engines.
   *
   * Off by default so the hybrid keeps behaving exactly as it was measured
   * (symex 27%, fuzzer 32%, hybrid 45% over the same 372 tasks); promoting it
   * is a separate decision that has to be earned by its own measurement. */
  bool seedExchange = false;

  /** Directory the two engines use to hand each other input vectors.
   *
   * A directory of files rather than a value passed from one phase to the
   * next, and the shape is the point: it survives between phases, between
   * runs, and between alternations -- which is what time-slicing will need.
   * LibFuzzer treats it as its corpus and grows it; the KLEE phase drops its
   * own path vectors in.
   *
   * Relative, because both engines run with the scratch directory as their
   * working directory. */
  static constexpr const char* seedDirectory = "seeds";

  /** Writes KLEE's per-path vectors into the seed corpus.
   *
   * Sound only because the engines agree on widths now: concatenating a
   * .ktest's objects yields exactly the byte buffer that would drive the
   * fuzzer down the same path. Returns how many seeds were written. */
  unsigned exportKleeVectorsAsSeeds();

  /** Writes what the fuzzer consumed as a .ktest KLEE can start from.
   *
   * The nondet log is the only record of a fuzzer run carrying both value and
   * type, which is what a .ktest needs. Returns the path, or empty. */
  std::string exportFuzzerVectorAsKtest();

  /** Instrument and execute nondeterministic generator */
  void applyNonDetGenerator();

  /** Use btree mode */
  void useBTree() { this->dataStructure = DataStructure::BTree; }

  /** Generate wasm wrapper bitcode for KLEE compatibility */
  static std::string generateWasmWrapperStatic(const std::string& wasmOutHeaderPath,
                                               const std::string& entryPointName);

  bool isTimeout() { return gotTimeout; }
  bool isVerified() { return witnessVerified; }

  /** Directory map2check was invoked from. The pipeline chdirs into a scratch
   * directory that cleanGarbage() deletes, so anything meant to outlive the
   * run must be written here. */
  std::string getOriginalPath() { return currentPath; }

  /** Absolute path of this run's scratch directory, where every intermediate
   * artefact lives. Named after the SHA-1 of the input bitcode, so two runs on
   * the same input share it. */
  std::string getScratchDir() { return currentPath + "/" + programHash; }
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_CALLER_HPP_
