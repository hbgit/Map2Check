# tacasv2a — Slicing for reach/assert that keeps the suite valid: Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make `--slice` stop crashing KLEE and stop producing test suites that are invalid on the original program. Also extend `--slice` to assert mode.

**Architecture:** Slicing stays where it is: in `Caller::sliceWithRespectToTarget`, before instrumentation. The pure pieces are:
- the criteria list, built from the target plus every `__VERIFIER_nondet_*` function;
- the weak-stub source;
- the parser for the slicer's `--statistics` output.

They move into a new header-only unit `modules/frontend/utils/slicer.hpp`, so they can be unit-tested without a build of the whole tool. The Caller then passes `-cutoff-diverging=false --statistics` and logs the counts. `map2check.cpp` enables the assert-mode criterion.

**Tech Stack:** C++17, LLVM 16, sbt-slicer (dg) pinned in `Dockerfile.dev`, GTest, bash integration tests. Build and run everything inside the dev image (`map2check-dev:aflpp`, built from `Dockerfile.dev`); the host has no clang, cmake or ninja.

**Spec:** `docs/superpowers/specs/2026-09-26-tacasv2a-slicing-reach-assert-design.md`

## Global Constraints

- Branch `tacas/slicing` (created from `tacas/aflpp`); the baseline is tacasv1. Never target LibFuzzer.
- Cutoff: `-cutoff-diverging=false`.
- Reachability criterion: `<target function>` plus every known `__VERIFIER_nondet_*` function.
- Assert criterion: `__VERIFIER_assert,__assert_fail` plus the same nondet functions (AssertPass instruments both).
- Slice before instrumentation (unchanged). Keep the weak stub for the criterion function.
- Other slicer flags (`--pta`, `--cda`, `--undefined-funs`) stay at their defaults in this stage.
- Pass `--statistics` to the slicer, and log globals/functions/blocks/instructions before → after, plus bytes.
- C++: Google style (`.clang-format`), `<filesystem>` (never boost). Commit messages end with `Co-Authored-By: Claude Opus 5.5 (1M context) <noreply@anthropic.com>`.

## Review Focus

- **A program that declares no nondet function at all** (`no_input.c`-like). The criteria must still be the target alone plus names that do not exist, and the slicer accepts those silently (verified). Pinned in Task 1.
- **`--slice` in assert mode on a program that only declares `__VERIFIER_assert`.** The weak stub must have the `(int)` signature or `llvm-link` rejects the type. Pinned in Task 3.
- **A slicer build whose `--statistics` prints nothing, or a different format.** The log must fall back to bytes only and never print zeros as if measured. Pinned in Task 1 (parser returns `found=false`).
- **A program with a nondet read that is irrelevant to the target and consumed before the relevant one.** The suite must carry both values, in the original order. Pinned in Task 2.
- **A program whose non-target branch returns early** (the path the cutoff used to rewrite). KLEE must run and report FAILED, not abort on a broken module. Pinned in Task 2.

---

## File Structure

- Create `modules/frontend/utils/slicer.hpp`, header-only and pure (no I/O). It holds `nondetFunctionNames()`, `slicingCriteria()`, `targetStubSource()`, `SlicerStatistics`, `parseSlicerStatistics()` and `describeSlice()`.
- Create `tests/unit/frontend/SlicerTest.cpp` with the GTest unit tests for `slicer.hpp`.
- Modify `tests/unit/frontend/CMakeLists.txt` to register `SlicerTest`.
- Modify `modules/frontend/caller.cpp` so that `sliceWithRespectToTarget` uses `slicer.hpp` and passes the new flags.
- Modify `modules/frontend/caller.hpp` to update the doc comment of `sliceWithRespectToTarget`; the signature stays the same.
- Modify `modules/frontend/map2check.cpp` for the `--slice` gating (reach + assert) and the help text.
- Modify `tests/integration/test_testcomp_regressions.sh`: new slicing sections, and the refusal-message check updated.
- Modify `CHANGELOG.md` to add a tacasv2a entry.

---

### Task 1: Pure slicing helpers (`slicer.hpp`) with unit tests

**Files:**
- Create: `modules/frontend/utils/slicer.hpp`
- Create: `tests/unit/frontend/SlicerTest.cpp`
- Modify: `tests/unit/frontend/CMakeLists.txt` (append)

**Interfaces:**
- Produces (namespace `Map2Check`):
  - `const std::vector<std::string>& nondetFunctionNames();`
  - `std::string slicingCriteria(const std::vector<std::string>& primary);` returns the comma-joined `primary` followed by every nondet name.
  - `std::string targetStubSource(const std::string& function);` returns a one-line C weak definition with the right signature.
  - `struct SlicerCounts { unsigned globals = 0, functions = 0, blocks = 0, instructions = 0; };`
  - `struct SlicerStatistics { bool found = false; SlicerCounts before, after; };`
  - `SlicerStatistics parseSlicerStatistics(const std::string& slicerOutput);`
  - `std::string describeSlice(const std::string& criterion, const SlicerStatistics& stats, uintmax_t bytesBefore, uintmax_t bytesAfter);`

- [ ] **Step 1: Write the failing unit tests**

Create `tests/unit/frontend/SlicerTest.cpp`:

```cpp
/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "../../../modules/frontend/utils/slicer.hpp"

// The suite is generated on the slice and run by TestCov on the ORIGINAL
// program, so every nondet read the original performs must survive slicing.
TEST(SlicingCriteria, AppendsEveryNondetFunctionAfterThePrimary) {
  const std::string criteria = Map2Check::slicingCriteria({"reach_error"});
  EXPECT_EQ(criteria.rfind("reach_error,", 0), 0u);
  for (const std::string& name : Map2Check::nondetFunctionNames()) {
    EXPECT_NE(criteria.find("," + name), std::string::npos) << name;
  }
}

TEST(SlicingCriteria, KeepsSeveralPrimariesInOrder) {
  const std::string criteria =
      Map2Check::slicingCriteria({"__VERIFIER_assert", "__assert_fail"});
  EXPECT_EQ(criteria.rfind("__VERIFIER_assert,__assert_fail,", 0), 0u);
}

TEST(NondetFunctionNames, CoversWhatNonDetPassInstruments) {
  const auto& names = Map2Check::nondetFunctionNames();
  for (const char* type : {"bool", "char", "uchar", "short", "ushort", "int",
                           "uint", "unsigned", "long", "ulong", "size_t",
                           "loff_t", "sector_t", "pointer", "pchar", "double"}) {
    const std::string name = std::string("__VERIFIER_nondet_") + type;
    EXPECT_NE(std::find(names.begin(), names.end(), name), names.end()) << name;
  }
}

TEST(TargetStubSource, VoidTargetGetsAVoidStub) {
  EXPECT_EQ(Map2Check::targetStubSource("reach_error"),
            "void __attribute__((weak)) reach_error(void) {}\n");
}

// __VERIFIER_assert takes the condition; a (void) stub would not link against
// the program's own declaration.
TEST(TargetStubSource, AssertStubTakesTheCondition) {
  EXPECT_EQ(Map2Check::targetStubSource("__VERIFIER_assert"),
            "void __attribute__((weak)) __VERIFIER_assert(int cond) {}\n");
}

TEST(ParseSlicerStatistics, ReadsBeforeAndAfter) {
  const std::string output =
      "Statistics before Globals/Functions/Blocks/Instr.: 37 97 2215 10764\n"
      "[llvm-slicer] Sliced away 1454 from 4227 nodes in DG\n"
      "Statistics after Globals/Functions/Blocks/Instr.: 37 38 444 2989\n";
  const Map2Check::SlicerStatistics stats =
      Map2Check::parseSlicerStatistics(output);
  ASSERT_TRUE(stats.found);
  EXPECT_EQ(stats.before.functions, 97u);
  EXPECT_EQ(stats.before.blocks, 2215u);
  EXPECT_EQ(stats.before.instructions, 10764u);
  EXPECT_EQ(stats.after.globals, 37u);
  EXPECT_EQ(stats.after.functions, 38u);
  EXPECT_EQ(stats.after.instructions, 2989u);
}

// A slicer that prints no statistics must not be reported as having sliced
// everything away.
TEST(ParseSlicerStatistics, MissingLinesAreNotFound) {
  EXPECT_FALSE(Map2Check::parseSlicerStatistics("").found);
  EXPECT_FALSE(Map2Check::parseSlicerStatistics(
                   "Statistics before Globals/Functions/Blocks/Instr.: 1 2 3 4\n")
                   .found);
}

TEST(DescribeSlice, ReportsCountsWhenFound) {
  Map2Check::SlicerStatistics stats;
  stats.found = true;
  stats.before = {37, 97, 2215, 10764};
  stats.after = {37, 38, 444, 2989};
  EXPECT_EQ(Map2Check::describeSlice("reach_error", stats, 184164, 171708),
            "Sliced with respect to reach_error: 97/2215/10764 -> 38/444/2989 "
            "functions/blocks/instructions (184164 -> 171708 bytes of "
            "bitcode)");
}

TEST(DescribeSlice, FallsBackToBytesWithoutStatistics) {
  EXPECT_EQ(Map2Check::describeSlice("reach_error", {}, 10, 8),
            "Sliced with respect to reach_error: 10 -> 8 bytes of bitcode");
}
```

Append to `tests/unit/frontend/CMakeLists.txt`:

```cmake

add_executable(SlicerTest
    SlicerTest.cpp
)
map2check_test(SlicerTest)
```

- [ ] **Step 2: Run the tests and confirm they fail to compile**

Run inside the dev image:
```bash
docker run --rm -v $(pwd):/workspace map2check-dev:aflpp bash -c \
  'mkdir -p /workspace/build_ut && cd /workspace/build_ut && cmake .. -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm -DENABLE_TEST=ON >/dev/null && ninja SlicerTest 2>&1 | tail -5'
```
Expected: FAIL with `fatal error: '../../../modules/frontend/utils/slicer.hpp' file not found`.

- [ ] **Step 3: Implement `slicer.hpp`**

Create `modules/frontend/utils/slicer.hpp`:

```cpp
/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_UTILS_SLICER_HPP_
#define MODULES_FRONTEND_UTILS_SLICER_HPP_

#include <cstdint>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace Map2Check {

/** Every __VERIFIER_nondet_* function the slice must keep.
 *
 * The suite is generated on the slice, but TestCov runs it on the ORIGINAL
 * program. A nondet call the slicer drops -- its value does not reach the
 * criterion -- is still consumed by the original, so the vector shifts and
 * the suite stops covering (measured: ntdrivers/floppy.i.cil-1.c, 29 reads in
 * the program and 10 in the slice; FAILED, NOT_COVERED). Keeping these calls
 * as criteria keeps the consumption order.
 *
 * The first sixteen are what NonDetPass instruments; the rest are SV-COMP
 * names it does not model yet, kept so their order is not lost either. The
 * slicer accepts names the program does not use. */
inline const std::vector<std::string>& nondetFunctionNames() {
  static const std::vector<std::string> names = {
      "__VERIFIER_nondet_bool",     "__VERIFIER_nondet_char",
      "__VERIFIER_nondet_uchar",    "__VERIFIER_nondet_short",
      "__VERIFIER_nondet_ushort",   "__VERIFIER_nondet_int",
      "__VERIFIER_nondet_uint",     "__VERIFIER_nondet_unsigned",
      "__VERIFIER_nondet_long",     "__VERIFIER_nondet_ulong",
      "__VERIFIER_nondet_size_t",   "__VERIFIER_nondet_loff_t",
      "__VERIFIER_nondet_sector_t", "__VERIFIER_nondet_pointer",
      "__VERIFIER_nondet_pchar",    "__VERIFIER_nondet_double",
      "__VERIFIER_nondet_float",    "__VERIFIER_nondet_longlong",
      "__VERIFIER_nondet_ulonglong", "__VERIFIER_nondet__Bool",
      "__VERIFIER_nondet_u8",       "__VERIFIER_nondet_u16",
      "__VERIFIER_nondet_u32",      "__VERIFIER_nondet_charp"};
  return names;
}

/** The -c argument: the primary criteria, then every nondet function. */
inline std::string slicingCriteria(const std::vector<std::string>& primary) {
  std::ostringstream criteria;
  bool first = true;
  for (const std::string& name : primary) {
    criteria << (first ? "" : ",") << name;
    first = false;
  }
  for (const std::string& name : nondetFunctionNames()) {
    criteria << (first ? "" : ",") << name;
    first = false;
  }
  return criteria.str();
}

/** A weak definition of the criterion function, restoring the body the
 * slicer removes without displacing a real one. The signature must match
 * the program's declaration, or llvm-link rejects the module. */
inline std::string targetStubSource(const std::string& function) {
  if (function == "__VERIFIER_assert") {
    return "void __attribute__((weak)) __VERIFIER_assert(int cond) {}\n";
  }
  return "void __attribute__((weak)) " + function + "(void) {}\n";
}

struct SlicerCounts {
  unsigned globals = 0;
  unsigned functions = 0;
  unsigned blocks = 0;
  unsigned instructions = 0;
};

struct SlicerStatistics {
  bool found = false;  // both the "before" and the "after" line were read
  SlicerCounts before;
  SlicerCounts after;
};

/** Reads sbt-slicer's --statistics lines:
 *   Statistics before Globals/Functions/Blocks/Instr.: 37 97 2215 10764
 *   Statistics after Globals/Functions/Blocks/Instr.: 37 38 444 2989 */
inline SlicerStatistics parseSlicerStatistics(const std::string& slicerOutput) {
  static const std::regex line(
      R"(Statistics (before|after) Globals/Functions/Blocks/Instr\.:\s+)"
      R"((\d+)\s+(\d+)\s+(\d+)\s+(\d+))");
  SlicerStatistics stats;
  bool sawBefore = false;
  bool sawAfter = false;
  for (std::sregex_iterator it(slicerOutput.begin(), slicerOutput.end(), line),
       end;
       it != end; ++it) {
    const std::smatch& m = *it;
    SlicerCounts counts;
    counts.globals = static_cast<unsigned>(std::stoul(m[2]));
    counts.functions = static_cast<unsigned>(std::stoul(m[3]));
    counts.blocks = static_cast<unsigned>(std::stoul(m[4]));
    counts.instructions = static_cast<unsigned>(std::stoul(m[5]));
    if (m[1] == "before") {
      stats.before = counts;
      sawBefore = true;
    } else {
      stats.after = counts;
      sawAfter = true;
    }
  }
  stats.found = sawBefore && sawAfter;
  return stats;
}

/** The one log line a slice produces. Counts when the slicer reported them,
 * bytes always -- a slice narrows the question being answered, and this line
 * is the only visible sign of how much was dropped. */
inline std::string describeSlice(const std::string& criterion,
                                 const SlicerStatistics& stats,
                                 uintmax_t bytesBefore, uintmax_t bytesAfter) {
  std::ostringstream text;
  text << "Sliced with respect to " << criterion << ": ";
  if (stats.found) {
    text << stats.before.functions << "/" << stats.before.blocks << "/"
         << stats.before.instructions << " -> " << stats.after.functions << "/"
         << stats.after.blocks << "/" << stats.after.instructions
         << " functions/blocks/instructions (" << bytesBefore << " -> "
         << bytesAfter << " bytes of bitcode)";
  } else {
    text << bytesBefore << " -> " << bytesAfter << " bytes of bitcode";
  }
  return text.str();
}

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_SLICER_HPP_
```

- [ ] **Step 4: Run the tests and confirm they pass**

```bash
docker run --rm -v $(pwd):/workspace map2check-dev:aflpp bash -c \
  'cd /workspace/build_ut && ninja SlicerTest >/dev/null && ./tests/unit/frontend/SlicerTest 2>&1 | tail -3'
```
Expected: `[  PASSED  ] 9 tests.` (If the binary is elsewhere, find it with `find /workspace/build_ut -name SlicerTest -type f`.)

- [ ] **Step 5: Commit**

```bash
git add modules/frontend/utils/slicer.hpp tests/unit/frontend/SlicerTest.cpp tests/unit/frontend/CMakeLists.txt
git commit -m "feat(tacasv2a): pure slicing helpers -- criteria, stub, statistics

Co-Authored-By: Claude Opus 5.5 (1M context) <noreply@anthropic.com>"
```

---

### Task 2: Slice without cutoff, keep the nondets, log the statistics

**Files:**
- Modify: `modules/frontend/caller.cpp`, in `Caller::sliceWithRespectToTarget` (starts around line 179: the command construction around lines 218-222, the success log around lines 243-245, and the stub around lines 262-268)
- Modify: `modules/frontend/caller.hpp:133-136` (doc comment only)
- Test: `tests/integration/test_testcomp_regressions.sh`, new section 13 inserted **before** the final `echo "  ---"` summary lines

**Interfaces:**
- Consumes: `Map2Check::slicingCriteria`, `Map2Check::targetStubSource`, `Map2Check::parseSlicerStatistics`, `Map2Check::describeSlice` from Task 1.
- Produces: the log line `Sliced with respect to <criterion>: …` (the integration tests grep the `Sliced with respect to` prefix). `sliceWithRespectToTarget` gains a second parameter, `const std::vector<std::string>& criteria`, the primary criteria; the first stays `targetFunction`, used for the stub and the log. Task 3 calls it with `{"__VERIFIER_assert", "__assert_fail"}`.

- [ ] **Step 1: Write the failing integration tests**

In `tests/integration/test_testcomp_regressions.sh`, insert this block immediately before the lines `echo "  ---"` / `echo "  Results: ..."` at the end:

```bash
# --- 13. a slice must leave KLEE something it can run -------------------------
# sbt-slicer's --cutoff-diverging (default on) rewrites every path that cannot
# reach the criterion into a `diverge:` block calling exit(0) -- with no debug
# location. The program is compiled with -g; once KLEE links uClibc, exit has a
# body, and the verifier rejects the module ("inlinable function call in a
# function with debug info must have a !dbg location"). KLEE aborted before
# executing anything, on every sliced task with a cut path: the slice arm of
# the v15 campaign ran without its symbolic engine.
mkdir -p "$WORK/cut"
cat > "$WORK/cut/cut.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 3) { return 1; }
  if (x == 7) { reach_error(); }
  return 0;
}
EOF
( cd "$WORK/cut" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --slice \
    --nondet-generator symex --timeout 45 cut.c ) > "$WORK/cut/run.log" 2>&1
if grep -q "Broken module" "$WORK/cut/run.log"; then
  fail "slice + KLEE" "KLEE rejected the sliced module (cutoff exit without !dbg)"
elif grep -q "VERIFICATION FAILED" "$WORK/cut/run.log"; then
  ok "KLEE runs on the slice and reaches the target"
else
  fail "slice + KLEE" "no FAILED verdict on a trivially reachable target"
  grep -E "Sliced|Exited klee|VERIFICATION" "$WORK/cut/run.log" | sed 's/^/    /'
fi

# --- 14. a suite found on the slice must hold on the original ----------------
# TestCov runs the suite on the ORIGINAL program. A nondet read the slicer
# dropped -- its value does not reach the target -- is still consumed there,
# so the vector shifts: measured on ntdrivers/floppy.i.cil-1.c, FAILED and
# NOT_COVERED. The slice keeps every nondet call, so both values appear, in
# the original order.
mkdir -p "$WORK/order"
cp "$WORK/one/reach.prp" "$WORK/order/"
cat > "$WORK/order/order.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  if (b == 42) { reach_error(); }
  return a;
}
EOF
( cd "$WORK/order" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --slice \
    --nondet-generator symex --generate-test-suite --property-file reach.prp \
    --timeout 60 order.c ) > "$WORK/order/run.log" 2>&1
order_inputs=$(sed -n 's:.*<input>\(.*\)</input>.*:\1:p' \
  "$WORK/order/test-suite/testcase-1.xml" 2>/dev/null | tr '\n' ' ')
if [ "$(echo $order_inputs | wc -w)" -eq 2 ] && \
   [ "$(echo $order_inputs | awk '{print $2}')" = "42" ]; then
  ok "the sliced suite keeps the original read order [$order_inputs]"
else
  fail "slice read order" "expected 2 inputs ending in 42, got [$order_inputs]"
fi
```

- [ ] **Step 2: Build the current code and confirm the new tests fail**

```bash
docker run --rm -v $(pwd):/workspace map2check-dev:aflpp bash -c '
  mkdir -p /workspace/build_aflpp && cd /workspace/build_aflpp &&
  cmake .. -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm -DCMAKE_INSTALL_PREFIX=/workspace/build_aflpp/install >/dev/null &&
  ninja >/dev/null && ninja install >/dev/null &&
  mkdir -p install/lib/klee && ln -sfn /opt/klee/lib/klee/runtime install/lib/klee/runtime &&
  ln -sfn /usr/lib/llvm-16/lib/clang install/lib/clang &&
  cd /workspace && MAP2CHECK_PATH=/workspace/build_aflpp/install bash tests/integration/test_testcomp_regressions.sh 2>&1 | grep -E "FAIL|PASS KLEE runs|read order|Results"'
```
Expected: both `FAIL slice + KLEE: KLEE rejected the sliced module …` and `FAIL slice read order: …`.

- [ ] **Step 3: Implement the Caller change**

In `modules/frontend/caller.cpp`, add `#include "utils/slicer.hpp"` next to the other `utils/` includes.

Change the signature (definition and declaration) to:

```cpp
bool Caller::sliceWithRespectToTarget(const std::string &targetFunction,
                                      const std::vector<std::string> &criteria)
```

and in `caller.hpp` replace the declaration and its comment with:

```cpp
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
```

Replace the command construction:

```cpp
  command << "timeout -k " << Map2Check::killGracePeriod << " " << static_cast<unsigned>(sliceBudget)
          << " " << slicer << " -c " << targetFunction
          << " --entry=main -o " << output << " "
          << input << " > slicer.output 2>&1";
```

with:

```cpp
  // -cutoff-diverging=false: the cutoff rewrites every path that cannot reach
  // the criterion into exit(0) with no debug location, and once KLEE links
  // uClibc the verifier rejects the module ("Broken module found") -- KLEE
  // never ran on a sliced task with a cut path (tacasv2a spec, defect 1).
  //
  // The nondet functions ride along as criteria so that every read the
  // original program performs survives; the suite is generated on the slice
  // and replayed on the original (defect 2).
  //
  // --statistics: counts before and after, logged below.
  command << "timeout -k " << Map2Check::killGracePeriod << " "
          << static_cast<unsigned>(sliceBudget) << " " << slicer << " -c "
          << Map2Check::slicingCriteria(criteria)
          << " --entry=main -cutoff-diverging=false --statistics -o " << output
          << " " << input << " > slicer.output 2>&1";
```

Replace the success log:

```cpp
  Map2Check::Log::Info("Sliced with respect to " + targetFunction + ": " +
                       std::to_string(before) + " -> " +
                       std::to_string(after) + " bytes of bitcode");
```

with:

```cpp
  std::ifstream slicerLog("slicer.output");
  std::stringstream slicerText;
  slicerText << slicerLog.rdbuf();
  std::string criterionLabel;
  for (const std::string &name : criteria) {
    criterionLabel += (criterionLabel.empty() ? "" : ",") + name;
  }
  Map2Check::Log::Info(Map2Check::describeSlice(
      criterionLabel, Map2Check::parseSlicerStatistics(slicerText.str()),
      before, after));
```

Replace the stub text:

```cpp
      stub << "void __attribute__((weak)) " << targetFunction << "(void) {}\n";
```

with:

```cpp
      stub << Map2Check::targetStubSource(targetFunction);
```

In `modules/frontend/map2check.cpp`, change the one existing call so it still compiles and keeps today's behaviour for reachability:

```cpp
      caller->sliceWithRespectToTarget(args.function, {args.function});
```

Make sure `caller.cpp` includes `<sstream>` and `<fstream>` (it already uses `std::ostringstream` and `std::ifstream`; add whichever is missing).

- [ ] **Step 4: Rebuild and run the integration tests plus the unit tests**

```bash
docker run --rm -v $(pwd):/workspace map2check-dev:aflpp bash -c '
  cd /workspace/build_aflpp && ninja >/dev/null && ninja install >/dev/null &&
  cd /workspace && MAP2CHECK_PATH=/workspace/build_aflpp/install bash tests/integration/test_testcomp_regressions.sh 2>&1 | grep -E "FAIL|KLEE runs|read order|sliced with respect|Results"'
```
Expected: `PASS KLEE runs on the slice and reaches the target`, `PASS the sliced suite keeps the original read order [<a> 42 ]`, `PASS the program was sliced with respect to the target`, and `Results: 21 passed, 0 failed`.

```bash
docker run --rm -v $(pwd):/workspace map2check-dev:aflpp bash -c 'cd /workspace/build_ut && ninja >/dev/null && ctest 2>&1 | tail -3'
```
Expected: `100% tests passed`.

- [ ] **Step 5: Commit**

```bash
git add modules/frontend/caller.cpp modules/frontend/caller.hpp modules/frontend/map2check.cpp tests/integration/test_testcomp_regressions.sh
git commit -m "fix(tacasv2a): slice without cutoff and keep every nondet read

The cutoff's exit(0) carries no !dbg and KLEE rejected every sliced
module with a cut path; nondet reads the slicer dropped shifted the
suite on the original program. Both are integration-tested now, and the
slice is logged in functions/blocks/instructions, not only bytes.

Co-Authored-By: Claude Opus 5.5 (1M context) <noreply@anthropic.com>"
```

---

### Task 3: `--slice` in assert mode

**Files:**
- Modify: `modules/frontend/map2check.cpp`, the `if (args.sliceProgram)` block (around lines 519-528) and the `("slice", …)` help text (around lines 748-750)
- Test: `tests/integration/test_testcomp_regressions.sh`, section 12's refusal check, plus a new section 15 before the summary
- Modify: `CHANGELOG.md` (top entry)

**Interfaces:**
- Consumes: `Caller::sliceWithRespectToTarget(const std::string&, const std::vector<std::string>&)` from Task 2.
- Produces: in assert mode the log line is `Sliced with respect to __VERIFIER_assert,__assert_fail: …`, and the refusal message becomes `--slice applies to reachability and assert only`.

- [ ] **Step 1: Write the failing tests**

In section 12 of `tests/integration/test_testcomp_regressions.sh`, change the refusal check's grep from `"applies to reachability only"` to `"applies to reachability and assert only"`.

Insert before the summary lines (after section 14):

```bash
# --- 15. assert mode slices towards the assertions ----------------------------
# AssertPass instruments __VERIFIER_assert and __assert_fail, so those are the
# criteria. The program only DECLARES __VERIFIER_assert: the weak stub must
# take the condition, or llvm-link rejects the (void) definition.
mkdir -p "$WORK/assert"
cat > "$WORK/assert/assert.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int cond);
int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  if (a > 0) { a = a - 1; }
  __VERIFIER_assert(b != 77);
  return a;
}
EOF
( cd "$WORK/assert" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --check-asserts --slice --nondet-generator symex --timeout 45 assert.c ) \
  > "$WORK/assert/run.log" 2>&1
if grep -q "Sliced with respect to __VERIFIER_assert,__assert_fail" "$WORK/assert/run.log" && \
   grep -q "VERIFICATION FAILED" "$WORK/assert/run.log"; then
  ok "assert mode slices towards the assertions and still finds the violation"
else
  fail "assert slice" "no assert-criterion slice, or the violation was lost"
  grep -E "Sliced|slice|VERIFICATION" "$WORK/assert/run.log" | sed 's/^/    /'
fi
```

- [ ] **Step 2: Run them and confirm they fail**

```bash
docker run --rm -v $(pwd):/workspace map2check-dev:aflpp bash -c '
  cd /workspace && MAP2CHECK_PATH=/workspace/build_aflpp/install bash tests/integration/test_testcomp_regressions.sh 2>&1 | grep -E "FAIL|Results"'
```
Expected: `FAIL slice mode guard: …` (old message) and `FAIL assert slice: …`.

- [ ] **Step 3: Implement the gating**

Replace the block:

```cpp
  if (args.sliceProgram) {
    if (args.mode == Map2Check::Map2CheckMode::REACHABILITY_MODE) {
      caller->sliceWithRespectToTarget(args.function, {args.function});
    } else {
      Map2Check::Log::Warning(
          "--slice applies to reachability only: there is no criterion to "
          "slice towards when the goal is coverage or a memory property. "
          "Analysing the whole program.");
    }
  }
```

with:

```cpp
  // Reachability slices towards the target; assert towards the two functions
  // AssertPass instruments. Memory properties and overflow need their own
  // criteria (tacasv2b/2c); coverage has none -- every branch is the goal.
  if (args.sliceProgram) {
    if (args.mode == Map2Check::Map2CheckMode::REACHABILITY_MODE) {
      caller->sliceWithRespectToTarget(args.function, {args.function});
    } else if (args.mode == Map2Check::Map2CheckMode::ASSERT_MODE) {
      caller->sliceWithRespectToTarget("__VERIFIER_assert",
                                       {"__VERIFIER_assert", "__assert_fail"});
    } else {
      Map2Check::Log::Warning(
          "--slice applies to reachability and assert only: there is no "
          "criterion to slice towards when the goal is coverage or a memory "
          "or overflow property. Analysing the whole program.");
    }
  }
```

Replace the help text:

```cpp
        ("slice",
         "\tslice the program with respect to the target before analysing it "
         "(reachability only; needs sbt-slicer)")
```

with:

```cpp
        ("slice",
         "\tslice the program with respect to the target (reachability) or "
         "the assertions (--check-asserts) before analysing it; needs "
         "sbt-slicer")
```

Update the comment block right above `if (args.sliceProgram)` that ends with "Reachability only. Slicing needs a criterion, and Cover-Branches has none -- every branch is the goal. Asking elsewhere is refused, not ignored." so that its last paragraph reads: "Reachability and assert. Slicing needs a criterion, and Cover-Branches has none -- every branch is the goal. Asking elsewhere is refused, not ignored."

- [ ] **Step 4: Rebuild and run everything**

```bash
docker run --rm -v $(pwd):/workspace map2check-dev:aflpp bash -c '
  cd /workspace/build_aflpp && ninja >/dev/null && ninja install >/dev/null &&
  cd /workspace && MAP2CHECK_PATH=/workspace/build_aflpp/install bash tests/integration/test_testcomp_regressions.sh 2>&1 | grep -E "FAIL|assert mode|refused|Results"'
```
Expected: `PASS assert mode slices towards the assertions and still finds the violation`, `PASS --slice is refused where there is no criterion to slice towards`, and `Results: 22 passed, 0 failed`.

- [ ] **Step 5: CHANGELOG**

Add at the top of the unreleased section of `CHANGELOG.md`, following its existing format:

```markdown
- tacasv2a: `--slice` no longer crashes KLEE and its test suites stay valid
  on the original program. The slicer runs with `-cutoff-diverging=false`
  (the cutoff's `exit(0)` had no debug location and KLEE rejected the
  module), and every `__VERIFIER_nondet_*` function is a slicing criterion,
  so the read order is preserved. `--slice` now also works with
  `--check-asserts`. The slice is logged in functions/blocks/instructions.
```

- [ ] **Step 6: Commit**

```bash
git add modules/frontend/map2check.cpp tests/integration/test_testcomp_regressions.sh CHANGELOG.md
git commit -m "feat(tacasv2a): --slice in assert mode

Co-Authored-By: Claude Opus 5.5 (1M context) <noreply@anthropic.com>"
```

---

### Task 4: Validation on the diagnostic sample and the CI gates

**Files:** none changed (verification only). If a check fails, fix it in the task that owns the code and re-run.

**Interfaces:**
- Consumes: the full build from Tasks 1-3.

- [ ] **Step 1: Re-run the 12-task diagnostic sample on the finished build**

The manifest is the one from the spec's §2 (the 12 programs listed there, TSV columns `category program data_model expected_unreach`). Run both arms with the same build, `BUDGET=300 TESTCOV_S=300`, with `tests/testcomp/run_testcomp_evaluation.sh` (`PROPERTY=cover-error`, `EXTRA_FLAGS=""` for control and `EXTRA_FLAGS="--slice"` for the slice arm, `RESULTS_DIR` outside the repository). The container needs `python3-pip zip gcc gcc-multilib lcov` and `pip3 install testcov`.
Expected: the slice arm covers at least 10/12, and `ntdrivers/floppy.i.cil-1.c` is `FAILED,COVERED`.

- [ ] **Step 2: Run the CI's TestCov step in the dev image**

```bash
docker run --rm -u root -v $(pwd):/workspace -w /workspace -e MAP2CHECK_PATH=/workspace/build_aflpp/install map2check-dev:aflpp bash -c '
  apt-get update -qq && apt-get install -y -qq python3-pip zip gcc lcov >/dev/null && pip3 install --quiet testcov &&
  python3 tests/integration/test_benchexec_toolinfo.py 2>&1 | grep Results &&
  bash tests/integration/test_cover_branches.sh 2>&1 | grep Results &&
  bash tests/testcomp/run_testcov_suite.sh 2>&1 | grep -E "FAIL|Results"'
```
Expected: `16 passed`, `5 passed`, `6 passed, 0 failed`.

- [ ] **Step 3: Record the result**

Append the sample table to the spec's §2 as "after implementation", then commit:

```bash
git add docs/superpowers/specs/2026-09-26-tacasv2a-slicing-reach-assert-design.md
git commit -m "docs(tacasv2a): diagnostic sample re-run on the implementation

Co-Authored-By: Claude Opus 5.5 (1M context) <noreply@anthropic.com>"
```
