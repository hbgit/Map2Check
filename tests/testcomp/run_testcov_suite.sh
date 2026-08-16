#!/bin/bash
# run_testcov_suite.sh -- runs every program in expected.txt through Map2Check
# and TestCov, and compares each verdict against the manifest.
#
# Exit 0 only if every observed verdict equals its expected verdict. BOTH
# directions are failures: a regression, and an improvement nobody recorded.
# The second half of that matters more than it looks -- an unrecorded
# improvement is indistinguishable from a harness that started reporting
# COVERED for everything.
#
# Per-program budget is deliberately small. This is a conformance gate, not a
# benchmark: a program that needs more than two minutes to have its error
# covered does not belong in this corpus.

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MANIFEST="$SCRIPT_DIR/expected.txt"
PROPERTY="$SCRIPT_DIR/cover-error-call.prp"
export BUDGET="${BUDGET:-120}"

PASSED=0
FAILED=0

echo "============================================================"
echo "Test-Comp validation via TestCov (budget ${BUDGET}s per program)"
echo "============================================================"

if ! command -v "${TESTCOV:-testcov}" >/dev/null 2>&1; then
  echo "FATAL: ${TESTCOV:-testcov} not found. Install it with: pip3 install testcov" >&2
  exit 2
fi

while read -r program expected _rest; do
  case "$program" in '' | \#*) continue ;; esac

  path="$SCRIPT_DIR/programs/$program"
  if [ ! -f "$path" ]; then
    echo "  FAIL $program: listed in expected.txt but the file is missing"
    FAILED=$((FAILED + 1))
    continue
  fi

  observed=$(bash "$SCRIPT_DIR/run_testcov.sh" "$path" "$PROPERTY" --verdict-only)

  if [ "$observed" = "$expected" ]; then
    printf '  PASS %-18s %s\n' "$program" "$observed"
    PASSED=$((PASSED + 1))
  else
    printf '  FAIL %-18s expected %s, observed %s\n' "$program" "$expected" "$observed"
    FAILED=$((FAILED + 1))
  fi
done < "$MANIFEST"

echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
