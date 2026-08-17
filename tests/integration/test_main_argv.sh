#!/bin/bash
# test_main_argv.sh — regression test for main(int argc, char *argv[]) support.
#
# Bug fixed: NonDetGeneratorKlee.c / NonDetGeneratorLibFuzzy.c used to call
# __map2check_main__() with no arguments, so any program whose main takes
# (argc, argv) hit KLEE's "calling function with too few arguments" error and
# was reported UNKNOWN (safe programs could never be verified TRUE).
#
# This test compiles a buggy and a safe program, both using main(argc, argv),
# and asserts the verdicts are FALSE and TRUE respectively.

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

PASSED=0
FAILED=0

cat > "$TMPDIR/argv_bug.c" <<'EOF'
/* NULL dereference, always reached, in a main(argc, argv) program */
int main(int argc, char *argv[]) {
    int *a = 0;
    (void)argc;
    (void)argv;
    return *a;
}
EOF

cat > "$TMPDIR/argv_safe.c" <<'EOF'
/* No bug, in a main(argc, argv) program */
int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return 0;
}
EOF

run() {
    local src="$1" expect="$2" name="$3"
    local bc="$TMPDIR/$name.bc"

    "$CLANG" -c -emit-llvm -g -O0 "$src" -o "$bc" 2>/dev/null || {
        echo "  SKIP $name: compile failed"
        return
    }

    local output
    output=$(timeout 120 "$MAP2CHECK" --memtrack --timeout 60 "$bc" 2>&1) || true

    if [ "$expect" = "FALSE" ]; then
        if echo "$output" | grep -qE "VERIFICATION FAILED|FALSE_"; then
            echo "  PASS $name: vulnerability detected"
            PASSED=$((PASSED+1))
        else
            echo "  FAIL $name: expected FALSE, got:"
            echo "$output" | tail -5
            FAILED=$((FAILED+1))
        fi
    else
        if echo "$output" | grep -qE "VERIFICATION FAILED|FALSE_"; then
            echo "  FAIL $name: expected TRUE, got FALSE"
            FAILED=$((FAILED+1))
        elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
            echo "  PASS $name: no false alarm"
            PASSED=$((PASSED+1))
        else
            echo "  FAIL $name: expected TRUE, got UNKNOWN"
            echo "$output" | tail -5
            FAILED=$((FAILED+1))
        fi
    fi
}

echo "============================================================"
echo "Map2Check main(argc, argv) Regression"
echo "============================================================"
run "$TMPDIR/argv_bug.c" FALSE argv_bug
run "$TMPDIR/argv_safe.c" TRUE argv_safe
echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"

[ "$FAILED" -eq 0 ] && exit 0 || exit 1
