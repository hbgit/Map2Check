/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_CALLER_HPP_
#define MODULES_FRONTEND_CALLER_HPP_

#include <cstddef>
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

/** Mode of a run given no mode flag (map2check.cpp). */
inline constexpr Map2CheckMode kDefaultMode = Map2CheckMode::MEMTRACK_MODE;

/** Value of opt's -m2c-analysis-mode for `mode`, i.e. the AnalysisMode of
 *  map2check-library that Map2CheckLibrary passes to map2check_set_mode:
 *  none=0, memtrack=1, memcleanup=2, overflow=3, assert=4, target=5.
 *  Cover-branches checks no property; reachability is Target. */
inline const char* runtimeAnalysisMode(Map2CheckMode mode) {
  switch (mode) {
    case Map2CheckMode::MEMTRACK_MODE:
      return "memtrack";
    case Map2CheckMode::MEMCLEANUP_MODE:
      return "memcleanup";
    case Map2CheckMode::OVERFLOW_MODE:
      return "overflow";
    case Map2CheckMode::ASSERT_MODE:
      return "assert";
    case Map2CheckMode::REACHABILITY_MODE:
      return "target";
    case Map2CheckMode::COVER_BRANCHES_MODE:
      return "none";
  }
  return "none";
}

/** NonDet generators */
// TODO(hbgit): Add suport to other nondet like: klee, afl++, afl+klee
enum class NonDetGenerator {
  None,       /**< Do not generate any input */
  AFLPlusPlus, /**< AFL++ through its libFuzzer driver (D1 route B1) */
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
  /** What decides how the instrumented program is linked with the runtime
   * (map2check-library, decision D1). Consumed by the pure command builders
   * below, so the link can be tested without running anything. */
  struct LinkInputs {
    std::string hash;               //!< scratch prefix, <hash>-output.bc
    Map2CheckMode mode;             //!< property mode
    NonDetGenerator generator;      //!< None/AFLPlusPlus: native, Klee: bitcode
    DataStructure dataStructure;    //!< ignored by the Rust runtime (--btree)
    bool wasm;                      //!< wasm2c-lifted input
    bool witness;                   //!< witness binary/bitcode (Step 4b)
    bool cmplog = false;            //!< AFL++ CmpLog companion binary
  };

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

  /** Directory the two engines use to hand each other input vectors.
   *
   * A directory of files rather than a value passed from one phase to the
   * next, meant to survive between phases, runs and alternations -- which is
   * what time-slicing will need. AFL++ starts from it and its discoveries are
   * copied back in after each fuzzer phase (afl-fuzz never writes into its -i
   * dir); the KLEE phase drops its own path vectors in.
   *
   * NOT yet true across phases (same as v15): it sits inside the scratch
   * directory, which each phase's Caller recreates empty. Fixing that is part
   * of the smart-seeds work (tacasv2/v3), since it changes what the hybrid
   * measures.
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

  /** Use btree mode. Ignored by the Rust runtime (kBtreeIgnoredWarning). */
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
};

// ---------------------------------------------------------------------------
// Linking with map2check-library (caller_link.cpp). Pure: they build strings
// or touch only the directory they are given, so CallerLinkTest covers them.
// ---------------------------------------------------------------------------

/** Installed native runtime, linked into every AFL++ binary (route B). */
inline constexpr const char* kRustRuntimeLibrary =
    "${MAP2CHECK_PATH}/lib/libmap2check.a";
/** Installed KLEE runtime bitcode (route A'), optional in CMake. */
inline constexpr const char* kRustKleeRuntime =
    "${MAP2CHECK_PATH}/lib/map2check-klee.bc";
/** System libraries the Rust standard library needs. */
inline constexpr const char* kRustRuntimeSystemLibs = "-ldl -lpthread -lm";
/** Minimum size of the AFL++ placeholder seed (D1 condition 4): the widest
 * value a nondet generator reads (long, double, pointers). */
inline constexpr std::size_t kAflSeedMinBytes = 8;
/** Logged once when --btree is given. Provisional until decision D3. */
inline constexpr const char* kBtreeIgnoredWarning =
    "--btree is ignored by the Rust runtime";

/** <hash>-result.bc, or <hash>-witness-result.bc for the witness variant. */
std::string resultBitcode(const Caller::LinkInputs& in);

/** llvm-link of the instrumented module into resultBitcode(in). Native
 * generators link nothing else (the runtime enters at the final link);
 * Klee links map2check-klee.bc and, for the witness, the witness bitcode. */
std::string buildLinkCommand(const Caller::LinkInputs& in);

/** afl-clang-fast -fsanitize=fuzzer over resultBitcode(in), the native
 * witness object (witness only) and libmap2check.a (D1 route B1). CmpLog is
 * prefixed with `env AFL_LLVM_CMPLOG=1`, so the command can follow `timeout`. */
std::string buildFuzzerCommand(const Caller::LinkInputs& in);

/** Name of the binary buildFuzzerCommand(in) writes. */
std::string fuzzerBinary(const Caller::LinkInputs& in);

/** C source of the witness constructor (Step 4b). */
std::string witnessConstructorSource();

/** <hash>-witness.c, the file witnessConstructorSource() is written to. */
std::string witnessSourcePath(const Caller::LinkInputs& in);

/** <hash>-witness.bc for Klee, <hash>-witness.o otherwise. */
std::string witnessObjectPath(const Caller::LinkInputs& in);

/** clang -c (-emit-llvm for Klee) -fexceptions of the witness constructor. */
std::string buildWitnessObjectCommand(const Caller::LinkInputs& in);

/** Replay of one AFL++ finding on the witness binary, the file passed as an
 * ARGUMENT (D1 condition 2: the libFuzzer driver never ends reading stdin). */
std::string buildReplayCommand(const std::string& hash,
                               const std::string& input);

/** Makes `dir` usable as afl-fuzz -i: creates it and, only when it is empty,
 * writes one placeholder seed of at least kAflSeedMinBytes bytes. A directory
 * that already holds seeds (the KLEE exchange) is left untouched. Returns
 * false if the directory or the seed could not be written. */
bool prepareAflInputDir(const std::string& dir);

/** kRustKleeRuntime with MAP2CHECK_PATH expanded from the environment. */
std::string installedKleeRuntime();

/** Whether installedKleeRuntime() exists. */
bool kleeRuntimeInstalled();

/** Logs kBtreeIgnoredWarning. */
void warnBtreeIgnored();

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_CALLER_HPP_
