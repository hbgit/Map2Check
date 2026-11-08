#!/bin/bash
set -e

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
CASTLE_DIR="tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250"
TMPDIR=$(mktemp -d)
PASSED=0
FAILED=0

cleanup() { rm -rf "$TMPDIR"; }
trap cleanup EXIT

echo "============================================================"
echo "Map2Check CASTLE Regression Tests"
echo "============================================================"

run_castle_test() {
  local name="$1"
  local cwe="$2"
  local expect_fail="$3"

  local src="$CASTLE_DIR/$name"
  local bc="$TMPDIR/$name.bc"

  echo ""
  echo "--- Test: $name (CWE-$cwe) ---"

  $CLANG -c -emit-llvm -g -O0 "$src" -o "$bc" 2>/dev/null || {
    echo "  SKIP: compile failed"
    return 0
  }

  local output
  output=$(timeout 120 "$MAP2CHECK" --memtrack --timeout 60 "$bc" 2>&1) || true

  if [ "$expect_fail" = "true" ]; then
    if echo "$output" | grep -qE "VERIFICATION FAILED|FALSE_"; then
      echo "  PASS: vulnerability detected"
      PASSED=$((PASSED+1))
    else
      echo "  FAIL: expected FALSE, got:"
      echo "$output" | tail -5
      FAILED=$((FAILED+1))
    fi
  else
    if echo "$output" | grep -qE "VERIFICATION FAILED|FALSE_"; then
      echo "  FAIL: expected TRUE, got FALSE:"
      echo "$output" | tail -5
      FAILED=$((FAILED+1))
    else
      echo "  PASS: no false alarm"
      PASSED=$((PASSED+1))
    fi
  fi
}

run_castle_test "CASTLE-476-5.c"  "476" "true"
run_castle_test "CASTLE-415-2.c"  "415" "true"
run_castle_test "CASTLE-476-8.c"  "476" "false"
run_castle_test "CASTLE-416-1.c"  "416" "true"
run_castle_test "CASTLE-787-6.c"  "787" "true"

echo ""
echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"

[ $FAILED -eq 0 ] && exit 0 || exit 1
