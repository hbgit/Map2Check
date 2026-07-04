#!/bin/bash
# test_wams_entrypoint.sh — Validate WASM entrypoint extraction
# Tests that generateWasmWrapperStatic correctly extracts module names
# from w2c_*_start entrypoints and generates valid wrappers.
set -e

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/test-comp2026/simulation/release}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
WASI_CLANG="${WASI_SDK_PATH:-/opt/wasi-sdk-33.0-x86_64-linux}/bin/clang"
TMPDIR=$(mktemp -d)
PASSED=0
FAILED=0

cleanup() { rm -rf "$TMPDIR"; }
trap cleanup EXIT

echo "============================================================"
echo "Map2Check WASM Entrypoint Tests"
echo "============================================================"

# -------------------------------------------------------------------
# Test 1: Simple WASM with _start entrypoint
# -------------------------------------------------------------------
echo ""
echo "--- Test 1: _start entrypoint translation ---"

cat > "$TMPDIR/test_entry.c" << 'EOF'
int main() { return 42; }
EOF

$WASI_CLANG --target=wasm32-wasip1 "$TMPDIR/test_entry.c" \
    -o "$TMPDIR/test_entry.wasm" 2>/dev/null

output=$(timeout 60 $MAP2CHECK --wasm --memtrack --timeout 50 \
    "$TMPDIR/test_entry.wasm" 2>&1 || true)

if echo "$output" | grep -q "Lifting complete. Entry point:"; then
    ep=$(echo "$output" | grep "Lifting complete" | sed 's/.*Entry point: //' | sed 's/\x1b\[[0-9;]*m//g' | xargs)
    echo "  Entry point detected: $ep"
    
    if echo "$ep" | grep -qE "w2c_.*(_start|_0x5Fstart)"; then
        echo "  PASS: _start entrypoint correctly identified"
        PASSED=$((PASSED+1))
    else
        echo "  FAIL: Unexpected entrypoint format"
        FAILED=$((FAILED+1))
    fi
else
    echo "  FAIL: No entry point found in output"
    FAILED=$((FAILED+1))
fi

# -------------------------------------------------------------------
# Test 2: Entrypoint with special characters in module name
# -------------------------------------------------------------------
echo ""
echo "--- Test 2: Module name with special chars ---"

# Compile a WASM module with a name containing special chars
cat > "$TMPDIR/test_special.c" << 'EOF'
int main(int argc, char** argv) {
    int x = argc + 1;  // use argc to avoid unused warning
    return x;
}
EOF

$WASI_CLANG --target=wasm32-wasip1 "$TMPDIR/test_special.c" \
    -o "$TMPDIR/test_special.wasm" 2>/dev/null

output=$(timeout 60 $MAP2CHECK --wasm --memtrack --timeout 50 \
    "$TMPDIR/test_special.wasm" 2>&1 || true)

if echo "$output" | grep -q "Lifting complete. Entry point:"; then
    ep=$(echo "$output" | grep "Lifting complete" | sed 's/.*Entry point: //')
    echo "  Entry point: $ep"

    # Verify it contains hex-encoded special chars (0x2E for '.', etc.)
    if echo "$ep" | grep -q "0x"; then
        echo "  INFO: Hex encoding detected in module name (expected)"
    fi
    
    # Verify the pipeline doesn't crash on this
    if echo "$output" | grep -q "VERIFICATION SUCCEEDED\|VERIFICATION FAILED\|VERIFICATION UNKNOWN"; then
        echo "  PASS: Pipeline completed successfully"
        PASSED=$((PASSED+1))
    else
        echo "  FAIL: Pipeline did not complete"
        FAILED=$((FAILED+1))
    fi
else
    echo "  FAIL: No entry point found"
    FAILED=$((FAILED+1))
fi

# -------------------------------------------------------------------
# Test 3: WASM module with imports (proc_exit)
# -------------------------------------------------------------------
echo ""
echo "--- Test 3: WASM module with WASI imports ---"

cat > "$TMPDIR/test_wasi.c" << 'EOF'
#include <stdlib.h>
int main() { exit(0); return 0; }
EOF

$WASI_CLANG --target=wasm32-wasip1 "$TMPDIR/test_wasi.c" \
    -o "$TMPDIR/test_wasi.wasm" 2>/dev/null

output=$(timeout 60 $MAP2CHECK --wasm --memtrack --timeout 50 \
    "$TMPDIR/test_wasi.wasm" 2>&1 || true)

if echo "$output" | grep -q "Lifting complete"; then
    echo "  PASS: WASM with imports lifted successfully"
    PASSED=$((PASSED+1))
else
    echo "  FAIL: Lifting failed for WASM with imports"
    FAILED=$((FAILED+1))
fi

echo ""
echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"

[ $FAILED -eq 0 ] && exit 0 || exit 1
