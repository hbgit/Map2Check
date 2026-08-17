#!/bin/bash
# test_verdict_classifier.sh — unit tests for the shared evaluation classifier.
#
# The CASTLE and Juliet baselines are only as trustworthy as this function: it
# turns map2check's raw output into the TP/TN/FP/FN buckets the reports are
# computed from. Two separate mislabelling bugs have already made it into
# published numbers, so every rule here has a case.

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
# shellcheck source=../lib/verdict_classifier.sh
. "$SCRIPT_DIR/../lib/verdict_classifier.sh"

PASSED=0
FAILED=0

check() {
    local name="$1" expect="$2" output="$3" code="${4:-0}" elapsed="${5:-0}" budget="${6:-0}"
    local got
    got=$(classify_map2check_verdict "$output" "$code" "$elapsed" "$budget")
    if [ "$got" = "$expect" ]; then
        echo "  PASS $name"
        PASSED=$((PASSED+1))
    else
        echo "  FAIL $name: expected $expect, got $got"
        FAILED=$((FAILED+1))
    fi
}

echo "============================================================"
echo "Map2Check Evaluation Verdict Classifier"
echo "============================================================"

# --- The regression this file exists for -------------------------------------
# A hybrid run whose fuzzer phase used its slice, then reached KLEE and came
# back undecided. "Note: Forcing timeout" is the fuzzer handing over to KLEE,
# not a verdict. This used to classify as TIMEOUT.
HYBRID_UNKNOWN='Started Map2Check
Instrumenting with LLVM LibFuzzer
Exited fuzzer with 31744
Note: Forcing timeout
Started Map2Check
Executing Klee with map2check
Exited klee with 0
Unable to prove or falsify the program.
VERIFICATION UNKNOWN'
check "hybrid handover then undecided is UNKNOWN, not TIMEOUT" \
    UNKNOWN "$HYBRID_UNKNOWN" 0 12 60

# Same handover, but KLEE crashed. That is a tool failure worth its own bucket,
# and it was also being hidden as TIMEOUT.
HYBRID_CRASH='Note: Forcing timeout
KLEE: WARNING: undefined reference to function: __isoc99_fscanf
timeout: the monitored command dumped core
Segmentation fault
Exited klee with 35584
Unable to prove or falsify the program.
VERIFICATION UNKNOWN'
check "KLEE crash is ERROR, not TIMEOUT" \
    ERROR "$HYBRID_CRASH" 0 13 60

# A definitive verdict still wins even when a phase died along the way.
check "verdict survives a crash in the other phase" \
    FALSE-OVERFLOW "$HYBRID_CRASH
FALSE-OVERFLOW" 0 13 60

# --- KLEE's own warnings are not infrastructure failures ----------------------
KLEE_WARNINGS='Note: Forcing timeout
KLEE: WARNING: undefined reference to function: fscanf
KLEE: WARNING: undefined reference to function: open
Unable to prove or falsify the program.
VERIFICATION UNKNOWN'
check "KLEE undefined-reference warnings are not ERROR" \
    UNKNOWN "$KLEE_WARNINGS" 0 5 60

check "linker undefined reference is ERROR" \
    ERROR 'undefined reference to `__map2check_main__`
collect2: error: ld returned 1 exit status' 0 1 60

check "opt rejecting a flag is ERROR" \
    ERROR "opt: Unknown command line argument '-m2c-entry-function'" 0 1 60

check "coreutils timeout failing to exec is ERROR" \
    ERROR "timeout: failed to run command '/x/map2check': No such file or directory" 0 1 60

# --- Timeouts -----------------------------------------------------------------
check "outer timeout exit 124 is TIMEOUT" \
    TIMEOUT "Started Map2Check" 124 300 300

check "budget exhausted without a verdict is TIMEOUT" \
    TIMEOUT "$HYBRID_UNKNOWN" 0 58 60

check "well under budget without a verdict is UNKNOWN" \
    UNKNOWN "$HYBRID_UNKNOWN" 0 3 60

check "no budget given falls back to UNKNOWN" \
    UNKNOWN "$HYBRID_UNKNOWN" 0 0 0

# --- Definitive verdicts ------------------------------------------------------
check "FALSE-DEREF"      FALSE-DEREF      "FALSE-DEREF detected" 0 1 60
check "FALSE-FREE"       FALSE-FREE       "FALSE-FREE detected" 0 1 60
check "FALSE-MEMTRACK"   FALSE-MEMTRACK   "FALSE-MEMTRACK detected" 0 1 60
check "FALSE-OVERFLOW"   FALSE-OVERFLOW   "FALSE-OVERFLOW detected" 0 1 60
check "FALSE-DIVBYZERO"  FALSE-DIVBYZERO  "FALSE-DIVBYZERO detected" 0 1 60
check "FALSE-MEMCLEANUP" FALSE-MEMCLEANUP "FALSE-MEMCLEANUP detected" 0 1 60
check "generic VERIFICATION FAILED" FALSE "VERIFICATION FAILED" 0 1 60
check "VERIFICATION SUCCEEDED"      TRUE  "VERIFICATION SUCCEEDED" 0 1 60

# A proven-safe program that took its whole budget is still TRUE.
check "verdict wins over budget exhaustion" \
    TRUE "VERIFICATION SUCCEEDED" 0 60 60

echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"

[ "$FAILED" -eq 0 ] && exit 0 || exit 1
