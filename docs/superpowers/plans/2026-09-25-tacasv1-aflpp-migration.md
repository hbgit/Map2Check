# tacasv1 — LibFuzzer → AFL++ Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace LibFuzzer with AFL++ 4.40c (persistent mode, PCGUARD) as Map2Check's fuzzing engine, with a full rename, preserving the hybrid loop and smart seeding byte-for-byte.

**Architecture:** The fuzzer is compiled at run time inside `Caller` from the already-linked `<hash>-result.bc` (mirroring the current `clang -fsanitize=fuzzer` flow). The `NonDetGeneratorLibFuzzy.c` driver is rewritten as `NonDetGeneratorAFL.c` using AFL++'s persistent-mode trampoline (`__AFL_FUZZ_INIT` + `__AFL_LOOP`); `caller.cpp` swaps the two compile commands to `afl-clang-fast` and the exec command to `afl-fuzz`. The coordinator stays inside `Caller` C++.

**Tech Stack:** C++17, LLVM 16, AFL++ 4.40c (`AFL_LLVM_INSTRUMENT=PCGUARD`), KLEE 3.1, CMake/Ninja, Ubuntu 22.04 Docker.

## Global Constraints

- LLVM 16 toolchain only (`clang-16`, `llvm-16`, `llvm-config-16` via apt.llvm.org).
- AFL++ version **4.40c** (git tag `v4.40c`); instrumentation mode **PCGUARD** (`AFL_LLVM_INSTRUMENT=PCGUARD`).
- **Full rename**: no `LibFuzzer` / `libFuzzer` / `LibFuzzy` identifiers remain in the production path; the CLI value `fuzzer` becomes `afl`; `SKIP_LIB_FUZZER` becomes `SKIP_AFL_PLUS_PLUS`. No transitional aliases.
- Do NOT change smart seeding (`--seed-exchange`, single pass, one fuzzer→KLEE vector), slicing, or KLEE budget logic.
- The hybrid loop in `main()` (fuzzer → KLEE → optional seed-exchange) stays structurally unchanged.
- AFL++ binaries resolve through `Map2Check::aflClangFastBinary()` / `Map2Check::aflFuzzBinary()` (env-overridable, default `/usr/local/bin`).
- Preserve the nondet width contract: `get_bytes_from_afl` consumes `sizeof(type)` bytes, matching `NonDetGeneratorKlee.c`.
- Commits use the repo's conventional style: `feat(tacasv1): ...`, `fix(...)`, `chore(...)`, `docs(...)`.

---

### Task 1: Install AFL++ 4.40c in Dockerfile.dev

**Files:**
- Modify: `Dockerfile.dev:129-132`

**Interfaces:**
- Produces: `/usr/local/bin/afl-clang-fast` (symlink to `afl-cc`), `/usr/local/bin/afl-fuzz`, `/usr/local/bin/afl-showmap`, runtime at `/usr/local/lib/afl`; env `AFL_PATH`, `AFL_LLVM_INSTRUMENT=PCGUARD`, and headless `AFL_*` flags. Later tasks (Task 2, Task 7) depend on these.

- [ ] **Step 1: Replace the LibFuzzer note (section 7) with the AFL++ build**

The current section 7 is three lines (a comment saying "No extra install needed"). Replace it with:

```dockerfile
# ============================================================
# 7. AFL++ 4.40c (LLVM 16, PCGUARD)
# ============================================================
# Tag-pinned like KLEE above: AFL++ has versioned releases, so -b v4.40c is
# the reproducible pin (the SHA pins in 7b/7c are for projects with none).
# PCGUARD needs no custom LLVM pass: afl-clang-fast adds clang-16's own
# -fsanitize-coverage=trace-pc-guard and the bundled runtime supplies the
# callbacks, so the build is lighter and more robust than classic/LTO modes.
RUN git clone --depth 1 -b v4.40c https://github.com/AFLplusplus/AFLplusplus.git /tmp/afl++ && \
    cd /tmp/afl++ && \
    make -j"$(nproc)" && \
    make install && \
    rm -rf /tmp/afl++

ENV PATH="/usr/local/bin:${PATH}"
# afl-cc locates its runtime relative to its own install; AFL_PATH is a safety
# net for the non-LLVM modes.
ENV AFL_PATH=/usr/local/lib/afl
# Headless, container-safe defaults: afl-fuzz aborts under CI/containers on the
# UI, CPU-affinity, cpufreq-governor and core-pattern checks. PCGUARD is the
# instrumentation mode afl-clang-fast must use everywhere.
ENV AFL_NO_UI=1 \
    AFL_NO_AFFINITY=1 \
    AFL_SKIP_CPUFREQ=1 \
    AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 \
    AFL_LLVM_INSTRUMENT=PCGUARD

# Fail the image build if AFL++ cannot actually instrument — the same failure
# mode section 7c guards against for sbt-slicer. afl-showmap exits non-zero on
# an uninstrumented binary, and the map is empty, so both checks must pass.
RUN printf 'int main(void){return 0;}\n' > /tmp/aflcheck.c && \
    /usr/local/bin/afl-clang-fast -o /tmp/aflcheck /tmp/aflcheck.c && \
    /usr/local/bin/afl-showmap -q -o /tmp/aflmap -- /tmp/aflcheck && \
    test -s /tmp/aflmap && \
    echo "AFL++ instruments: OK" && rm -f /tmp/aflcheck.c /tmp/aflcheck /tmp/aflmap
```

- [ ] **Step 2: Verify the section is well-formed**

Run: `docker build --target <dev-stage> -t map2check-dev .` (or the project's image build). The AFL++ smoke `RUN` must print `AFL++ instruments: OK`; if `afl-showmap` fails, the build fails.

- [ ] **Step 3: Commit**

```bash
git add Dockerfile.dev
git commit -m "feat(tacasv1): install AFL++ 4.40c (PCGUARD) in the dev image"
```

---

### Task 2: Resolve AFL++ binaries in tools.hpp

**Files:**
- Modify: `modules/frontend/utils/tools.hpp` (after the `kleeBinary` constant, line 61)

**Interfaces:**
- Produces: `Map2Check::aflClangFastBinary()` → `std::string`, `Map2Check::aflFuzzBinary()` → `std::string`. Consumed by `caller.cpp` in Task 5.

- [ ] **Step 1: Add the two resolvers**

Insert after `constexpr char const* kleeBinary = "${MAP2CHECK_PATH}/bin/klee";`:

```cpp
/** Default root of the AFL++ install (Dockerfile.dev section 7). */
constexpr char const* aflDefaultRoot = "/usr/local";
/** Path to the afl-clang-fast wrapper (symlink to afl-cc), overridable.
 *
 * Resolved like the slicer and the invariant generator: an environment
 * override first, a documented default second. AFL++ is a subprocess tool,
 * invoked by caller.cpp at run time, so it is not copied into MAP2CHECK_PATH
 * the way clang and klee are. */
inline std::string aflClangFastBinary() {
  const char* override_path = getenv("AFL_CC");
  if (override_path != nullptr) return std::string(override_path);
  return std::string(aflDefaultRoot) + "/bin/afl-clang-fast";
}
/** Path to the afl-fuzz binary, overridable. */
inline std::string aflFuzzBinary() {
  const char* override_path = getenv("AFL_FUZZ");
  if (override_path != nullptr) return std::string(override_path);
  return std::string(aflDefaultRoot) + "/bin/afl-fuzz";
}
```

- [ ] **Step 2: Verify it compiles**

Run: `cmake --build <build-dir> --target map2check 2>&1 | tail -5`
Expected: no errors (the functions are `inline`, so no link issues; they are not yet referenced).

- [ ] **Step 3: Commit**

```bash
git add modules/frontend/utils/tools.hpp
git commit -m "feat(tacasv1): resolve afl-clang-fast/afl-fuzz paths"
```

---

### Task 3: Swap the CMake module and option

**Files:**
- Create: `cmake/FindAFLPlusPlus.cmake`
- Delete: `cmake/FindLibFuzzer.cmake`
- Modify: `CMakeLists.txt:5`, `CMakeLists.txt:63-65`

**Interfaces:**
- Produces: `AFL_PLUS_PLUS_FOUND` (CMake var). Consumed by nothing functional (availability reporting only), matching `FindLibFuzzer`'s former role.

- [ ] **Step 1: Write `cmake/FindAFLPlusPlus.cmake`**

```cmake
# FindAFLPlusPlus.cmake — Locate the AFL++ fuzzers (4.40c, LLVM 16)
#
# AFL++ is a standalone toolchain invoked at run time by caller.cpp through
# system(): afl-clang-fast compiles the fuzzer binary (PCGUARD) and afl-fuzz
# drives it. It is installed into the image by Dockerfile.dev section 7 and
# resolved at run time by Map2Check::aflClangFastBinary() /
# Map2Check::aflFuzzBinary() (tools.hpp), which honour an env override and fall
# back to /usr/local/bin.
#
# This module only records availability so the build can say so — the role
# FindLibFuzzer.cmake played before the AFL++ migration.
#
# Sets:
#   AFL_PLUS_PLUS_FOUND  — TRUE if both binaries are present

find_program(AFL_CLANG_FAST afl-clang-fast PATHS /usr/local/bin /opt/afl++/bin)
find_program(AFL_FUZZ afl-fuzz PATHS /usr/local/bin /opt/afl++/bin)

if(AFL_CLANG_FAST AND AFL_FUZZ)
  set(AFL_PLUS_PLUS_FOUND TRUE)
  message(STATUS "Found AFL++: ${AFL_CLANG_FAST} / ${AFL_FUZZ}")
else()
  set(AFL_PLUS_PLUS_FOUND FALSE)
  message(WARNING "AFL++ not found (afl-clang-fast/afl-fuzz). "
    "Fuzzing will be unavailable; build the dev image (Dockerfile.dev section 7) "
    "or set AFL_CC/AFL_FUZZ.")
endif()
```

- [ ] **Step 2: Delete `cmake/FindLibFuzzer.cmake`**

Run: `git rm cmake/FindLibFuzzer.cmake`

- [ ] **Step 3: Swap the option and include in `CMakeLists.txt`**

Change line 5 from:
```cmake
option(SKIP_LIB_FUZZER "Don't use libFuzzer" OFF)
```
to:
```cmake
option(SKIP_AFL_PLUS_PLUS "Don't use AFL++" OFF)
```

Change lines 63-65 from:
```cmake
if(NOT SKIP_LIB_FUZZER)
  include(cmake/FindLibFuzzer.cmake)
endif()
```
to:
```cmake
if(NOT SKIP_AFL_PLUS_PLUS)
  include(cmake/FindAFLPlusPlus.cmake)
endif()
```

- [ ] **Step 4: Verify configure**

Run: `cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR 2>&1 | grep -i afl`
Expected: `Found AFL++: /usr/local/bin/afl-clang-fast / /usr/local/bin/afl-fuzz` (or the `AFL++ not found` warning when building outside the image — either is non-fatal).

- [ ] **Step 5: Commit**

```bash
git add cmake/FindAFLPlusPlus.cmake CMakeLists.txt
git rm cmake/FindLibFuzzer.cmake
git commit -m "feat(tacasv1): replace FindLibFuzzer with FindAFLPlusPlus"
```

---

### Task 4: Rewrite the nondet generator for AFL++

**Files:**
- Create: `modules/backend/library/lib/NonDetGeneratorAFL.c`
- Delete: `modules/backend/library/lib/NonDetGeneratorLibFuzzy.c`
- Modify: `modules/backend/library/lib/CMakeLists.txt:19`

**Interfaces:**
- Produces: `NonDetGeneratorAFL.bc` (linked by `caller.cpp` in Task 5), defining the same public symbols `NonDetGeneratorLibFuzzy.c` did: `nondet_init`, `nondet_destroy`, `nondet_cancel`, `nondet_generate_aux_witness_files`, `nondet_assume`, all `map2check_non_det_*` generators, and a `main` trampoline (LibFuzzer's `main` came from the runtime; AFL++'s comes from this file).

- [ ] **Step 1: Write `modules/backend/library/lib/NonDetGeneratorAFL.c`**

```c
/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "../header/NonDetGenerator.h"
#include "../header/NonDetLog.h"

#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>

/* Logic used for cases generation:
   1 - main function of original program is changed to _map2check_main
   2 - AFL++ persistent mode feeds one test case per __AFL_LOOP iteration
 */

extern int __map2check_main__(int argc, char **argv);

#include "../header/Map2CheckFunctions.h"

void nondet_init() { nondet_log_init(); }

void nondet_destroy() { nondet_log_destroy(); }

static jmp_buf map2check_reject_env;

void nondet_cancel() { longjmp(map2check_reject_env, 1); }

void nondet_assume(int expr) {
  if (!expr) {
    nondet_cancel();
  }
}

void nondet_generate_aux_witness_files() {
  nondet_log_to_file(map2check_nondet_get_log());
}

const uint8_t *map2check_afl_data;

size_t map2check_afl_size;

uint8_t get_next_input_from_afl() {
  static int i = 0;
  if (i < map2check_afl_size) {
    return map2check_afl_data[i++];
  }

  i = 0;
  return map2check_afl_data[i];
}

/* Fills `out` with `size` bytes from the AFL buffer, in target order.
 *
 * Same width contract as NonDetGeneratorKlee.c: sizeof(type) bytes per value,
 * so a vector means the same thing to both engines and seeding stays sound. */
static void get_bytes_from_afl(void *out, size_t size) {
  unsigned char *destination = (unsigned char *)out;
  size_t i = 0;
  for (; i < size; i++) {
    destination[i] = get_next_input_from_afl();
  }
}

#define MAP2CHECK_NON_DET_GENERATOR(type)                                      \
  type map2check_non_det_##type() {                                            \
    type value;                                                                \
    get_bytes_from_afl(&value, sizeof(value));                                 \
    return value;                                                              \
  }

MAP2CHECK_NON_DET_GENERATOR(char)
MAP2CHECK_NON_DET_GENERATOR(pointer)
MAP2CHECK_NON_DET_GENERATOR(ushort)
MAP2CHECK_NON_DET_GENERATOR(short)
MAP2CHECK_NON_DET_GENERATOR(long)
MAP2CHECK_NON_DET_GENERATOR(ulong)
MAP2CHECK_NON_DET_GENERATOR(bool)
MAP2CHECK_NON_DET_GENERATOR(uchar)
MAP2CHECK_NON_DET_GENERATOR(size_t)
#ifndef __INTELLISENSE__
MAP2CHECK_NON_DET_GENERATOR(loff_t)
#endif
MAP2CHECK_NON_DET_GENERATOR(sector_t)
MAP2CHECK_NON_DET_GENERATOR(double)
MAP2CHECK_NON_DET_GENERATOR(int)
MAP2CHECK_NON_DET_GENERATOR(uint)
MAP2CHECK_NON_DET_GENERATOR(unsigned)

#define MAP2CHECK_MAX_FUZZED_STRING 4096

char *map2check_non_det_pchar() {
  unsigned length = map2check_non_det_unsigned();
  if (length == 0)
    return NULL;
  if (length > MAP2CHECK_MAX_FUZZED_STRING)
    length = MAP2CHECK_MAX_FUZZED_STRING;
  char *string = malloc(length);
  if (string == NULL)
    return NULL;
  unsigned i = 0;
  for (i = 0; i < (length - 1); i++) {
    string[i] = map2check_non_det_char();
  }
  string[i] = '\0';
  return string;
}

/* AFL++ persistent-mode trampoline.
 *
 * __AFL_FUZZ_INIT registers the shared-memory test case. __AFL_LOOP runs the
 * body once per input under afl-fuzz; run standalone (replaying a saved crash
 * file as argv[1]) it runs exactly once with that file as input.
 *
 * A failed nondet_assume longjmps back here and skips the input — the
 * persistent-mode equivalent of the pthread_exit the LibFuzzer generator used
 * (a rejected input, not a crash). */
__AFL_FUZZ_INIT();

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  while (__AFL_LOOP(10000)) {
    if (setjmp(map2check_reject_env) == 0) {
      map2check_afl_data = __AFL_FUZZ_TESTCASE_BUF;
      map2check_afl_size = __AFL_FUZZ_TESTCASE_LEN;
      __map2check_main__(0, NULL);
    }
    /* else: input rejected by nondet_assume; continue to the next iteration */
  }
  return 0;
}
```

- [ ] **Step 2: Delete `NonDetGeneratorLibFuzzy.c` and update the library list**

Run: `git rm modules/backend/library/lib/NonDetGeneratorLibFuzzy.c`

Change `modules/backend/library/lib/CMakeLists.txt:19` from:
```cmake
list(APPEND MAP2CHECK_C_LIB "NonDetGeneratorLibFuzzy")
```
to:
```cmake
list(APPEND MAP2CHECK_C_LIB "NonDetGeneratorAFL")
```

- [ ] **Step 3: Verify the bytecode builds**

Run: `cmake --build <build-dir> --target NonDetGeneratorAFL 2>&1 | tail -5`
Expected: `Compiling NonDetGeneratorAFL to bytecode` and `modules/backend/library/lib/NonDetGeneratorAFL.bc` exists. (The `.bc` is emitted via `clang -c -emit-llvm`, which does NOT need `afl-clang-fast`; instrumentation happens at the final native compile in Task 5.)

- [ ] **Step 4: Commit**

```bash
git add modules/backend/library/lib/NonDetGeneratorAFL.c modules/backend/library/lib/CMakeLists.txt
git rm modules/backend/library/lib/NonDetGeneratorLibFuzzy.c
git commit -m "feat(tacasv1): AFL++ persistent nondet generator"
```

---

### Task 5: Rewire the Caller (enum, link, compile, execute)

**Files:**
- Modify: `modules/frontend/caller.hpp:42-46` (enum), comment-only updates at 40-41, 84, 142, 151, 162, 165
- Modify: `modules/frontend/caller.cpp:315-368` (`applyNonDetGenerator`), `546-548` (`linkLLVM`), `782-839` (`executeAnalysis`)

**Interfaces:**
- Consumes: `Map2Check::aflClangFastBinary()`, `Map2Check::aflFuzzBinary()` (Task 2); `NonDetGeneratorAFL.bc` (Task 4).
- Produces: `NonDetGenerator::AFLPlusPlus` enum value, consumed by `map2check.cpp` (Task 6).

- [ ] **Step 1: Rename the enum in `caller.hpp`**

Change:
```cpp
enum class NonDetGenerator {
  None,      /**< Do not generate any input */
  LibFuzzer, /**< LibFuzzer from LLVM */
  Klee,      /**< Use klee for symbolic analysis */
};
```
to:
```cpp
enum class NonDetGenerator {
  None,       /**< Do not generate any input */
  AFLPlusPlus, /**< AFL++ (persistent mode, PCGUARD) */
  Klee,       /**< Use klee for symbolic analysis */
};
```

- [ ] **Step 2: Swap the link target in `caller.cpp linkLLVM()` (lines 546-548)**

Change:
```cpp
    case (NonDetGenerator::LibFuzzer): {
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorLibFuzzy.bc";
      break;
    }
```
to:
```cpp
    case (NonDetGenerator::AFLPlusPlus): {
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorAFL.bc";
      break;
    }
```

- [ ] **Step 3: Swap the compile commands in `applyNonDetGenerator()` (lines 315-368)**

Change the `case (NonDetGenerator::LibFuzzer):` label to `case (NonDetGenerator::AFLPlusPlus):`, the log line to `"Instrumenting with AFL++"`, and the two commands:

From:
```cpp
      command
          << bound << Map2Check::clangBinary
          << "  -g -fsanitize=fuzzer -fsanitize-coverage=inline-8bit-counters "
          << Caller::postOptimizationFlags()
          << " -o " + programHash + "-fuzzed.out"
          << " " + programHash + "-result.bc";
```
to:
```cpp
      command
          << bound << Map2Check::aflClangFastBinary()
          << "  -g " << Caller::postOptimizationFlags()
          << " -o " + programHash + "-fuzzed.out"
          << " " + programHash + "-result.bc";
```

From:
```cpp
      commandWitness << bound << Map2Check::clangBinary
                     << "  -g -fsanitize=fuzzer "
                     << " -o " + programHash + "-witness-fuzzed.out"
                     << " " + programHash + "-witness-result.bc";
```
to:
```cpp
      commandWitness << bound << Map2Check::aflClangFastBinary()
                     << "  -g "
                     << " -o " + programHash + "-witness-fuzzed.out"
                     << " " + programHash + "-witness-result.bc";
```

And the build-failure warning text (lines 361-366): replace `"the LibFuzzer binary did not build within "` with `"the AFL++ binary did not build within "`.

- [ ] **Step 4: Swap the exec in `executeAnalysis()` (lines 782-839)**

Change the `case (NonDetGenerator::LibFuzzer):` label to `case (NonDetGenerator::AFLPlusPlus):`, and the messages `"Executing LibFuzzer with map2check"` / `"the LibFuzzer binary is unavailable"` to `"Executing AFL++ with map2check"` / `"the AFL++ binary is unavailable"`.

Replace the exec block (from `Map2Check::Log::Info("Executing ...")` through the `commandWitness` replay) with:

```cpp
      Map2Check::Log::Info("Executing AFL++ with map2check");
      std::ostringstream command;
      command.str("");
      // Against what is LEFT, not against the nominal budget — see
      // Caller::remainingSeconds.
      const double fuzzerBudget =
          std::min(0.2 * this->timeout,
                   static_cast<double>(this->remainingSeconds()));
      // afl-fuzz needs a non-empty -i dir (LibFuzzer started from empty), and
      // a -o dir that does not already exist (the hybrid may run the fuzzer
      // phase twice). One minimal seed, and a clean output dir each time.
      std::error_code seedErr;
      std::filesystem::create_directories(Caller::seedDirectory, seedErr);
      std::string seedFile = std::string(Caller::seedDirectory) + "/seed";
      if (!std::filesystem::exists(seedFile, seedErr)) {
        std::ofstream seed(seedFile);
        seed << "A";
      }
      std::filesystem::remove_all("afl-out", seedErr);
      command << "timeout -k " << Map2Check::killGracePeriod << " "
              << static_cast<unsigned>(fuzzerBudget) << " ";
      command << Map2Check::aflFuzzBinary()
              << " -i " << Caller::seedDirectory
              << " -o afl-out"
              << " -V " << std::max(1u, static_cast<unsigned>(fuzzerBudget))
              << " -- ./" << programHash << "-fuzzed.out"
              << " > fuzzer.output 2>&1";

      int result = system(command.str().c_str());
      Map2Check::Log::Warning("Exited fuzzer with " + std::to_string(result));
      if (result == 31744)  // Timeout
        gotTimeout = true;

      // Replay any crash with the witness binary to confirm a real violation.
      // __AFL_FUZZ_INIT reads argv[1] as the input file when run standalone.
      std::error_code crashErr;
      if (std::filesystem::exists("afl-out/crashes", crashErr)) {
        for (const auto &entry :
             std::filesystem::directory_iterator("afl-out/crashes")) {
          std::ostringstream commandWitness;
          commandWitness.str("");
          commandWitness << "./" << programHash << "-witness-fuzzed.out "
                         << entry.path().string();
          system(commandWitness.str().c_str());
        }
      }
      Map2Check::Log::Debug("Finished fuzzer");

      if (isWitnessFileCreated()) {
        witnessVerified = true;
      }

      break;
```

(Keep the surrounding `hasFuzzer` availability check and the final `isWitnessFileCreated()` after the switch exactly as they are.)

- [ ] **Step 5: Verify it compiles**

Run: `cmake --build <build-dir> --target map2check 2>&1 | tail -20`
Expected: build succeeds with no `NonDetGenerator::LibFuzzer` references remaining. If any remain, the compiler will error on the removed enum value.

- [ ] **Step 6: Commit**

```bash
git add modules/frontend/caller.hpp modules/frontend/caller.cpp
git commit -m "feat(tacasv1): drive AFL++ from the Caller"
```

---

### Task 6: Rename the CLI value and hybrid loop

**Files:**
- Modify: `modules/frontend/map2check.cpp:725-727` (help), `901` (valid values), `913` (capture), `950` & `976` (hybrid loop), `571` & `583` (evidence guard)

**Interfaces:**
- Consumes: `NonDetGenerator::AFLPlusPlus` (Task 5).
- Produces: CLI value `afl` for `--nondet-generator`.

- [ ] **Step 1: Help text (lines 725-727)**

Change:
```cpp
        ("nondet-generator", po::value<std::string>(),
                      R"(specifies the nondet-generator, valid values are fuzzer (libFuzzer),
symex (Klee))")
```
to:
```cpp
        ("nondet-generator", po::value<std::string>(),
                      R"(specifies the nondet-generator, valid values are afl (AFL++),
symex (Klee))")
```

- [ ] **Step 2: Valid values and capture (lines 901, 913)**

Change `{"fuzzer", "symex"}` to `{"afl", "symex"}`, and:
```cpp
        if(generatorname == available_generators[0])
          args.generator = Map2Check::NonDetGenerator::LibFuzzer;
```
to:
```cpp
        if(generatorname == available_generators[0])
          args.generator = Map2Check::NonDetGenerator::AFLPlusPlus;
```

- [ ] **Step 3: Hybrid loop (lines 950, 976)**

Change both `args.generator = Map2Check::NonDetGenerator::LibFuzzer;` to `args.generator = Map2Check::NonDetGenerator::AFLPlusPlus;`.

- [ ] **Step 4: Evidence guard (lines 571, 583)**

Change `Map2Check::NonDetGenerator::LibFuzzer` to `Map2Check::NonDetGenerator::AFLPlusPlus` in both the `evidenceIsTrustworthy` condition and the `!caller->isVerified()` condition.

- [ ] **Step 5: Verify**

Run: `cmake --build <build-dir> --target map2check 2>&1 | tail -20`
Then: `./release/bin/map2check --help 2>&1 | grep -A1 nondet-generator`
Expected: the help shows `valid values are afl (AFL++), symex (Klee)`.

- [ ] **Step 6: Commit**

```bash
git add modules/frontend/map2check.cpp
git commit -m "feat(tacasv1): --nondet-generator afl replaces fuzzer"
```

---

### Task 7: End-to-end smoke test

**Files:**
- Test (throwaway): a temporary C file, removed after the test.

**Interfaces:**
- Consumes: the full build from Tasks 1-6.

- [ ] **Step 1: Write the smoke program**

```c
extern int __VERIFIER_nondet_int(void);
void reach_error(void) { __builtin_trap(); }
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x != 0) {
    reach_error();
  }
  return 0;
}
```

Save as `/tmp/tacas-smoke.c`. The `x != 0` guard is trivially satisfied, so AFL++ finds the crash on its first inputs — the point is to exercise instrumentation + crash replay + verdict, not fuzzing efficacy.

- [ ] **Step 2: Run map2check against it**

Run (inside the dev image, with `release/bin` on PATH):
```bash
map2check --target-function --target-function-name reach_error \
  --nondet-generator afl --timeout 30 /tmp/tacas-smoke.c
```
Expected: the run reports a violation (verdict `FALSE` / `TARGET_REACHED`, not `UNKNOWN`), and the fuzzer log shows `Executing AFL++ with map2check`.

- [ ] **Step 3: Confirm the AFL++ specifics**

Run: `grep -i "no instrumentation" fuzzer.output; echo $?` inside the scratch dir.
Expected: `1` (no "no instrumentation" line — AFL++ instrumented the binary). If `0`, the PCGUARD instrumentation did not apply to the pre-linked `.bc`; see the fallback in the spec §8 (instrument `compileCFile()` with `afl-clang-fast` instead).

- [ ] **Step 4: Confirm the hybrid default still works**

Run:
```bash
map2check --target-function --target-function-name reach_error --timeout 30 /tmp/tacas-smoke.c
```
Expected: fuzzer (AFL++) → KLEE sequence runs and reaches a verdict (no `fuzzer` string left in `--help`, no `LibFuzzer` in the log).

- [ ] **Step 5: Clean up and commit the spec reference**

```bash
rm -f /tmp/tacas-smoke.c
```

No repo change; if the smoke exposed a gap, fix it in the owning task before proceeding.

---

### Task 8: CI/release scripts and docs

**Files:**
- Modify: `.github/workflows/ci.yml`, `.github/workflows/release.yml`, `scripts/make-release.sh`, `scripts/prepare-release.sh`, `make-unit-test.sh` (all `-DSKIP_LIB_FUZZER=ON` → `-DSKIP_AFL_PLUS_PLUS=ON`)
- Modify: `README.md`, `CLAUDE.md`, `CHANGELOG.md`, `TODO.md`, `docs/map2check_migration_plan.md`

**Interfaces:**
- None (documentation/config parity with the rename).

- [ ] **Step 1: Scripts and CI flag rename**

Run across the five files:
```bash
grep -rl 'SKIP_LIB_FUZZER' .github scripts make-unit-test.sh | xargs sed -i 's/SKIP_LIB_FUZZER/SKIP_AFL_PLUS_PLUS/g'
```
Then inspect each diff (`git diff`) to confirm only the flag name changed, and that no `libFuzzer`/`LibFuzzer` references remain in those files (update the surrounding comments where they mention LibFuzzer, e.g. `release.yml`'s header comment and `make-release.sh`'s `cp libFuzzer.a` line — replace that copy with AFL++ availability, since AFL++ is not copied into the release dir).

- [ ] **Step 2: Docs**

- `README.md` line 15 and `CLAUDE.md` line 7: replace "LibFuzzer" with "AFL++" in the stack description.
- `README.md`/`CLAUDE.md` build instructions: `-DSKIP_LIB_FUZZER=ON` → `-DSKIP_AFL_PLUS_PLUS=ON`.
- `CHANGELOG.md`: add a `tacasv1` entry — "Replaced LibFuzzer with AFL++ 4.40c (persistent, PCGUARD) as the fuzzing engine."
- `TODO.md`: update the `dynamic_analysis_unsafe` note (the embedded fuzzer is now AFL++, not LibFuzzer).
- `docs/map2check_migration_plan.md` §3.1 (mark `FindAFLPlusPlus.cmake`/PCGUARD/wrapper done) and §3.2 (note the coordinator **stays in Caller C++**, not `modules/coordinator/` Python/pybind11).

- [ ] **Step 3: Verify no stragglers**

Run:
```bash
grep -rniE 'libfuzzer|libfuzzy' --include='*.cpp' --include='*.hpp' --include='*.c' --include='*.h' --include='*.cmake' --include='CMakeLists.txt' --include='*.yml' --include='*.sh' modules/ cmake/ .github/ scripts/ make-unit-test.sh
```
Expected: only historical mentions in `docs/` (which are intentionally left for the record), nothing in `modules/`, `cmake/`, `.github/`, `scripts/`.

- [ ] **Step 4: Commit**

```bash
git add .github scripts make-unit-test.sh README.md CLAUDE.md CHANGELOG.md TODO.md docs/map2check_migration_plan.md
git commit -m "chore(tacasv1): rename SKIP_LIB_FUZZER→SKIP_AFL_PLUS_PLUS and update docs"
```

---

## Self-Review

- **Spec coverage:** every spec section maps to a task — §5 build (Tasks 1, 3), §6 runtime (Tasks 2, 4, 5, 6), §9 smoke (Task 7), §10 docs (Task 8). The parallel `-M/-S` and smart-seed work are explicitly out of scope, matching the spec §4.
- **Placeholders:** none — every code step has the full code, every command has an expected result.
- **Type consistency:** `NonDetGenerator::AFLPlusPlus` is defined in Task 5 and referenced identically in Task 6; `aflClangFastBinary()`/`aflFuzzBinary()` defined in Task 2, used in Task 5; `NonDetGeneratorAFL.bc` produced in Task 4, linked in Task 5.
