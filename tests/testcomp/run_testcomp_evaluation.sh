#!/bin/bash
# run_testcomp_evaluation.sh -- Map2Check over a stratified Test-Comp corpus,
# with every suite handed to TestCov for validation.
#
# What this measures, and what it does not. The per-task budget here is a small
# fraction of Test-Comp's 900s, so the numbers are NOT competition scores and
# must never be quoted as such. What they are is a broad functional sweep: does
# the tool produce a well-formed suite across the whole shape of the benchmark,
# and does an independent validator agree the suite covers what it claims.
#
# Resumable by design. The CSV is the state: a task already in it is skipped, so
# a container that dies, a WSL restart, or a deadline hit can all be continued by
# re-running the same command. That is also why the deadline is checked between
# tasks rather than enforced with an outer timeout -- a killed run mid-write
# leaves a truncated row that the resume logic would then trust.
#
# Environment:
#   MANIFEST      tab-separated corpus from build_corpus.py (required)
#   PROPERTY      cover-error | cover-branches (required)
#   RESULTS_DIR   where the CSV and raw logs go (required)
#   SHARD/SHARDS  process every SHARDS-th task starting at SHARD (default 0/1)
#   GENERATOR     symex | fuzzer | hybrid | hybrid-seed (default hybrid)
#   EXTRA_FLAGS   passed verbatim to map2check (e.g. "--slice")
#   BUDGET        seconds given to map2check per task (default 60)
#   TESTCOV_S     seconds given to TestCov per task (default 90)
#   DEADLINE_S    stop starting new tasks after this many seconds (default 18000)
#
# The container needs gcc-multilib. Most sv-benchmarks tasks declare ILP32, and
# without it TestCov cannot compile the program for -32 and reports no Result
# line at all -- which looks like a tool error and is really a missing package.

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BENCH="$SCRIPT_DIR/bench/sv-benchmarks/c"

MANIFEST="${MANIFEST:?set MANIFEST}"
PROPERTY="${PROPERTY:?set PROPERTY to cover-error or cover-branches}"
RESULTS_DIR="${RESULTS_DIR:?set RESULTS_DIR}"
SHARD="${SHARD:-0}"
SHARDS="${SHARDS:-1}"
BUDGET="${BUDGET:-60}"
TESTCOV_S="${TESTCOV_S:-90}"
DEADLINE_S="${DEADLINE_S:-18000}"

# Which engine produces the inputs. Parameterised so the three arms can be
# compared on the SAME task list:
#
#   symex        KLEE only -- every Test-Comp measurement before 2026-08-23
#   fuzzer       LibFuzzer only
#   hybrid       the tool's actual default: LibFuzzer at 0.2x, then KLEE
#   hybrid-seed  the same, with the two engines exchanging input vectors
#
# The default is hybrid, which is also the tool's own default when no
# --nondet-generator is given. Every Test-Comp measurement before 2026-08-23
# forced symex and so measured a mode nobody actually runs.
#
# Measured on the same 372-task corpus: symex 23% covered, fuzzer 35%, hybrid
# 47% -- and the hybrid captured the whole UNION of what the two engines reach
# alone. Directories written before the switch carry `symex` in this comment's
# place; they are not comparable to hybrid runs and should not be pooled.
GENERATOR="${GENERATOR:-hybrid}"

# Extra flags handed straight to map2check, for measuring an opt-in capability
# against a run without it. Kept separate from GENERATOR because these are not
# engine choices: --slice and --seed-exchange both sit on top of whichever
# engine is running.
EXTRA_FLAGS="${EXTRA_FLAGS:-}"
case "$GENERATOR" in
  symex|fuzzer) GENERATOR_FLAG="--nondet-generator $GENERATOR" ;;
  hybrid)       GENERATOR_FLAG="" ;;   # absent flag IS the hybrid path
  hybrid-seed)  GENERATOR_FLAG="--seed-exchange" ;;
  *) echo "unknown GENERATOR: $GENERATOR" >&2; exit 2 ;;
esac

MAP2CHECK_DIR="${MAP2CHECK_PATH:?set MAP2CHECK_PATH}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"

case "$PROPERTY" in
  cover-error)    PRP="$BENCH/properties/coverage-error-call.prp"
                  GOAL_FLAGS="--target-function --target-function-name reach_error" ;;
  cover-branches) PRP="$BENCH/properties/coverage-branches.prp"
                  GOAL_FLAGS="--cover-branches" ;;
  *) echo "unknown PROPERTY: $PROPERTY" >&2; exit 2 ;;
esac

mkdir -p "$RESULTS_DIR/raw"
CSV="$RESULTS_DIR/results.csv"
if [ ! -f "$CSV" ]; then
  echo "category,program,data_model,expected_unreach,verdict,testcov,coverage,goals,n_tests,elapsed_s" > "$CSV"
fi

# Already-done set, read once. Re-reading per task turns a resume into O(n^2)
# over a corpus of hundreds.
declare -A DONE
while IFS=, read -r _cat prog _rest; do
  [ -n "$prog" ] && DONE["$prog"]=1
done < <(tail -n +2 "$CSV")

started=$(date +%s)
index=-1
processed=0
skipped=0

# fd 3, not stdin: map2check, zip and testcov all inherit stdin, and one of them
# consuming it eats the rest of the manifest. That is not hypothetical -- it is
# how an earlier harness processed exactly one case per chunk.
while IFS=$'\t' read -r category program data_model expected <&3; do
  case "$category" in ''|\#*) continue ;; esac
  index=$((index + 1))
  [ $((index % SHARDS)) -eq "$SHARD" ] || continue

  if [ -n "${DONE[$program]:-}" ]; then
    skipped=$((skipped + 1))
    continue
  fi

  now=$(date +%s)
  if [ $((now - started)) -ge "$DEADLINE_S" ]; then
    echo "[deadline] stopping after ${processed} tasks ($(( (now-started)/60 )) min)"
    break
  fi

  src="$BENCH/$program"
  if [ ! -f "$src" ]; then
    echo "$category,$program,$data_model,$expected,MISSING,NA,,,0,0" >> "$CSV"
    continue
  fi

  # A private directory per task. map2check names its scratch by the hash of the
  # input, so two tasks with identical content would otherwise share -- and a
  # directory left behind by an aborted run would be reused with stale contents.
  work=$(mktemp -d)
  cp "$src" "$work/" 2>/dev/null || { rm -rf "$work"; continue; }
  cp "$PRP" "$work/prop.prp"
  name=$(basename "$src")
  arch=$([ "$data_model" = "LP64" ] && echo 64bit || echo 32bit)

  t0=$(date +%s)
  (
    cd "$work" || exit 1
    MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 $((BUDGET + 30)) "$MAP2CHECK" \
        $GENERATOR_FLAG $EXTRA_FLAGS --generate-test-suite $GOAL_FLAGS \
        --property-file prop.prp --architecture "$arch" \
        --timeout "$BUDGET" "$name"
  ) > "$work/map2check.log" 2>&1 </dev/null
  t1=$(date +%s)

  verdict=$(grep -oE 'VERIFICATION (FAILED|SUCCEEDED|UNKNOWN)' "$work/map2check.log" | tail -1)
  verdict="${verdict#VERIFICATION }"
  [ -z "$verdict" ] && verdict="ERROR"
  n_tests=$(ls "$work/test-suite"/testcase-*.xml 2>/dev/null | wc -l)

  testcov="NO_SUITE"
  coverage=""
  goals=""
  if [ -d "$work/test-suite" ]; then
    ( cd "$work/test-suite" && zip -q -r ../suite.zip . ) </dev/null
    bits=$([ "$arch" = "64bit" ] && echo -64 || echo -32)
    ( cd "$work" && timeout -k 10 "$TESTCOV_S" testcov --test-suite suite.zip \
        --no-isolation "$bits" --goal prop.prp "$name" ) \
      > "$work/testcov.log" 2>&1 </dev/null
    # TestCov exits 0 whether or not the suite covers, so the verdict is the
    # Result line, never $?.
    #
    # The label is property-aware, because the same Result line does not mean
    # the same thing in both categories.
    #
    # cover-error is pass/fail: TRUE means the suite reproduced the bug,
    # UNKNOWN means it did not.
    #
    # cover-branches has no pass/fail. The competition scores the coverage
    # FRACTION, and the Result line says nothing about it: measured, DONE
    # appears at 100% and at 77.78%, while UNKNOWN appears when TestCov had to
    # abort a test's execution. So DONE means "validated cleanly" and UNKNOWN
    # means "validated, with a test that would not run" -- and in BOTH cases
    # the coverage column beside it is the number that matters. Labelling them
    # FULL and PARTIAL, as a first version did, reads as a coverage claim the
    # Result line is not making.
    if grep -qE '^Result: TRUE' "$work/testcov.log"; then
      testcov="COVERED"
    elif grep -qE '^Result: DONE' "$work/testcov.log"; then
      testcov=$([ "$PROPERTY" = "cover-branches" ] && echo VALIDATED || echo DONE)
    elif grep -qE '^Result: UNKNOWN' "$work/testcov.log"; then
      testcov=$([ "$PROPERTY" = "cover-branches" ] && echo VALIDATED_ABORTS \
                                                    || echo NOT_COVERED)
    else
      testcov="TESTCOV_ERROR"
    fi
    coverage=$(grep -oE '^Coverage: [0-9.]+' "$work/testcov.log" | tail -1 | awk '{print $2}')
    goals=$(grep -oE '^Number of goals: [0-9]+' "$work/testcov.log" | tail -1 | awk '{print $4}')
  fi

  echo "$category,$program,$data_model,$expected,$verdict,$testcov,$coverage,$goals,$n_tests,$((t1-t0))" >> "$CSV"
  printf '[%s] %-14s %-52s %-9s %-13s %s%%\n' \
      "$(date +%H:%M:%S)" "$category" "$(basename "$program")" "$verdict" "$testcov" "${coverage:-—}"

  # Raw logs only for the interesting cases: keeping every log over hundreds of
  # tasks fills the disk with successes nobody reads.
  if [ "$testcov" = "TESTCOV_ERROR" ] || [ "$verdict" = "ERROR" ]; then
    cp "$work/map2check.log" "$RESULTS_DIR/raw/$(echo "$program" | tr '/' '_').m2c.log" 2>/dev/null
    cp "$work/testcov.log" "$RESULTS_DIR/raw/$(echo "$program" | tr '/' '_').tc.log" 2>/dev/null
  fi

  rm -rf "$work"
  processed=$((processed + 1))
done 3< "$MANIFEST"

echo "=========================================================="
echo "shard $SHARD/$SHARDS: $processed processed, $skipped already done"
echo "CSV: $CSV"
echo "=========================================================="
