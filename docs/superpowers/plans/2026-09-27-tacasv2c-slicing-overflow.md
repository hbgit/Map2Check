# tacasv2c — Slicing for overflow: Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** make `--slice` work with `--check-overflow` by reusing `Caller::sliceInstrumented()` from 2b. Also extend the MemSafety harness to SV-COMP NoOverflows.

**Architecture:**
- Overflow checks are `map2check_binop_*` runtime calls, so the post-instrumentation slice from 2b already covers them.
- This plan changes only three things:
  - the gating in `map2check.cpp`;
  - the harness (the corpus property and the runner mode);
  - the classifier row.

**Tech Stack:** C++17, bash, python3, sbt-slicer. Work in the worktree `../Map2Check-2c`, branch `feat/tacas-slicing-overflow`. Build in `build_2c`, installing to `build_2c/install`. `build_aflpp` must not be touched while R8–R10 run from the main checkout.

**Spec:** `docs/superpowers/specs/2026-09-27-tacasv2c-slicing-overflow-design.md`

## Global Constraints

- Criteria: every `@map2check_*` plus the nondets, `--entry=__map2check_main__`, `-cutoff-diverging=false` (unchanged from 2b).
- `--slice` covers reach, assert, memtrack, memcleanup and overflow. Only cover-branches is refused.
- Refusal message: `--slice applies to reachability, assert, memory and overflow properties only`.
- Every mini-round is logged in `docs/reports/tacas-experiment-log.md`.

## Review Focus

- **A signed overflow whose operands come through unsigned or pointer arithmetic.** The data dependences of the check call's operands must keep them. Covered by the dg semantics; the R11–R13 wrong-* counts are the measure.
- **A safe program must not become FALSE-OVERFLOW after slicing.** Pinned by integration test 22.
- **NoOverflows tasks with `expected_verdict: false`.** The classifier must require FALSE-OVERFLOW; any other FALSE is wrong. Pinned by the classifier rows.

---

### Task 1: Gating plus integration tests

**Files:** `modules/frontend/map2check.cpp`, `tests/integration/test_testcomp_regressions.sh`

- [ ] **Step 1: Failing tests.**
  - In section 12, change the refusal run from `--check-overflow` to `--cover-branches`, and the grep to `"applies to reachability, assert, memory and overflow properties only"`.
  - Add, before the summary lines:

```bash
# --- 21. overflow slices the instrumented module and keeps the violation -----
# Overflow checks are map2check_binop_* runtime calls, so the same
# post-instrumentation slice as the memory properties applies.
mkdir -p "$WORK/ovf"
cat > "$WORK/ovf/ovf.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int unrelated = 0;
  for (int i = 0; i < 4; i++) { unrelated += i; }
  if (x > 2147483000) { x = x + 1000; }
  return x + unrelated;
}
EOF
( cd "$WORK/ovf" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --check-overflow --slice --nondet-generator symex --timeout 45 ovf.c ) > "$WORK/ovf/run.log" 2>&1
if grep -q "Sliced with respect to map2check runtime" "$WORK/ovf/run.log" && \
   grep -q "VERIFICATION FAILED" "$WORK/ovf/run.log"; then
  ok "overflow slices after instrumentation and keeps the violation"
else
  fail "overflow slice" "no slice, or the overflow was lost"
  grep -E "Sliced|slice|VERIFICATION" "$WORK/ovf/run.log" | sed 's/^/    /'
fi

# --- 22. slicing must not invent an overflow ---------------------------------
cat > "$WORK/ovf/safe.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 0 && x < 1000) { x = x + 1000; }
  return x;
}
EOF
( cd "$WORK/ovf" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --check-overflow --slice --nondet-generator symex --timeout 45 safe.c ) > "$WORK/ovf/safe.log" 2>&1
if grep -q "VERIFICATION FAILED" "$WORK/ovf/safe.log"; then
  fail "overflow slice soundness" "a safe program was reported FALSE after slicing"
else
  ok "slicing does not invent an overflow"
fi
```

  Expected before the fix: `slice mode guard` and `overflow slice` FAIL. Test 22 passes trivially today; ledger it as a regression guard.
- [ ] **Step 2: Implement.**
  - In `map2check.cpp`, the pre-`callPass` warning condition also excludes `OVERFLOW_MODE`, and its text becomes `"--slice applies to reachability, assert, memory and overflow properties only: there is no criterion to slice towards when the goal is branch coverage. Analysing the whole program."`.
  - The post-`callPass` hook condition adds `|| args.mode == Map2Check::Map2CheckMode::OVERFLOW_MODE`, and its comment names overflow.
  - The `--slice` help text mentions `--check-overflow`.
- [ ] **Step 3:** Build `build_2c` (fresh: `cmake .. -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm -DCMAKE_INSTALL_PREFIX=/workspace/build_2c/install`; symlink `install/lib/klee/runtime` and `install/lib/clang` as the CI does). Run the integration suite against `build_2c/install`. Expected: `Results: 29 passed, 0 failed`.
- [ ] **Step 4: Commit** `feat(tacasv2c): --slice for overflow`.

### Task 2: NoOverflows harness

**Files:** `tests/testcomp/build_corpus.py`, `tests/memsafety/run_memsafety_evaluation.sh`, `tests/lib/memsafety_classifier.sh`, `tests/integration/test_memsafety_classifier.sh`

- [ ] **Step 1: Failing classifier rows.** Append to the test:
```bash
check false no-overflow      FALSE-OVERFLOW  correct-false
check false no-overflow      FALSE-DEREF     wrong-false
check true  ""               FALSE-OVERFLOW  wrong-false
```
  Run it. Expected: the first row FAILs (`want correct-false got wrong-false`).
- [ ] **Step 2:** In the classifier, add `no-overflow) want="FALSE-OVERFLOW" ;;`. Run it. Expected: `14 passed`.
- [ ] **Step 3:** In `build_corpus.py`:
  - add `"overflow": "no-overflow.prp"` to `PROPERTY_FILE`;
  - add `MEMORY_CATEGORIES["overflow"] = {"Main": ["bitvector", "nla-digbench-scaling", "recursive-simple", "loop-zilu", "signedintegeroverflow-regression", "goblint-regression"], "BusyBox": ["busybox-1.22.0"], "Juliet": ["Juliet_Test"]}`;
  - when an overflow task is false and has no subproperty, the subproperty is `no-overflow` (same rule as memcleanup).

  Smoke: `--property overflow --per-category 3`. Expected: 9 rows; the false rows carry `no-overflow`.
- [ ] **Step 4:** In the runner, add `overflow) MODE_FLAGS="--check-overflow" ;;` to the PROPERTY case, and update its header comment. Run `bash -n`.
- [ ] **Step 5: Commit** `test(tacasv2c): NoOverflows harness`.

### Task 3: Mini-rounds R11–R13 and the log

Run each round in the `build_2c` install, both arms (`EXTRA_FLAGS=""` and `--slice`).

- [ ] **R11, CASTLE CWE-190:** the full CASTLE runner. Report the `--check-overflow` rows only, and compare with v15 (overflow TP 10, FN 2).
- [ ] **R12, Juliet CWE-190/191:** `JULIET_CWES="190 191"`, `PER_FAMILY=1`.
- [ ] **R13, SV-COMP NoOverflows:** `--per-category 10`, `BUDGET=120`.
- [ ] **Append R11–R13 to the log**, with data, a comparison against R7 and the 2b rounds, and wrong-* called out. Commit.
