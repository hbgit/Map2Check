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
// TODO(hbgit): Add suport to other nondet like: klee, afl++, afl+klee
enum class NonDetGenerator {
  None,       /**< Do not generate any input */
  AFLPlusPlus, /**< AFL++ (persistent mode, PCGUARD) */
  Klee,       /**< Use klee for symbolic analysis */
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

  /** Seconds of the run's budget that have not been spent yet.
   *
   * The engines used to size themselves from the NOMINAL budget: AFL++
   * took 0.2x and KLEE 0.8x, which adds to exactly the whole of it and leaves
   * nothing for the two compile-instrument-link passes between them. Under the
   * hybrid default the Caller is rebuilt per phase, so that overhead is paid
   * twice. Measured on the v12 Test-Comp corpus: every ERROR verdict -- 20 of
   * them, concentrated in ECA and Recursive -- landed at 87 to 89 seconds
   * against a 60 second budget and a 90 second outer timeout. The tool was not
   * failing; it was being killed mid-sentence and printing no verdict at all,
   * which every harness here reads as a crash.
   *
   * Sizing each phase against what is LEFT keeps the sum inside the budget
   * however many phases there turn out to be. */
  unsigned remainingSeconds() const;
  /** remainingSeconds for a budget of `timeout`, without a Caller: what main
   * sizes the alternating phases with. */
  static unsigned remainingOf(unsigned timeout);
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
  /** Back to the directory map2check was started in, without deleting the
   * scratch directory: under --debug the scratch is kept, but the next hybrid
   * phase must still start from the same place (the seed store is computed
   * from it). */
  void restoreWorkingDirectory();

  /** Slice the program with respect to the target before analysing it.
   *
   * Off by default. Slicing changes WHAT IS ANALYSED, not merely how fast: a
   * slice taken with respect to one error site can legitimately remove
   * another, so a run with this on answers a narrower question than a run
   * without it. That is right for a competition task with one property and
   * wrong for a baseline scoring precision per CWE, which is why it is a
   * decision the caller makes rather than a default. */
  bool sliceProgram = false;

  /** Runs sbt-slicer over the compiled (not yet instrumented) bitcode.
   *
   * `criteria` are the primary slicing criteria (the target function, or the
   * assert functions); every __VERIFIER_nondet_* function is added to them so
   * the suite found on the slice stays valid on the original program. The
   * cutoff of diverging paths is off: its exit(0) carries no debug location
   * and KLEE rejects the module. `targetFunction` gets its body back through a
   * weak stub. Returns false if the slicer is unavailable or produced nothing
   * usable, leaving the original bitcode in place. */
  bool sliceWithRespectToTarget(const std::string& targetFunction,
                                const std::vector<std::string>& criteria);

  /** Slices the INSTRUMENTED module (<hash>-output.bc) in place, for the
   * memory properties: every map2check_* runtime call and every nondet read
   * is a criterion, and the entry is __map2check_main__. Runs after callPass
   * and before linkLLVM. Returns false, leaving the module untouched, if the
   * slicer is unavailable or produced nothing usable. */
  bool sliceInstrumented();

  /** Turns on the exchange of input vectors between the two engines.
   *
   * Off by default so the hybrid keeps behaving exactly as it was measured
   * (symex 27%, fuzzer 32%, hybrid 45% over the same 372 tasks); promoting it
   * is a separate decision that has to be earned by its own measurement. */
  bool seedExchange = false;

  /** The seed store, beside the scratch directory: <cwd>/<hash>.seeds with
   * afl/ (fuzzer inputs), ktest/ (KLEE seeds) and replay/ (where queue entries
   * are replayed). Beside, not inside: every hybrid phase recreates the scratch
   * directory, and a store inside it never reached the next phase. Used only
   * under --seed-exchange. */
  std::string seedStore;
  /** Set by main on the hybrid's first phase: the fuzzer corpus is converted
   * into KLEE seeds only when a KLEE phase follows. After the last phase the
   * replays would be pure cost against a spent budget. */
  bool feedsKleePhase = false;
  const std::string& seedStorePath() const { return seedStore; }

  /** Slices kept across the phases of one run (<cwd>/<hash>.slice); see
   * Map2Check::sliceCachePath. Removed by main with the seed store. */
  std::string sliceCache;
  const std::string& sliceCachePath() const { return sliceCache; }

  /** Writes KLEE's per-path vectors into the seed corpus.
   *
   * Sound only because the engines agree on widths now: concatenating a
   * .ktest's objects yields exactly the byte buffer that would drive the
   * fuzzer down the same path. Returns how many seeds were written. */
  unsigned exportKleeVectorsAsSeeds();

  /** Set by main under --alternate-engines (tacas 3b): the most this phase's
   * engine may run, in seconds, instead of its fixed share of the budget (0:
   * the fixed shares). */
  double engineWindow = 0;
  /** Seconds without new coverage after which the engine is stopped (0: never).
   * AFL++ gets it as AFL_EXIT_ON_TIME; KLEE is watched through run.stats. */
  unsigned stagnationLimit = 0;
  /** Whether this phase's KLEE was stopped for stagnating: incomplete. */
  bool stoppedOnStagnation = false;

  /** At most this many fuzzer queue entries are converted into KLEE seeds. */
  static constexpr size_t kMaxSeedsFromFuzzer = 64;

  /** Converts the AFL++ queue into typed .ktest seeds for the KLEE phase, by
   * replaying each entry through the witness binary inside the seed store's
   * replay/ directory. Returns how many seeds were written. */
  unsigned exportFuzzerCorpusAsKtests();

  
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
 private:
  /** Shared by both slicing entry points: disassembles `input`, adds the
   * program's nondet names (and, with addRuntimeNames, its map2check_* calls)
   * to `primary`, runs sbt-slicer bounded, and logs the slice under `label`.
   * Returns false, with a warning, when there is no usable output. */
  bool runSlicer(const std::string& input, const std::string& output,
                 std::vector<std::string> primary, bool addRuntimeNames,
                 const std::string& entry, const std::string& label);
  /** runSlicer without the slice cache: what actually calls sbt-slicer. */
  bool runSlicerUncached(const std::string& input, const std::string& output,
                         std::vector<std::string> primary,
                         bool addRuntimeNames, const std::string& entry,
                         const std::string& label);
  /** Applies MAP2CHECK_SLICE_CLEANUP to a fresh slice, in place; on failure
   * the slice is kept as the slicer wrote it. */
  void cleanUpSlice(const std::string& slice);
  /** Runs the KLEE command, stopping it (SIGINT) once it stagnates for
   * stagnationLimit seconds; plain system() when the limit is 0 or the build
   * has no SQLite to read KLEE's stats with. Returns system()'s status. */
  int runKleeWatched(const std::string& command);
  /** Keeps KLEE's latest tests (at most kMaxSeedsFromFuzzer) in the seed
   * store's kleeprev/, to seed its next turn. */
  void keepKleeTestsAsSeeds();
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_CALLER_HPP_
