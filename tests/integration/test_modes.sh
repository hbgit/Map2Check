#!/bin/bash
# test_modes.sh — regression test for the non-memtrack analysis modes.
#
# Bug caught: caller.cpp passed -m2c-entry-function to opt unconditionally, but
# that cl::opt is registered only by MemoryTrackPass. In --check-overflow,
# --check-asserts and --target-function modes (which do not load MemoryTrackPass)
# opt aborted with "Unknown command line argument '-m2c-entry-function'", so main
# was never renamed and every program was reported ERROR/UNKNOWN.
#
# This test exercises each mode with a buggy and a safe program and asserts a
# definitive verdict (FALSE / TRUE), so any such regression fails the build.

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

PASSED=0
FAILED=0

cat > "$TMPDIR/ov_bug.c" <<'EOF'
#include <limits.h>
int main(void) { int x = 2147483647; int y = x + 1; return y; }
EOF

cat > "$TMPDIR/ov_safe.c" <<'EOF'
int main(void) { int x = 1; int y = x + 1; return y; }
EOF

cat > "$TMPDIR/as_bug.c" <<'EOF'
extern void __VERIFIER_assert(int);
int main(void) { int x = 0; __VERIFIER_assert(x > 0); return 0; }
EOF

cat > "$TMPDIR/as_safe.c" <<'EOF'
extern void __VERIFIER_assert(int);
int main(void) { int x = 1; __VERIFIER_assert(x > 0); return 0; }
EOF

cat > "$TMPDIR/tg_bug.c" <<'EOF'
extern void __VERIFIER_error(void);
int main(void) { __VERIFIER_error(); return 0; }
EOF

cat > "$TMPDIR/tg_safe.c" <<'EOF'
extern void __VERIFIER_error(void);
int main(void) { return 0; }
EOF

run() {
    local src="$1" mode="$2" expect="$3" name="$4"
    local bc="$TMPDIR/$name.bc"

    "$CLANG" -c -emit-llvm -g -O0 "$src" -o "$bc" 2>/dev/null || {
        echo "  SKIP $name: compile failed"
        return
    }

    local output
    output=$(timeout 120 "$MAP2CHECK" $mode --timeout 60 "$bc" 2>&1) || true

    if [ "$expect" = "FALSE" ]; then
        if echo "$output" | grep -qE "VERIFICATION FAILED|FALSE_"; then
            echo "  PASS $name [$mode]: vulnerability detected"
            PASSED=$((PASSED+1))
        else
            echo "  FAIL $name [$mode]: expected FALSE, got:"
            echo "$output" | tail -5
            FAILED=$((FAILED+1))
        fi
    else
        if echo "$output" | grep -qE "VERIFICATION FAILED|FALSE_"; then
            echo "  FAIL $name [$mode]: expected TRUE, got FALSE"
            FAILED=$((FAILED+1))
        elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
            echo "  PASS $name [$mode]: no false alarm"
            PASSED=$((PASSED+1))
        else
            echo "  FAIL $name [$mode]: expected TRUE, got UNKNOWN/ERROR:"
            echo "$output" | tail -5
            FAILED=$((FAILED+1))
        fi
    fi
}

echo "============================================================"
echo "Map2Check Non-Memtrack Modes Regression"
echo "============================================================"
run "$TMPDIR/ov_bug.c" "--check-overflow" FALSE overflow_bug
run "$TMPDIR/ov_safe.c" "--check-overflow" TRUE overflow_safe
run "$TMPDIR/as_bug.c" "--check-asserts" FALSE assert_bug
run "$TMPDIR/as_safe.c" "--check-asserts" TRUE assert_safe
run "$TMPDIR/tg_bug.c" "--target-function --target-function-name __VERIFIER_error" FALSE target_bug
run "$TMPDIR/tg_safe.c" "--target-function --target-function-name __VERIFIER_error" TRUE target_safe
echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"

[ "$FAILED" -eq 0 ] && exit 0 || exit 1
