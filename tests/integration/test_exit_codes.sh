#!/bin/bash
# test_exit_codes.sh — the process exit code must reflect how the run ended.
#
# Finding G: main() discarded the return value of map2check_execution() at all
# three call sites, so the exit code was always 0 (or SIGABRT from the
# --expected-result path). Two consequences:
#
#   1. The Test-Comp tool-info (utils/moduleBenchExec/map2check_testcomp.py)
#      decides DONE vs ERROR from `exit_code == 0`, so an internally failed run
#      was reported DONE.
#   2. --expected-result signalled a wrong answer with abort() (SIGABRT, 134),
#      which BenchExec reads as "the tool crashed" -- the opposite of the truth.
#
# The convention this test pins down: exit 0 means "the analysis ran to a
# verdict, whatever it was" (the verdict lives in stdout, which is what SV-COMP
# parses), and non-zero means "the run itself failed or disagreed with the
# harness", with --expected-result getting its own code (4).

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

cat > "$TMPDIR/ov_bug.c" <<'EOF'
#include <limits.h>
int main(void) { int x = 2147483647; int y = x + 1; return y; }
EOF

"$CLANG" -c -emit-llvm -g -O0 "$TMPDIR/ov_bug.c" -o "$TMPDIR/ov_bug.bc" 2>/dev/null || {
  echo "  SKIP: compile failed"
  exit 1
}

echo "=== Exit-code contract ==="

# 1. A definitive FALSE is still exit 0 -- the verdict is in stdout, not the code.
output=$("$MAP2CHECK" --nondet-generator symex --check-overflow --timeout 60 \
  "$TMPDIR/ov_bug.bc" 2>&1)
rc=$?
if [ "$rc" -eq 0 ] && echo "$output" | grep -qE "VERIFICATION FAILED|OVERFLOW"; then
  ok "FALSE verdict keeps exit code 0 (verdict in stdout)"
else
  fail "FALSE exit code" "rc=$rc (want 0 with FALSE on stdout)"
  echo "$output" | tail -5
fi

# 2. A matching --expected-result is exit 0.
output=$("$MAP2CHECK" --nondet-generator symex --check-overflow --timeout 60 \
  --expected-result OVERFLOW "$TMPDIR/ov_bug.bc" 2>&1)
rc=$?
if [ "$rc" -eq 0 ]; then
  ok "--expected-result OVERFLOW (matching) exits 0"
else
  fail "matching expected-result" "rc=$rc (want 0)"
fi

# 3. A mismatching --expected-result is a distinct exit code, not SIGABRT.
output=$("$MAP2CHECK" --nondet-generator symex --check-overflow --timeout 60 \
  --expected-result FALSE-FREE "$TMPDIR/ov_bug.bc" 2>&1)
rc=$?
if [ "$rc" -eq 4 ]; then
  ok "--expected-result mismatch exits 4 (not SIGABRT 134)"
elif [ "$rc" -eq 134 ]; then
  fail "mismatching expected-result" "rc=134 (SIGABRT) -- abort() still in place"
else
  fail "mismatching expected-result" "rc=$rc (want 4)"
fi

echo "=== Results: $PASSED passed, $FAILED failed ==="
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
