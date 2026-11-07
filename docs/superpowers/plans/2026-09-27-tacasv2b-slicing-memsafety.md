# tacasv2b — Slicing for memtrack/memcleanup: Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** make `--slice` work with `--memtrack` and `--memcleanup-property`, by slicing the instrumented module with every runtime call as a criterion. Also add an SV-COMP MemSafety evaluation harness, and let the CASTLE and Juliet runners take extra flags.

**Architecture:**
- **Shared slicer helper.** The slicer invocation is factored out of `sliceWithRespectToTarget` into a private `Caller::runSlicer` helper. It handles disassembly, criteria, budget, statistics and fallback.
- **New `Caller::sliceInstrumented()`.** It uses the helper on `<hash>-output.bc`, between `callPass` and `linkLLVM`. The criteria are every `@map2check_*` symbol plus every nondet function, and the entry is `__map2check_main__`.
- **Harness.** It reuses `build_corpus.py`, with directory-based categories for memory, plus a new runner and a tested classifier.

**Tech Stack:** C++17, LLVM 16, sbt-slicer, GTest, bash, python3. Build and run everything in `map2check-dev:aflpp`. Build dirs:
- `build_aflpp`, install prefix pinned to `/workspace/build_aflpp/install`;
- `build_aflpp_ut`, with `ENABLE_TEST=ON`.

**Spec:** `docs/superpowers/specs/2026-09-27-tacasv2b-slicing-memsafety-design.md`

## Global Constraints

- Branch `feat/tacas-slicing-mem` (from `feat/tacas-slicing`). Baseline tacasv1; nothing targets LibFuzzer.
- Memory criteria: every `@map2check_*` symbol in the instrumented IR, plus the fixed nondet list, plus the nondet names found in the IR.
- Slicer flags: `--entry=__map2check_main__ -cutoff-diverging=false --statistics`.
- Slicing order: for reach/assert, before `callPass` (unchanged); for memtrack/memcleanup, after `callPass` and before `linkLLVM`. Overflow and cover-branches are still refused.
- Refusal message: `--slice applies to reachability, assert and memory properties only`.
- On any slicer failure, fall back to the unsliced module with a warning. Never abort the run.
- Every mini-round is appended to `docs/reports/tacas-experiment-log.md` and compared with the previous one (standing rule).
- Commits end with `Co-Authored-By: Claude Opus 5.5 (1M context) <noreply@anthropic.com>`.

## Review Focus

- **Instrumented modules that call a runtime function only through a declaration never called.** The criterion is harmless (verified in 2a); nothing to pin beyond the unit test.
- **A VLA/`llvm.stacksave` program.** The slicer errors, the run must fall back and keep the unsliced verdict. Pinned in Task 2 (integration test 20).
- **A safe program must never become FALSE because of slicing** (a spurious violation). Pinned in Task 2 (test 19).
- **A FALSE with the wrong subproperty** (e.g. FALSE-FREE on a valid-deref task) must count as `wrong-false`, not correct. Pinned in Task 3 (classifier tests).
- **Rerunning an evaluation with an existing CSV must resume, not duplicate rows.** Pinned in Task 3 (the runner copies the resumable pattern; checked in its smoke step).

---

## File Structure

- Modify `modules/frontend/utils/slicer.hpp`: add `runtimeNamesInIR()`.
- Modify `tests/unit/frontend/SlicerTest.cpp`: add tests for it.
- Modify `modules/frontend/caller.hpp` / `caller.cpp`: add `runSlicer` (private) and `sliceInstrumented` (public), and refactor `sliceWithRespectToTarget` onto `runSlicer`.
- Modify `modules/frontend/map2check.cpp`: gating, and the post-`callPass` hook.
- Modify `tests/integration/test_testcomp_regressions.sh`: sections 17–21, plus the section-12 refusal moved to overflow.
- Modify `tests/testcomp/build_corpus.py`: add the `memsafety` and `memcleanup` properties, directory categories, and expected verdict/subproperty.
- Create `tests/lib/memsafety_classifier.sh` with `classify_memsafety_result`.
- Create `tests/integration/test_memsafety_classifier.sh` with the classifier table tests.
- Create `tests/memsafety/run_memsafety_evaluation.sh`, the new runner.
- Modify `tests/castle/run_castle_evaluation.sh` and `tests/juliet/run_juliet_evaluation.sh` to accept `EXTRA_FLAGS`.

---

### Task 1: `runtimeNamesInIR`

**Files:** Modify `modules/frontend/utils/slicer.hpp`, `tests/unit/frontend/SlicerTest.cpp`

**Interfaces:** Produces `std::vector<std::string> Map2Check::runtimeNamesInIR(const std::string& ir);`, which returns every `@map2check_[A-Za-z0-9_]+`, in first-appearance order, without duplicates.

- [ ] **Step 1: Failing test.** Append to `SlicerTest.cpp`:

```cpp
// After instrumentation the memory property lives in the runtime calls
// MemoryTrackPass inserted; every one of them is a criterion, so nothing that
// records memory is sliced away.
TEST(RuntimeNamesInIR, FindsEveryMap2checkSymbolOnce) {
  const std::string ir =
      "declare void @map2check_malloc(ptr, i64)\n"
      "  call void @map2check_check_deref(ptr %3, i64 4), !dbg !7\n"
      "  call void @map2check_malloc(ptr %1, i64 8)\n"
      "  call i32 @__VERIFIER_nondet_int()\n";
  const std::vector<std::string> names = Map2Check::runtimeNamesInIR(ir);
  ASSERT_EQ(names.size(), 2u);
  EXPECT_EQ(names[0], "map2check_malloc");
  EXPECT_EQ(names[1], "map2check_check_deref");
}
```

- [ ] **Step 2: Run it and confirm it fails** (`ninja SlicerTest` in `build_aflpp_ut`). Expected: `no member named 'runtimeNamesInIR'`.
- [ ] **Step 3: Implement** it in `slicer.hpp`, next to `nondetNamesInIR`:

```cpp
/** Every map2check_* runtime symbol in a module's textual IR, in order of
 * first appearance. Used as the slicing criteria for the memory properties:
 * the property is decided by these calls, so none of them may be removed. */
inline std::vector<std::string> runtimeNamesInIR(const std::string& ir) {
  static const std::regex symbol(R"(@(map2check_[A-Za-z0-9_]+))");
  std::vector<std::string> names;
  for (std::sregex_iterator it(ir.begin(), ir.end(), symbol), end; it != end;
       ++it) {
    const std::string name = (*it)[1];
    if (std::find(names.begin(), names.end(), name) == names.end()) {
      names.push_back(name);
    }
  }
  return names;
}
```

- [ ] **Step 4: Run SlicerTest.** Expected: `[  PASSED  ] 12 tests.`
- [ ] **Step 5: Commit** `feat(tacasv2b): runtime names as slicing criteria`.

---

### Task 2: Slice the instrumented module in the memory modes

**Files:** Modify `caller.hpp`, `caller.cpp`, `map2check.cpp`, `tests/integration/test_testcomp_regressions.sh`

**Interfaces:**
- Consumes: `runtimeNamesInIR`, `nondetNamesInIR`, `slicingCriteria`, `parseSlicerStatistics` and `describeSlice`.
- Produces:
  - `bool Caller::sliceInstrumented();` (public);
  - `bool Caller::runSlicer(const std::string& input, const std::string& output, std::vector<std::string> primary, bool addRuntimeNames, const std::string& entry, const std::string& label);` (private).
  - Log prefix `Sliced with respect to map2check runtime`.

- [ ] **Step 1: Failing integration tests.**
  - In section 12, change the refusal run from `--memtrack` to `--check-overflow`, and the grep to `"applies to reachability, assert and memory properties only"`.
  - Insert before the summary lines the sections below. Each program is written with a `cat > … <<'EOF'` heredoc, as in the existing sections.

```bash
# --- 17. memtrack slices the instrumented module and keeps the violation -----
# Memory has no criterion in the user's program: the property is decided by the
# runtime calls MemoryTrackPass inserts, so the slice is taken AFTER
# instrumentation with every map2check_* call as a criterion.
mkdir -p "$WORK/mem"
cat > "$WORK/mem/dfree.c" <<'EOF'
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  int unrelated = 0;
  for (int i = 0; i < 4; i++) { unrelated += i; }
  int *p = malloc(sizeof(int));
  free(p);
  if (n == 11) { free(p); }
  return unrelated;
}
EOF
( cd "$WORK/mem" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memtrack --nondet-generator symex --timeout 45 dfree.c ) > "$WORK/mem/plain.log" 2>&1
( cd "$WORK/mem" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memtrack --slice --nondet-generator symex --timeout 45 dfree.c ) > "$WORK/mem/slice.log" 2>&1
plain_v=$(grep -oE "FALSE-[A-Z]+" "$WORK/mem/plain.log" | tail -1)
slice_v=$(grep -oE "FALSE-[A-Z]+" "$WORK/mem/slice.log" | tail -1)
if grep -q "Sliced with respect to map2check runtime" "$WORK/mem/slice.log" && \
   grep -q "VERIFICATION FAILED" "$WORK/mem/slice.log" && [ -n "$slice_v" ] && \
   [ "$slice_v" = "$plain_v" ]; then
  ok "memtrack slices after instrumentation and keeps the violation ($slice_v)"
else
  fail "memtrack slice" "plain=[$plain_v] slice=[$slice_v]"
  grep -E "Sliced|slice|VERIFICATION" "$WORK/mem/slice.log" | sed 's/^/    /'
fi

# --- 18. memcleanup slices too and still sees the leak ----------------------
cat > "$WORK/mem/leak.c" <<'EOF'
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  int *p = malloc(sizeof(int));
  if (n == 5) { return 0; }
  free(p);
  return 0;
}
EOF
( cd "$WORK/mem" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memcleanup-property --slice --nondet-generator symex --timeout 45 leak.c ) \
  > "$WORK/mem/leak.log" 2>&1
if grep -q "Sliced with respect to map2check runtime" "$WORK/mem/leak.log" && \
   grep -q "VERIFICATION FAILED" "$WORK/mem/leak.log"; then
  ok "memcleanup slices and still finds the leak"
else
  fail "memcleanup slice" "no slice, or the leak was lost"
  grep -E "Sliced|slice|VERIFICATION" "$WORK/mem/leak.log" | sed 's/^/    /'
fi

# --- 19. slicing must not invent a memory violation --------------------------
cat > "$WORK/mem/safe.c" <<'EOF'
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  int *p = malloc(sizeof(int));
  if (p == 0) { return 0; }
  *p = n;
  if (n == 11) { *p = 0; }
  free(p);
  return 0;
}
EOF
( cd "$WORK/mem" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memtrack --slice --nondet-generator symex --timeout 45 safe.c ) > "$WORK/mem/safe.log" 2>&1
if grep -q "VERIFICATION FAILED" "$WORK/mem/safe.log"; then
  fail "slice soundness" "a safe program was reported FALSE after slicing"
else
  ok "slicing does not invent a memory violation"
fi

# --- 20. a construct the slicer rejects falls back, loudly -------------------
# sbt-slicer errors on llvm.stacksave (variable-length arrays). The run must
# fall back to the unsliced module and reach the same verdict.
cat > "$WORK/mem/vla.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  if (n > 0 && n < 10) {
    int a[n];
    a[n] = 1;
    return a[0];
  }
  return 0;
}
EOF
( cd "$WORK/mem" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memtrack --nondet-generator symex --timeout 45 vla.c ) > "$WORK/mem/vla-plain.log" 2>&1
( cd "$WORK/mem" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memtrack --slice --nondet-generator symex --timeout 45 vla.c ) > "$WORK/mem/vla.log" 2>&1
vla_plain=$(grep -oE "VERIFICATION [A-Z]+" "$WORK/mem/vla-plain.log" | tail -1)
vla_slice=$(grep -oE "VERIFICATION [A-Z]+" "$WORK/mem/vla.log" | tail -1)
if { grep -q "Sliced with respect to map2check runtime" "$WORK/mem/vla.log" || \
     grep -q "analysing the unsliced program" "$WORK/mem/vla.log"; } && \
   [ -n "$vla_slice" ] && [ "$vla_slice" = "$vla_plain" ]; then
  ok "a slicer failure falls back and keeps the verdict ($vla_slice)"
else
  fail "slice fallback" "plain=[$vla_plain] slice=[$vla_slice]"
fi
```

  Run the suite. Expected FAILs: `slice mode guard` (old message), and `memtrack slice` and `memcleanup slice` (no slice). Test 19 passes before the fix, trivially (the slice is refused); it is a regression guard, not a RED test. Test 20 passes before the fix too (no slice: same verdict), for the same reason. Ledger both.

- [ ] **Step 2: Implement.**
  - **Extract `runSlicer`** from `sliceWithRespectToTarget`. It covers the lines from the `slicer` existence check to the `describeSlice` log, parameterized by `input`, `output`, `primary`, `addRuntimeNames`, `entry` and `label`. It returns `false` on a missing slicer or on no usable output (keeping the existing warnings), and `true` after logging. When `addRuntimeNames` is set, the runtime names are appended to `primary` before `slicingCriteria(primary, programNondets)`. The IR file becomes `input + ".ll"`, so the two call sites do not collide.
  - **`sliceWithRespectToTarget`** becomes: `if (!runSlicer(programHash + "-compiled.bc", programHash + "-sliced.bc", criteria, false, "main", joined(criteria))) return false;` followed by the existing stub-and-rename code.
  - **`sliceInstrumented()`:**

```cpp
bool Caller::sliceInstrumented() {
  // Memory properties have no criterion in the user's program: the property
  // is decided by the runtime calls MemoryTrackPass inserted, so the slice is
  // taken after instrumentation with every map2check_* call as a criterion
  // (none can be dropped), and __map2check_main__ -- the renamed user main --
  // as the entry. tacasv2b spec, section 2.
  const std::string input = programHash + "-output.bc";
  const std::string output = programHash + "-sliced-instrumented.bc";
  if (!std::filesystem::exists(input)) return false;
  if (!runSlicer(input, output, {}, true, "__map2check_main__",
                 "map2check runtime")) {
    return false;
  }
  std::error_code error;
  std::filesystem::rename(output, input, error);
  return !error;
}
```

  - **In `map2check.cpp`:**
    - In the pre-`callPass` block, the memory modes fall through without a warning.
    - The warning text changes to `"--slice applies to reachability, assert and memory properties only: there is no criterion to slice towards when the goal is coverage or overflow. Analysing the whole program."`.
    - After `caller->callPass(args.function);` insert:

```cpp
  // Memory properties slice the INSTRUMENTED module (see sliceInstrumented).
  if (args.sliceProgram &&
      (args.mode == Map2Check::Map2CheckMode::MEMTRACK_MODE ||
       args.mode == Map2Check::Map2CheckMode::MEMCLEANUP_MODE)) {
    caller->sliceInstrumented();
  }
```

  - Update the `--slice` help text to mention `--memtrack` and `--memcleanup-property`.
- [ ] **Step 3: Rebuild and run the integration suite and ctest.** Expected: `Results: 27 passed, 0 failed`, ctest 100%.
- [ ] **Step 4: Commit** `feat(tacasv2b): --slice for memtrack and memcleanup, after instrumentation`.

---

### Task 3: SV-COMP MemSafety harness

**Files:**
- Modify `tests/testcomp/build_corpus.py`.
- Create `tests/lib/memsafety_classifier.sh`, `tests/integration/test_memsafety_classifier.sh` and `tests/memsafety/run_memsafety_evaluation.sh`.

**Interfaces:**
- Produces `classify_memsafety_result <expected> <subproperty> <verdict>`, which prints one of `correct-true correct-false wrong-true wrong-false unknown error`. `<verdict>` is the output of `classify_map2check_verdict`.
- Produces manifests with the columns `category program data_model expected subproperty`.

- [ ] **Step 1: Failing classifier test.** Create `tests/integration/test_memsafety_classifier.sh`:

```bash
#!/bin/bash
# Table test for classify_memsafety_result: a FALSE only counts when its kind
# matches the task's subproperty; a TRUE on a false task is the dangerous error.
set -u
. "$(dirname "$0")/../lib/memsafety_classifier.sh"
PASSED=0; FAILED=0
check() {  # expected subproperty verdict want
  got=$(classify_memsafety_result "$1" "$2" "$3")
  if [ "$got" = "$4" ]; then PASSED=$((PASSED+1)); else FAILED=$((FAILED+1)); echo "  FAIL $1/$2/$3: want $4 got $got"; fi
}
check true  ""               TRUE            correct-true
check true  ""               FALSE-DEREF     wrong-false
check false valid-deref      FALSE-DEREF     correct-false
check false valid-free       FALSE-FREE      correct-false
check false valid-memtrack   FALSE-MEMTRACK  correct-false
check false valid-memcleanup FALSE-MEMCLEANUP correct-false
check false valid-deref      FALSE-FREE      wrong-false
check false valid-deref      TRUE            wrong-true
check false valid-deref      UNKNOWN         unknown
check false valid-deref      TIMEOUT         unknown
check true  ""               ERROR           error
echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ]
```

  Run `bash tests/integration/test_memsafety_classifier.sh`. Expected: fails, because `memsafety_classifier.sh` does not exist.
- [ ] **Step 2: Implement** `tests/lib/memsafety_classifier.sh`:

```bash
# shellcheck shell=bash
# classify_memsafety_result <expected true|false> <subproperty> <verdict>
# <verdict> is classify_map2check_verdict's output. A FALSE counts as correct
# only when its kind matches the subproperty the task declares; FALSE of the
# wrong kind is a wrong answer, not a lucky one.
classify_memsafety_result() {
  local expected="$1" sub="$2" verdict="$3" want=""
  case "$sub" in
    valid-deref) want="FALSE-DEREF" ;;
    valid-free) want="FALSE-FREE" ;;
    valid-memtrack) want="FALSE-MEMTRACK" ;;
    valid-memcleanup) want="FALSE-MEMCLEANUP" ;;
  esac
  case "$verdict" in
    TRUE) [ "$expected" = "true" ] && echo correct-true || echo wrong-true ;;
    FALSE*) if [ "$expected" = "false" ] && [ "$verdict" = "$want" ]; then
              echo correct-false; else echo wrong-false; fi ;;
    ERROR) echo error ;;
    *) echo unknown ;;
  esac
}
```

  Run the test. Expected: `Results: 11 passed, 0 failed`.
- [ ] **Step 3: `build_corpus.py`.**
  - Add `"memsafety": "valid-memsafety.prp"` and `"memcleanup": "valid-memcleanup.prp"` to `PROPERTY_FILE`.
  - Add `MEMORY_CATEGORIES`, a dict from category to directory list. For `memsafety`: Arrays, Heap, LinkedLists, Other and Juliet, exactly as spec §3.4. For `memcleanup`: one category, `MemCleanup`, covering every directory. The `memcleanup` expansion globs `*/*.yml` and filters by property.
  - In `main`, when the property is `memsafety` or `memcleanup`, expand categories with `glob(BENCH/<dir>/*.yml)` instead of `expand_set`.
  - Generalize `task_info` so that, for the wanted property, it captures `expected_verdict` and `subproperty` (the lines after the matching `- property_file:`), and returns them as a 4th value.
  - For memory properties, write the header `# category\tprogram\tdata_model\texpected\tsubproperty` and 5 columns. The cover-* outputs stay unchanged.
  - Smoke: `python3 tests/testcomp/build_corpus.py --property memsafety --per-category 3 --out /tmp/ms.tsv`. Expected: a summary listing Arrays/Heap/LinkedLists/Other/Juliet with non-zero "applicable", and 15 rows.
- [ ] **Step 4: `tests/memsafety/run_memsafety_evaluation.sh`.**
  - Copy the structure of `tests/testcomp/run_testcomp_evaluation.sh`: the env vars `MANIFEST PROPERTY RESULTS_DIR SHARD SHARDS BUDGET DEADLINE_S EXTRA_FLAGS MAP2CHECK_PATH`, the resumable CSV, fd 3, and a private work dir per task.
  - Differences:
    - The mode flag is `--memtrack` for `memsafety` and `--memcleanup-property` for `memcleanup`. There is no TestCov.
    - The raw verdict comes from `classify_map2check_verdict "$output" "$rc" "$elapsed" "$BUDGET"` (source `tests/lib/verdict_classifier.sh`).
    - The class comes from `classify_memsafety_result "$expected" "$subproperty" "$verdict"` (source `tests/lib/memsafety_classifier.sh`).
    - The slice statistics come from the log (`grep -o "Sliced with respect to.*"`).
    - The CSV header is `category,program,data_model,expected,subproperty,verdict,class,elapsed_s,slice`.
  - Smoke: run it on `/tmp/ms.tsv` with `BUDGET=30`, twice. Expected: 15 rows after the first run, still 15 after the second (resume).
- [ ] **Step 5: Commit** `test(tacasv2b): SV-COMP MemSafety evaluation harness`.

---

### Task 4: `EXTRA_FLAGS` in the CASTLE and Juliet runners

**Files:** Modify `tests/castle/run_castle_evaluation.sh`, `tests/juliet/run_juliet_evaluation.sh`

- [ ] **Step 1:** In each runner, next to the other env defaults, add `EXTRA_FLAGS="${EXTRA_FLAGS:-}"`, with a comment that it holds opt-in flags such as `--slice` appended to every run. Append `$EXTRA_FLAGS` to the map2check invocation, right after the mode flags. In CASTLE that is both invocations (lines ~174 and ~199); in Juliet, the equivalent call.
- [ ] **Step 2:** `bash -n` both files. Then run a 2-task CASTLE smoke with `EXTRA_FLAGS=--slice` and `RESULTS_DIR` in the scratchpad (use whatever the runner offers to limit tasks; if nothing, a temporary copy of the dataset list with 2 entries). Expected: the raw logs show `Sliced with respect to`.
- [ ] **Step 3: Commit** `test(tacasv2b): opt-in flags for the CASTLE and Juliet runners`.

---

### Task 5: Mini-rounds R7–R9 and the log

- [ ] **R7, CASTLE:** the full 250, `EXTRA_FLAGS=""` and `EXTRA_FLAGS=--slice`, same build, separate `RESULTS_DIR` in the scratchpad. Tally TP/TN/FP/FN/UNKNOWN/TIMEOUT per arm, and compare with v15 (`tests/castle/results_v15`: TP 54, FN 14, FP 1).
- [ ] **R8, SV-COMP MemSafety:** 10 per category (`--per-category 10`), both arms, `BUDGET=120`. Tally the classes per arm and per category; report the median time and the slice reduction.
- [ ] **R9, Juliet scope C:** a small per-CWE sample (use the runner's sampling options; if none, 5 per CWE), both arms.
- [ ] **Append R7, R8 and R9** to `docs/reports/tacas-experiment-log.md`. For each: data, comparison with the previous round and with the other arm, and the reading, with **wrong-true and wrong-false called out explicitly**. Commit `docs(tacas): R7-R9 -- memory slicing mini-rounds`.
