#!/bin/bash
# test_cwe_mode_mapping.sh — the CASTLE CWE→mode mapping must never fall back to
# reachability mode.
#
# Finding B: 13 CWEs were mapped to `--target-function --target-function-name
# main`, which is degenerate — TargetPass instruments call sites whose callee is
# the named function, and a program never calls its own main. On the v6 baseline
# 98 of 217 runs went through that mode and produced zero FALSEs on 59 known
# vulnerable programs, earning 12 fake true negatives and 5 guaranteed false
# negatives.
#
# The contract this pins down:
#   1. no CWE maps to a reachability target of `main` (or any reachability mode);
#   2. 628/674/770/835 (termination/resource) are declared out of scope;
#   3. every CWE in the benchmark has an explicit mode or an out-of-scope
#      declaration — nothing can silently fall through unmapped.

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RUNNER="$SCRIPT_DIR/../castle/run_castle_evaluation.sh"
JSON="$SCRIPT_DIR/../castle/CASTLE-Benchmark/datasets/CASTLE-C250.min.json"

[ -f "$RUNNER" ] || { echo "  FAIL: $RUNNER not found"; exit 1; }
[ -f "$JSON" ] || { echo "  FAIL: $JSON not found"; exit 1; }

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

# Source the two declarations in a subshell so the runner itself never runs.
eval "$(sed -n '/declare -A CWE_MODE=/,/^)/p' "$RUNNER")"
eval "$(grep '^OUT_OF_SCOPE_CWES=' "$RUNNER")"

echo "=== CWE→mode mapping contract ==="

# 1. No mode may name a reachability target.
offender=""
for cwe in "${!CWE_MODE[@]}"; do
  case "${CWE_MODE[$cwe]}" in
    *target-function*) offender="$offender $cwe";;
  esac
done
if [ -n "$offender" ]; then
  fail "no reachability target" "CWE(s)$offender still use --target-function"
else
  ok "no CWE maps to --target-function (finding B guard)"
fi

# 2. The termination/resource CWEs must be declared out of scope.
missing=""
for cwe in 628 674 770 835; do
  found=""
  for o in "${OUT_OF_SCOPE_CWES[@]}"; do
    [ "$o" = "$cwe" ] && found=1
  done
  [ -n "$found" ] || missing="$missing $cwe"
done
if [ -n "$missing" ]; then
  fail "termination CWEs out of scope" "missing from OUT_OF_SCOPE_CWES:$missing"
else
  ok "628/674/770/835 declared out of scope"
fi

# 3. Every CWE in the benchmark is either mapped or out of scope.
bench_cwes=$(python3 -c "
import json
data = json.load(open('$JSON'))
print(' '.join(sorted({str(t['cwe']) for t in data['tests']})))
")
unmapped=""
for cwe in $bench_cwes; do
  in_mode=""
  for k in "${!CWE_MODE[@]}"; do [ "$k" = "$cwe" ] && in_mode=1; done
  in_scope=""
  for o in "${OUT_OF_SCOPE_CWES[@]}"; do [ "$o" = "$cwe" ] && in_scope=1; done
  if [ -z "$in_mode" ] && [ -z "$in_scope" ]; then unmapped="$unmapped $cwe"; fi
done
if [ -n "$unmapped" ]; then
  fail "full coverage" "CWE(s) neither mapped nor out of scope:$unmapped"
else
  ok "every benchmark CWE is mapped or declared out of scope"
fi

echo "=== Results: $PASSED passed, $FAILED failed ==="
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
