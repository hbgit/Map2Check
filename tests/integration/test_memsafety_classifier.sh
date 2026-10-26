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
check false no-overflow      FALSE-OVERFLOW  correct-false
check false no-overflow      FALSE-DEREF     wrong-false
check true  ""               FALSE-OVERFLOW  wrong-false
# sv-benchmarks' Juliet_Test MemSafety tasks declare no subproperty: any
# memory FALSE is the right answer there, but not a leak-at-exit or overflow.
check false any              FALSE-DEREF     correct-false
check false any              FALSE-MEMTRACK  correct-false
check false any              FALSE-OVERFLOW  wrong-false
echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ]
