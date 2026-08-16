#!/bin/bash
# run_testcov.sh <program.c> <property.prp> [--verdict-only]
#
# Generates a test suite with Map2Check and hands it to TestCov, the official
# Test-Comp validator. This answers the question the structural test cannot:
# not "is the XML well formed" but "does this suite actually reproduce the bug",
# which is the only thing the competition scores.
#
# TestCov takes a zip while the competition consumes a directory, so the zip is
# built here rather than by the tool.

set -u

PROGRAM="$1"
PROPERTY="$2"
VERDICT_ONLY=""
[ "${3:-}" = "--verdict-only" ] && VERDICT_ONLY=1

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
TESTCOV="${TESTCOV:-testcov}"
BUDGET="${BUDGET:-120}"

WORK=$(mktemp -d)
# Keeping the working directory on request is what makes a red CI job
# debuggable; cleaning by default keeps local runs from littering /tmp with one
# directory per program per invocation.
if [ -n "${KEEP_WORK:-}" ]; then
  trap 'echo "  (work kept at $WORK)"' EXIT
else
  trap 'rm -rf "$WORK"' EXIT
fi

# Verdict extraction. These patterns are not guesses: they are what TestCov 3.7
# printed on a covering and on a non-covering suite, recorded verbatim in
# NOTES-testcov-output.md.
#
# The exit code is NOT usable -- TestCov returns 0 in both cases, so a gate
# built on $? would pass unconditionally. Anything that matches neither pattern
# becomes ERROR rather than NOT_COVERED, so a wording change in a future TestCov
# fails loudly instead of quietly reporting that nothing covers anything.
testcov_verdict() {
  local log="$1"
  if grep -qE '^Result: TRUE[[:space:]]*$' "$log"; then
    echo COVERED
  elif grep -qE '^Result: UNKNOWN[[:space:]]*$' "$log"; then
    echo NOT_COVERED
  else
    echo ERROR
  fi
}

name=$(basename "$PROGRAM")
prop=$(basename "$PROPERTY")
cp "$PROGRAM" "$WORK/"
cp "$PROPERTY" "$WORK/"

( cd "$WORK" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 "$BUDGET" \
    "$MAP2CHECK" --target-function --target-function-name reach_error \
    --generate-test-suite --property-file "$prop" --architecture 64bit \
    --nondet-generator symex --timeout "$((BUDGET / 2))" "$name" ) \
  > "$WORK/map2check.log" 2>&1

if [ ! -d "$WORK/test-suite" ]; then
  # No violation found means no suite, which for validation purposes is exactly
  # "the error was not covered". Telling that apart from "found the wrong thing"
  # is the tool's job, not the validator's.
  if [ -n "$VERDICT_ONLY" ]; then
    echo NOT_COVERED
    exit 0
  fi
  echo "FAIL $name: no test suite produced"
  tail -3 "$WORK/map2check.log"
  exit 1
fi

( cd "$WORK/test-suite" && zip -q -r ../suite.zip . )

# --no-isolation: no cgroup access on CI runners. -64: TestCov defaults to -m32
# while Map2Check compiles for the host model, and the mismatch surfaces as a
# suite that covers nothing rather than as an error.
( cd "$WORK" && timeout -k 10 "$BUDGET" "$TESTCOV" --test-suite suite.zip \
    --no-isolation -64 --goal "$prop" "$name" ) > "$WORK/testcov.log" 2>&1

verdict=$(testcov_verdict "$WORK/testcov.log")

if [ -n "$VERDICT_ONLY" ]; then
  echo "$verdict"
  [ "$verdict" = "ERROR" ] && exit 1
  exit 0
fi

if [ "$verdict" = "COVERED" ]; then
  echo "PASS $name: TestCov reproduced the error"
  exit 0
fi
echo "FAIL $name: TestCov verdict is $verdict"
tail -20 "$WORK/testcov.log"
exit 1
