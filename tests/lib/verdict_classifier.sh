# shellcheck shell=bash
# Shared verdict classifier for the CASTLE and Juliet evaluation runners.
#
#   classify_map2check_verdict <output> <exit_code> <elapsed_s> <budget_s>
#
# Prints exactly one of:
#   FALSE-DEREF FALSE-FREE FALSE-MEMTRACK FALSE-OVERFLOW FALSE-DIVBYZERO
#   FALSE-MEMCLEANUP FALSE TRUE TIMEOUT ERROR UNKNOWN
#
# Both runners used to carry their own copy of this logic and both got the
# timeout oracle wrong in the same way, so it lives here once and is covered by
# tests/integration/test_verdict_classifier.sh.
#
# --- Why "Forcing timeout" is NOT a timeout signal ---
#
# map2check's default is hybrid: LibFuzzer for 0.2x the budget, then KLEE for
# the remaining 0.8x. map2check.cpp prints "Note: Forcing timeout" whenever a
# phase exhausts its slice -- which the fuzzer phase does on essentially every
# run that does not get an early hit. It is the normal path into the KLEE
# phase, not a verdict. Keying TIMEOUT off it relabels every UNKNOWN and every
# ERROR as TIMEOUT, which silently hides real tool failures (a KLEE segfault on
# an external call, for one) behind a bucket that reads as "just needs more
# time".
#
# The trustworthy timeout signals are the harness's own: the outer `timeout`
# exiting 124, or a run that consumed its whole budget without deciding.
#
# --- Why "undefined reference" needs qualifying ---
#
# KLEE prints "KLEE: WARNING: undefined reference to function: fscanf" on
# perfectly healthy runs for every unresolved external. The pattern was meant
# to catch the *linker* error ("undefined reference to __map2check_main__"), so
# it has to skip KLEE's warning lines or every benchmark touching libc lands in
# ERROR.

classify_map2check_verdict() {
  local output="$1"
  local exit_code="${2:-0}"
  local elapsed="${3:-0}"
  local budget="${4:-0}"

  # 1. A definitive verdict wins over everything else. A crash in one phase
  #    does not invalidate a decision reached by the other.
  if echo "$output" | grep -q "FALSE-DEREF"; then
    echo "FALSE-DEREF"; return
  elif echo "$output" | grep -q "FALSE-FREE"; then
    echo "FALSE-FREE"; return
  elif echo "$output" | grep -q "FALSE-MEMTRACK"; then
    echo "FALSE-MEMTRACK"; return
  elif echo "$output" | grep -q "FALSE-OVERFLOW"; then
    echo "FALSE-OVERFLOW"; return
  elif echo "$output" | grep -q "FALSE-DIVBYZERO"; then
    echo "FALSE-DIVBYZERO"; return
  elif echo "$output" | grep -q "FALSE-MEMCLEANUP"; then
    echo "FALSE-MEMCLEANUP"; return
  elif echo "$output" | grep -q "VERIFICATION FAILED"; then
    echo "FALSE"; return
  elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
    echo "TRUE"; return
  fi

  # 2. Hard timeout: the outer `timeout` killed the process.
  if [ "$exit_code" = "124" ]; then
    echo "TIMEOUT"; return
  fi

  # 3. Infrastructure and tool failures. Distinct from UNKNOWN because they are
  #    actionable: the tool broke rather than failed to decide.
  if echo "$output" | grep -qE "timeout: failed to run command|Unknown command line argument"; then
    echo "ERROR"; return
  fi
  if echo "$output" | grep -E "undefined reference to" | grep -qv "KLEE: WARNING"; then
    echo "ERROR"; return
  fi
  if echo "$output" | grep -qE "Segmentation fault|dumped core|Aborted \(core dumped\)"; then
    echo "ERROR"; return
  fi

  # 4. Soft timeout: no verdict, and the run consumed (nearly) its whole
  #    budget. Reported separately from UNKNOWN so the baseline can tell
  #    "needs more time" apart from "explored and still undecided".
  if [ "$budget" -gt 0 ] 2>/dev/null; then
    local threshold
    threshold=$(awk -v b="$budget" 'BEGIN { printf "%d", b * 9 / 10 }')
    if awk -v e="$elapsed" -v t="$threshold" 'BEGIN { exit !(e >= t) }'; then
      echo "TIMEOUT"; return
    fi
  fi

  echo "UNKNOWN"
}
