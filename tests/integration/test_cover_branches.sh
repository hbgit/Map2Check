#!/bin/bash
# test_cover_branches.sh -- the Cover-Branches suite, validated by TestCov.
#
# Cover-Error asks for one test case, the violating one. Cover-Branches asks
# for a suite that exercises as many branches as possible, and the difference
# is not a parameter -- it is a different source of input vectors.
#
# The vectors come from KLEE's own .ktest output, one per explored path. The
# alternative, having the instrumented program write a log per state, was tried
# and measured: it turned a 1-second run answering FALSE into a 100-second run
# that exhausted its budget and answered TRUE, because every write is an
# external call KLEE executes concretely. Reading .ktest files after the search
# has finished costs the search nothing.
#
# The assertion that matters is the last one. Well-formed XML proves nothing
# about coverage; only an independent validator can say whether the suite
# actually exercises the branches, and TestCov is that validator.

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
TESTCOV="${TESTCOV:-testcov}"
BUDGET="${BUDGET:-120}"

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

cat > "$WORK/branches.prp" <<'EOF'
COVER( init(main()), FQL(COVER EDGES(@DECISIONEDGE)) )
EOF

# Six decision edges over two independent inputs. A single-test-case suite can
# reach at most half of them, so a tool that quietly fell back to the
# Cover-Error path would show up as ~50% coverage rather than as an error.
cat > "$WORK/branches.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  int r = 0;
  if (x > 10) { r += 1; } else { r += 2; }
  if (y < -5) { r += 4; } else { r += 8; }
  if (x == y) { r += 16; }
  return r;
}
EOF

echo "=== Cover-Branches suite ==="

if ! command -v "$TESTCOV" >/dev/null 2>&1; then
  echo "  SKIP: $TESTCOV not installed (pip3 install testcov)"
  exit 0
fi

( cd "$WORK" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 $((BUDGET + 60)) \
    "$MAP2CHECK" --nondet-generator symex --generate-test-suite --cover-branches \
    --property-file branches.prp --architecture 64bit \
    --timeout "$BUDGET" branches.c ) > "$WORK/m2c.log" 2>&1

n_tests=$(ls "$WORK/test-suite"/testcase-*.xml 2>/dev/null | wc -l)

# More than one, because that is the whole point: Cover-Error emits exactly one.
if [ "$n_tests" -gt 1 ]; then
  ok "the suite holds more than one test case ($n_tests)"
else
  fail "test case count" "got $n_tests, expected several (one per KLEE path)"
  tail -5 "$WORK/m2c.log" | sed 's/^/    /'
  echo "  Results: $PASSED passed, $FAILED failed"
  exit 1
fi

# Distinct vectors. Identical ones would mean the same path was recorded many
# times, which inflates the suite while covering nothing extra.
vectors=$(for f in "$WORK/test-suite"/testcase-*.xml; do
            sed -n 's:.*<input>\(.*\)</input>.*:\1:p' "$f" | tr '\n' ',' ; echo
          done)
n_distinct=$(printf '%s\n' "$vectors" | sort -u | grep -c .)
if [ "$n_distinct" -eq "$n_tests" ]; then
  ok "every test case is a distinct input vector ($n_distinct)"
else
  fail "vector distinctness" "$n_distinct distinct out of $n_tests"
fi

# Cover-Branches test cases are paths, not violations.
if grep -q 'coversError="true"' "$WORK/test-suite"/testcase-*.xml 2>/dev/null; then
  fail "coversError" "a branch test case claims to cover the error"
else
  ok "no branch test case claims coversError"
fi

if grep -q "@DECISIONEDGE" "$WORK/test-suite/metadata.xml"; then
  ok "metadata records the branch-coverage specification"
else
  fail "specification" "metadata does not carry the cover-branches property"
fi

# The one that cannot be faked.
( cd "$WORK/test-suite" && zip -q -r ../suite.zip . )
( cd "$WORK" && timeout -k 10 300 "$TESTCOV" --test-suite suite.zip \
    --no-isolation -64 --goal branches.prp branches.c ) > "$WORK/tc.log" 2>&1

coverage=$(grep -oE '^Coverage: [0-9.]+' "$WORK/tc.log" | tail -1 | awk '{print $2}')
if [ -z "$coverage" ]; then
  fail "TestCov ran" "no Coverage line -- see below"
  tail -15 "$WORK/tc.log" | sed 's/^/    /'
else
  # 100% is what this program should reach; the threshold is set below it so a
  # small change in KLEE's path ordering does not turn into a red build, while
  # a fallback to a one-case suite (~50%) still fails.
  reached=$(python3 -c "print(1 if float('$coverage') >= 80.0 else 0)")
  if [ "$reached" = "1" ]; then
    ok "TestCov measures ${coverage}% branch coverage"
  else
    fail "branch coverage" "only ${coverage}%, expected >= 80%"
  fi
fi

echo "  ---"
echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ] || exit 1
