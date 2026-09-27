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

# The SAME idiom under a shorter name, and the name was the whole difference.
# The XCSP family writes `void assume(int c){ if(!c) abort(); }`, which the
# first version of the list did not match -- so all 59 XCSP tasks answered
# "program correct" in two seconds, every one of them on a program with a
# reachable bug. A defect found by spelling.
mkdir -p "$WORK/assume2"
cp "$WORK/one/reach.prp" "$WORK/assume2/"
cat > "$WORK/assume2/plain.c" <<'EOF'
extern void abort(void);
extern void __assert_fail(const char *, const char *, unsigned int,
                          const char *) __attribute__((__noreturn__));
void reach_error(void) { __assert_fail("0", "plain.c", 3, "reach_error"); }
void assume(int cond) { if (!cond) { abort(); } }
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  assume(a >= 0);
  assume(a < 50);
  reach_error();
  return 0;
}
EOF
( cd "$WORK/assume2" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 250 "$MAP2CHECK"     --target-function --target-function-name reach_error     --timeout 90 plain.c ) > "$WORK/assume2/run.log" 2>&1

# reach_error sits AFTER the assumptions, exactly as the XCSP programs place
# it, so a run that still aborts on the first one cannot get there.
if grep -q 'VERIFICATION FAILED' "$WORK/assume2/run.log"; then
  ok "a bug behind a plain assume() is found, not called correct"
else
  fail "plain assume"        "got $(grep -oE 'VERIFICATION [A-Z]+' "$WORK/assume2/run.log" | tail -1) -- the program has a reachable bug"
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

# --- 9. every generator must report a verdict --------------------------------
# The UNKNOWN branch was guarded on the generator being KLEE, so an undecided
# LibFuzzer run printed no verdict line at all. Every harness here parses
# stdout for one, and so does the BenchExec tool-info; silence reads as a
# crash. Whole categories came back as ERROR in the engine comparison for this
# reason alone.
mkdir -p "$WORK/verdict"
cat > "$WORK/verdict/hard.c" <<'EOF'
extern void __assert_fail(const char *, const char *, unsigned int,
                          const char *) __attribute__((__noreturn__));
void reach_error(void) { __assert_fail("0", "hard.c", 3, "reach_error"); }
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  /* Two exact 32-bit matches at once: a fuzzer will not stumble onto this
   * inside the budget, so the run ends undecided -- which is the point. */
  if (a == 1234567 && b == 7654321) { reach_error(); }
  return 0;
}
EOF
for gen in afl symex; do
  ( cd "$WORK/verdict" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 120 "$MAP2CHECK" \
      --target-function --target-function-name reach_error \
      --nondet-generator "$gen" --timeout 45 hard.c ) > "$WORK/verdict/$gen.log" 2>&1
  if grep -qE 'VERIFICATION (FAILED|SUCCEEDED|UNKNOWN)' "$WORK/verdict/$gen.log"; then
    ok "the $gen generator reports a verdict"
  else
    fail "$gen verdict" "no VERIFICATION line -- a caller reads this as a crash"
  fi
done

# --- 10. the fuzzer must reach the whole width of a type ---------------------
# Each generator took ONE byte and cast it, so a short could only be 0..255 and
# nothing negative was reachable at all -- an unsigned byte cast to a signed
# type stays non-negative. Half of every signed type was unreachable.
#
# It also blocked seeding: the byte layout IS the exchange format between the
# engines, and a KLEE vector holding short x = 4242 cannot be written into a
# slot one byte wide. sizeof(type) is what KLEE already passes to
# klee_make_symbolic, so this puts both engines on one layout.
mkdir -p "$WORK/width"
cat > "$WORK/width/neg.c" <<'EOF'
extern void __assert_fail(const char *, const char *, unsigned int,
                          const char *) __attribute__((__noreturn__));
void reach_error(void) { __assert_fail("0", "neg.c", 3, "reach_error"); }
extern short __VERIFIER_nondet_short(void);
int main(void) {
  short s = __VERIFIER_nondet_short();
  if (s < 0) { reach_error(); }
  return 0;
}
EOF
( cd "$WORK/width" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 150 "$MAP2CHECK" \
    --target-function --target-function-name reach_error \
    --nondet-generator afl --timeout 60 neg.c ) > "$WORK/width/run.log" 2>&1

if grep -q 'VERIFICATION FAILED' "$WORK/width/run.log"; then
  ok "the fuzzer reaches a negative short"
else
  fail "fuzzer value range" \
       "did not reach short < 0 -- half of the type is unreachable"
fi

# --- 11. the engines must be able to hand each other input vectors ----------
# Two engines that cover almost disjoint sets -- measured over 372 tasks, 65
# reachable only by the fuzzer and 47 only by KLEE -- and until now they shared
# nothing. LibFuzzer did not even keep its own corpus: with no corpus directory
# it holds everything in memory and drops it when the process ends.
#
# Asserted here is the MECHANISM, not a coverage gain. Whether cooperating
# finds more is a question for the corpus, not for a unit-sized program; what
# a test can pin is that the channel exists, is off by default, and carries
# vectors in the direction it claims to.
mkdir -p "$WORK/seed"
cat > "$WORK/seed/seed.c" <<'EOF'
extern void __assert_fail(const char *, const char *, unsigned int,
                          const char *) __attribute__((__noreturn__));
void reach_error(void) { __assert_fail("0", "seed.c", 3, "reach_error"); }
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  if (a > 100 && a < 200) {
    if (b == a + 7) { reach_error(); }
  }
  return 0;
}
EOF

# Off by default: the hybrid's measured behaviour must not change until the
# exchange has earned its place beside it.
( cd "$WORK/seed" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 250 "$MAP2CHECK" \
    --target-function --target-function-name reach_error \
    --debug --timeout 60 seed.c ) > "$WORK/seed/off.log" 2>&1
scratch_off=$(find "$WORK/seed" -maxdepth 1 -name '*.map2check' -print -quit)
n_off=$(ls "$scratch_off/seeds" 2>/dev/null | wc -l)
if [ "$n_off" -eq 0 ]; then
  ok "no seed corpus without --seed-exchange"
else
  fail "default behaviour" "$n_off seeds written without asking"
fi
rm -rf "$WORK/seed"/*.map2check

( cd "$WORK/seed" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 300 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --seed-exchange \
    --debug --timeout 60 seed.c ) > "$WORK/seed/on.log" 2>&1
scratch_on=$(find "$WORK/seed" -maxdepth 1 -name '*.map2check' -print -quit)
# Only the fuzzer's own discoveries count: the Caller writes a placeholder
# seed into seeds/ itself, so a plain file count would pass with no copy-back.
n_on=$(ls "$scratch_on/seeds" 2>/dev/null | grep -c '^afl-')

# afl-fuzz never writes into its -i dir; the Caller copies its queue back in
# after the fuzzer phase, so the corpus survives the fuzzer process. (It does
# not yet survive into the next phase: each Caller recreates the scratch
# directory -- inherited from v15, left to the smart-seeds work.)
if [ "$n_on" -gt 0 ]; then
  ok "the fuzzer discoveries are copied into seeds/ with --seed-exchange ($n_on files)"
else
  fail "seed corpus" "nothing kept -- the corpus is still in-memory only"
fi

# KLEE -> fuzzer: its per-path vectors become seed files. Sound only because
# both engines now consume sizeof(type) per read, so concatenating a .ktest's
# objects is exactly the buffer that drives the fuzzer down the same path.
#
# The export only happens if the KLEE phase runs, and with CmpLog the fuzzer
# phase sometimes solves seed.c on its own and ends the hybrid first. That is
# not a failure of the channel, so retry a couple of times for a run where
# KLEE gets its turn; only a KLEE phase that ran and exported nothing fails.
klee_log="$WORK/seed/on.log"
for attempt in 2 3; do
  grep -q "Executing Klee" "$klee_log" && break
  rm -rf "$WORK/seed"/*.map2check
  klee_log="$WORK/seed/on.$attempt.log"
  ( cd "$WORK/seed" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 300 "$MAP2CHECK" \
      --target-function --target-function-name reach_error --seed-exchange \
      --timeout 60 seed.c ) > "$klee_log" 2>&1
done
if grep -q "Seeded the fuzzer corpus with" "$klee_log"; then
  ok "KLEE's path vectors are exported into the seed corpus"
elif ! grep -q "Executing Klee" "$klee_log"; then
  ok "KLEE -> fuzzer not exercised: the fuzzer solved seed.c first in 3 runs"
else
  fail "KLEE -> fuzzer" "no vectors exported"
fi

# --- 12. slicing must degrade loudly, never silently -------------------------
# Finding K2 measured a nondeterministic read at 1 -> 21 -> 114 -> 861 partial
# paths for 2 -> 4 reads, and most of that forking happens in code that cannot
# influence the target. Slicing removes it before KLEE ever sees it.
#
# What is asserted here is the behaviour when things are NOT ideal, because
# that is where this class of feature fails: --add-invariants spent years
# accepted and ignored (issue #54), and a slicer that quietly does nothing
# would be the same defect wearing a different name.
mkdir -p "$WORK/slice"
cp "$WORK/one/reach.c" "$WORK/slice/"

( cd "$WORK/slice" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --slice \
    --nondet-generator symex --timeout 45 reach.c ) > "$WORK/slice/run.log" 2>&1

# Either the slicer is installed and slices, or it is absent and the run says
# so. What must never happen is silence.
if grep -q "Sliced with respect to" "$WORK/slice/run.log"; then
  ok "the program was sliced with respect to the target"
elif grep -q "sbt-slicer is not installed" "$WORK/slice/run.log"; then
  ok "an absent slicer is reported, and the run continues unsliced"
else
  fail "slicing" "--slice produced neither a slice nor an explanation"
fi

# There is no criterion to slice towards when the goal is a memory property or
# branch coverage, so asking must be refused rather than quietly ignored.
( cd "$WORK/slice" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --cover-branches --slice --nondet-generator symex --timeout 45 reach.c ) \
  > "$WORK/slice/mode.log" 2>&1
if grep -q "applies to reachability, assert, memory and overflow properties only" "$WORK/slice/mode.log"; then
  ok "--slice is refused where there is no criterion to slice towards"
else
  fail "slice mode guard" "--slice was accepted in a mode that has no criterion"
fi

# --- 13. a slice must leave KLEE something it can run -------------------------
# sbt-slicer's --cutoff-diverging (default on) rewrites every path that cannot
# reach the criterion into a `diverge:` block calling exit(0) -- with no debug
# location. The program is compiled with -g; once KLEE links uClibc, exit has a
# body, and the verifier rejects the module ("inlinable function call in a
# function with debug info must have a !dbg location"). KLEE aborted before
# executing anything, on every sliced task with a cut path: the slice arm of
# the v15 campaign ran without its symbolic engine.
mkdir -p "$WORK/cut"
# ECA-shaped on purpose: the slicer turns a plain return from main into a
# `safe_return`, so a straight-line program never gets a `diverge:` block. A
# reactive loop whose step can take a path that never reaches the target does;
# bounded to two steps so that KLEE decides it well inside the budget.
cat > "$WORK/cut/cut.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
extern void exit(int);
int a = 1;
void step(int in) {
  if (in == 5) { a = 2; return; }
  if (in == 6 && a == 2) { reach_error(); }
  if (in == 9) { exit(0); }
}
int main(void) {
  for (int i = 0; i < 2; i++) {
    int in = __VERIFIER_nondet_int();
    step(in);
  }
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

# --- 16. nondet names the fixed list does not know are kept too --------------
# The criteria carry a fixed list of __VERIFIER_nondet_* names, and no fixed
# list knows every name a benchmark declares (int128, uint128, ...). A missing
# one silently brings back the shifted suite of section 14, so the names are
# also read from the program. Checked on the slicer command itself: int128 is
# not in the fixed list, so its presence there proves it came from the program.
mkdir -p "$WORK/names"
cat > "$WORK/names/names.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern __int128 __VERIFIER_nondet_int128(void);
extern void reach_error(void);
int main(void) {
  __int128 wide = __VERIFIER_nondet_int128();
  int b = __VERIFIER_nondet_int();
  if (b == 42) { reach_error(); }
  return (int)wide;
}
EOF
( cd "$WORK/names" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --target-function --target-function-name reach_error --slice --debug \
    --nondet-generator symex --timeout 30 names.c ) > "$WORK/names/run.log" 2>&1
if grep "sbt-slicer" "$WORK/names/run.log" | grep -q "__VERIFIER_nondet_int128"; then
  ok "nondet names declared by the program are slicing criteria too"
else
  fail "program nondet names" "__VERIFIER_nondet_int128 is not among the criteria"
fi

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

# --- 21. a memory error inside an external function must not be sliced away --
# CASTLE-787-2 overflows a stack buffer inside strcpy. No map2check_* call
# depends on that call, and the slicer treats external functions as only
# reading their arguments, so it was dropped -- and the run answered TRUE for a
# program with an out-of-bounds write. Every external call is a criterion now.
cat > "$WORK/mem/strcpy.c" <<'EOF'
#include <stdio.h>
#include <string.h>
int main(void) {
  char username[10];
  strcpy(username, "Is_this_too_long_for_this_array_buffer?");
  printf("Hello %s!\n", username);
  return 0;
}
EOF
( cd "$WORK/mem" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memtrack --slice --nondet-generator symex --timeout 45 strcpy.c ) > "$WORK/mem/strcpy.log" 2>&1
if grep -q "VERIFICATION SUCCEEDED" "$WORK/mem/strcpy.log"; then
  fail "external call slicing" "the overflowing strcpy was sliced away: TRUE"
else
  ok "a memory error inside an external call survives slicing"
fi

# --- 22. KLEE stopping on its timer is not a proof ---------------------------
# KLEE halting on --max-time exits 0, like a run that explored every path. One
# short path wrote NONE to the property file, and the run answered TRUE for a
# program with a reachable null dereference. Found by slicing (memsafety-cve
# frr.i, pacparser.i: the slice let KLEE reach its own timer), but it is not a
# slicing defect -- this program is not sliced.
mkdir -p "$WORK/halt"
cat > "$WORK/halt/halt.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 0) { return 0; }
  int n = 0;
  while (1) {
    int y = __VERIFIER_nondet_int();
    if (y > 3) { n++; } else { n += 2; }
    if (n == 200000) { int *p = 0; *p = 1; }
  }
  return 0;
}
EOF
( cd "$WORK/halt" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
    --memtrack --nondet-generator symex --timeout 20 halt.c ) > "$WORK/halt/run.log" 2>&1
if grep -q "VERIFICATION SUCCEEDED" "$WORK/halt/run.log"; then
  fail "halted KLEE verdict" "TRUE after KLEE stopped on its timer"
else
  ok "a KLEE run halted on its timer is not reported TRUE"
fi

# --- 23. overflow slices the instrumented module and keeps the violation -----
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

# --- 24. slicing must not invent an overflow ---------------------------------
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

echo "  ---"
echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ] || exit 1
