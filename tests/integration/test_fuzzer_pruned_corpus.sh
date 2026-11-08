#!/bin/bash
# test_fuzzer_pruned_corpus.sh -- the fuzzer's corpus reaches the Cover-Branches
# suite even when every path ends in a failed assumption.
#
# A reactive program (`while (1)` reading inputs under assume_abort_if_not,
# the shape of the Hardness tasks) never returns from main: each execution
# ends when an input violates an assumption. The witness replay that turns a
# queue entry into a test case used to longjmp out on that assumption without
# writing the nondet log, so every entry was dropped -- in the Test-Comp 2027
# pre-run the median Hardness suite had 2 tests while AFL++ held hundreds.

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

echo "=== Fuzzer corpus on assumption-pruned paths ==="

cat > "$WORK/task.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void abort(void);
void assume_abort_if_not(int cond) { if (!cond) abort(); }
int state = 0;
int main(void) {
  while (1) {
    int x = __VERIFIER_nondet_int();
    assume_abort_if_not(x > 0 && x < 100);
    if (x > 50) state++;
    else if (x > 25) state += 2;
    else state--;
  }
  return 0;
}
EOF

(cd "$WORK" && MAP2CHECK_AFL_EXIT_ON_TIME=10 \
   "$MAP2CHECK_DIR/map2check" --cover-branches --generate-test-suite \
   --nondet-generator afl --timeout 40 task.c > map2check.out 2>&1)

n=$(grep -o 'Test suite: [0-9]* test cases from the fuzzer' "$WORK/map2check.out" \
      | grep -o '[0-9]*' | tail -1)
if [ "${n:-0}" -gt 0 ]; then
  ok "the fuzzer's corpus is in the suite ($n test cases)"
else
  fail "the fuzzer's corpus is in the suite" "none (see below)"
  grep -E 'AFL\+\+ ran|Test suite' "$WORK/map2check.out"
fi

echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ]
