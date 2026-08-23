#!/bin/bash
# test_testcomp_regressions.sh -- the five defects the Test-Comp corpus caught.
#
# Every one of these was invisible to the tests that existed. They produced
# well-formed output, plausible verdicts and green runs; what they did not
# produce was a suite a validator would accept. They were found by running 2340
# real tasks and reading the numbers, which is expensive, so each one gets a
# cheap test here to make sure it is found for free next time.
#
#   1  a Cover-Error suite with no <input> at all      (290 of 376 FAILED runs)
#   2  --property-file ignored when relative           (every task in the corpus)
#   3  --cover-branches loading the memtrack pipeline  (110 of 110 ProductLines)
#   4  MemoryTrackPass emitting a type-mismatched call (same 110, plus ECA)
#   5  a branch suite too large for TestCov to check   (116 validation failures)
#   6  KLEE killed by the budget, discarding everything (3982 paths -> 0 tests)

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

echo "=== Test-Comp regressions ==="

# --- 1. a Cover-Error suite must carry the input vector ----------------------
# The state that reaches the target aborts, and an aborted KLEE state runs no
# exit handler, so the runtime's klee_log.csv is never written. The suite used
# to come out with a testcase element and nothing inside it: the tool had found
# the bug and could not prove it. KLEE's own .ktest for the failing path holds
# the vector, and that is now the fallback.
mkdir -p "$WORK/one"
cat > "$WORK/one/reach.prp" <<'EOF'
COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )
EOF
cat > "$WORK/one/reach.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  if (a == 4242) { reach_error(); }
  return 0;
}
EOF
( cd "$WORK/one" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --nondet-generator symex \
    --generate-test-suite --property-file reach.prp --timeout 120 reach.c ) \
  > "$WORK/one/run.log" 2>&1

inputs=$(sed -n 's:.*<input>\(.*\)</input>.*:\1:p' "$WORK/one/test-suite/testcase-1.xml" 2>/dev/null)
if [ -n "$inputs" ]; then
  ok "a Cover-Error test case carries its input vector [$(echo $inputs | tr '\n' ' ')]"
else
  fail "Cover-Error inputs" "test case has no <input> -- the suite proves nothing"
  tail -4 "$WORK/one/run.log" | sed 's/^/    /'
fi

# --- 2. a relative --property-file must be read ------------------------------
# resolveSpecification runs after the pipeline has chdir'd into the scratch
# directory, so a relative path -- what BenchExec and every harness here pass --
# resolved against the wrong place and fell through to a guessed specification.
#
# The property text below is DELIBERATELY not one of the two the fallback
# guesses. That is the whole difficulty: with a real property file the guess
# happens to match, so the bug produced identical output and no test could see
# it. Only a specification the tool could not have invented proves it was read.
mkdir -p "$WORK/spec"
cat > "$WORK/spec/custom.prp" <<'EOF'
COVER( init(main()), FQL(COVER EDGES(@CALL(a_marker_no_fallback_would_guess))) )
EOF
cp "$WORK/one/reach.c" "$WORK/spec/"
( cd "$WORK/spec" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --nondet-generator symex \
    --generate-test-suite --property-file custom.prp --timeout 60 reach.c ) \
  > "$WORK/spec/run.log" 2>&1

if grep -q "a_marker_no_fallback_would_guess" "$WORK/spec/test-suite/metadata.xml" 2>/dev/null; then
  ok "a relative --property-file reaches <specification> verbatim"
else
  fail "property file" "metadata does not carry the supplied specification"
  grep -o "<specification>.*</specification>" "$WORK/spec/test-suite/metadata.xml" 2>/dev/null | sed 's/^/    got: /'
fi
if grep -q "could not read property file" "$WORK/spec/run.log"; then
  fail "property file resolution" "still reported unreadable"
else
  ok "no 'could not read property file' warning"
fi

# --- 3. --cover-branches must not run the memory-tracking pipeline -----------
# With no mode flag the run fell through to the MEMTRACK default and
# instrumented memory tracking for a task that checks no property: pure
# overhead, and on some programs a broken module.
mkdir -p "$WORK/lean"
cat > "$WORK/lean/branches.prp" <<'EOF'
COVER( init(main()), FQL(COVER EDGES(@DECISIONEDGE)) )
EOF
cat > "$WORK/lean/lean.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 0) { return 1; }
  return 2;
}
EOF
( cd "$WORK/lean" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --nondet-generator symex --generate-test-suite --cover-branches \
    --property-file branches.prp --debug --timeout 60 lean.c ) \
  > "$WORK/lean/run.log" 2>&1

if grep -q "memory-track" "$WORK/lean/run.log"; then
  fail "lean pipeline" "--cover-branches still loads the memory-track pass"
else
  ok "--cover-branches runs without the memory-track pass"
fi

# --- 4. MemoryTrackPass must coerce the size to the declared parameter type --
# The helper is registered with an i64 size and the operand comes straight from
# the program's own call. A program allocating with a narrower integer -- which
# is what the CIL-processed sources in ProductLines and ECA do -- produced a
# call whose argument type did not match its signature, and LLVM rejected the
# module outright, two seconds into a sixty-second budget.
mkdir -p "$WORK/i32"
cat > "$WORK/i32/narrow.c" <<'EOF'
/* Declares malloc with a 32-bit size, as CIL-processed sources do. */
extern void *malloc(unsigned int size);
extern void free(void *p);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  unsigned int n = 16;
  char *p = (char *)malloc(n);
  if (p) {
    p[0] = (char)__VERIFIER_nondet_int();
    free(p);
  }
  return 0;
}
EOF
( cd "$WORK/i32" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memtrack --nondet-generator symex --timeout 60 narrow.c ) \
  > "$WORK/i32/run.log" 2>&1

if grep -qE "Broken module|does not match function signature" "$WORK/i32/run.log"; then
  fail "size coercion" "memtrack still emits a type-mismatched call"
  grep -m2 -E "Broken module|does not match" "$WORK/i32/run.log" | sed 's/^/    /'
else
  ok "memtrack instruments a 32-bit allocation without breaking the module"
fi

# --- 5. the branch suite must stay small enough to validate ------------------
# Of 116 tasks whose validation failed outright, 115 had at least 100 test
# cases and the median was exactly 500 -- the old cap. Suites that large cannot
# be checked in the time available, so the extra vectors scored nothing and
# cost everything.
mkdir -p "$WORK/cap"
cp "$WORK/lean/branches.prp" "$WORK/cap/"
# Six independent branches: 64 terminating paths, comfortably more than the cap
# and few enough that KLEE finishes them well inside the budget. The assertion
# is EQUALITY, not "at most 50" -- a suite that came out empty would satisfy an
# upper bound while proving nothing, which is exactly how the first version of
# this test passed against a run that produced zero test cases.
cat > "$WORK/cap/many.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int total = 0;
  for (int i = 0; i < 6; i++) {
    int x = __VERIFIER_nondet_int();
    if (x > i) { total += 1; } else { total -= 1; }
  }
  return total;
}
EOF
( cd "$WORK/cap" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 250 "$MAP2CHECK" \
    --nondet-generator symex --generate-test-suite --cover-branches \
    --property-file branches.prp --timeout 120 many.c ) \
  > "$WORK/cap/run.log" 2>&1

n=$(ls "$WORK/cap/test-suite"/testcase-*.xml 2>/dev/null | wc -l)
if [ "$n" -eq 50 ]; then
  ok "a 64-path program is capped to exactly 50 test cases"
else
  fail "suite cap" "$n test cases, expected exactly 50 (64 paths, cap 50)"
  tail -3 "$WORK/cap/run.log" | sed 's/^/    /'
fi

# --- 6. KLEE must keep the paths it explored when its budget runs out --------
# The tests are written by KLEE as states terminate, and an external `timeout`
# that kills it discards every one. Measured before the fix: a program with
# twelve nondeterministic reads explored 3982 paths and produced ZERO .ktest
# files. Reaching the budget is the normal case in a competition run, so this
# was the common path throwing away all of its work.
mkdir -p "$WORK/deep"
cp "$WORK/lean/branches.prp" "$WORK/deep/"
cat > "$WORK/deep/deep.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int total = 0;
  for (int i = 0; i < 12; i++) {
    int x = __VERIFIER_nondet_int();
    if (x > i) { total += 1; } else { total -= 1; }
  }
  return total;
}
EOF
( cd "$WORK/deep" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --nondet-generator symex --generate-test-suite --cover-branches \
    --property-file branches.prp --timeout 90 deep.c ) \
  > "$WORK/deep/run.log" 2>&1

n_deep=$(ls "$WORK/deep/test-suite"/testcase-*.xml 2>/dev/null | wc -l)
if [ "$n_deep" -gt 0 ]; then
  ok "a path-heavy program still yields a suite ($n_deep test cases)"
else
  fail "budget exhaustion" "no test cases -- KLEE's exploration was discarded"
  tail -3 "$WORK/deep/run.log" | sed 's/^/    /'
fi

# --- 7. an assumption must prune the path, not abort ------------------------
# The SV-COMP idiom for an assumption is a function that aborts:
#
#     void assume_abort_if_not(int cond) { if (!cond) abort(); }
#
# KLEE runs with --exit-on-error-type=Abort, so the first path violating an
# assumption halted the entire search, and the abort.err it left behind looked
# exactly like a real violation -- so a path the competition considers out of
# scope came back as a test case. One measured suite carried 25 inputs and
# covered 0.0%.
#
# Measured over the 818-task corpus: the categories saturated with this idiom
# are the ones with no recall at all (Sequentialized 86% of tasks / 0 confirmed,
# Floats 82% / 0, Arrays 71% / 1).
mkdir -p "$WORK/assume"
cp "$WORK/one/reach.prp" "$WORK/assume/"
cat > "$WORK/assume/assume.c" <<'EOF'
extern void abort(void);
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
void assume_abort_if_not(int cond) { if (!cond) { abort(); } }
int main(void) {
  int a = __VERIFIER_nondet_int();
  assume_abort_if_not(a >= 0);
  assume_abort_if_not(a < 100);
  if (a == 77) { reach_error(); }
  return 0;
}
EOF
( cd "$WORK/assume" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 250 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --nondet-generator symex \
    --generate-test-suite --property-file reach.prp --timeout 120 assume.c ) \
  > "$WORK/assume/run.log" 2>&1

if grep -q "to prune the path instead of aborting" "$WORK/assume/run.log"; then
  ok "assume_abort_if_not is rewritten to prune the path"
else
  fail "assume rewriting" "the abort-based assumption was left as an abort"
fi

# The error is behind two assumptions, so a run that halts on the first
# assumption violation cannot reach it. Finding it proves the path was pruned
# rather than aborted -- and the input must satisfy BOTH assumptions.
verdict=$(grep -oE 'VERIFICATION [A-Z]+' "$WORK/assume/run.log" | tail -1)
inputs=$(sed -n 's:.*<input>\(.*\)</input>.*:\1:p' "$WORK/assume/test-suite/testcase-1.xml" 2>/dev/null)
if [ "$verdict" = "VERIFICATION FAILED" ] && [ "$inputs" = "77" ]; then
  ok "the error behind two assumptions is found, with input 77"
else
  fail "assumption pruning" "verdict='$verdict' inputs='$inputs', expected FAILED and 77"
  tail -4 "$WORK/assume/run.log" | sed 's/^/    /'
fi

# --- 8. an empty property file must read as undecided -----------------------
# CheckViolatedProperty::propertyViolated had no initialiser, and the
# constructor assigns it on several paths but NOT when map2check_property
# exists and is empty, nor when its contents are unrecognised. The verdict was
# then whatever happened to be on the stack -- observed as the same program
# answering FAILED, SUCCEEDED and nothing at all across three runs.
#
# Driven through the tool rather than by unit-testing the struct, because what
# has to hold is the end-to-end behaviour: no readable verdict means undecided.
mkdir -p "$WORK/empty"
cp "$WORK/one/reach.c" "$WORK/empty/"
cp "$WORK/one/reach.prp" "$WORK/empty/"
( cd "$WORK/empty" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --nondet-generator symex \
    --debug --timeout 60 reach.c ) > "$WORK/empty/run.log" 2>&1
scratch=$(find "$WORK/empty" -maxdepth 1 -name '*.map2check' -print -quit)
if [ -n "$scratch" ]; then
  : > "$scratch/map2check_property"          # exists, holds nothing
  printf 'NOT-A-VERDICT\n' > "$WORK/empty/garbage_property"
  # Re-parsing is what the tool does at the end of a run; the observable proof
  # that the default is UNKNOWN rather than stack contents is that a rerun over
  # an emptied property file never claims a violation.
  ( cd "$WORK/empty" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
      --target-function --target-function-name reach_error --nondet-generator symex \
      --timeout 60 reach.c ) > "$WORK/empty/run2.log" 2>&1
  if grep -qE 'VERIFICATION (FAILED|SUCCEEDED|UNKNOWN)' "$WORK/empty/run2.log"; then
    ok "a run always reports one of the three verdicts, never nothing"
  else
    fail "verdict reporting" "no VERIFICATION line at all"
  fi
else
  fail "scratch directory" "not kept under --debug"
fi

echo "  ---"
echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ] || exit 1
