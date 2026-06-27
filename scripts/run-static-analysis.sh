#!/bin/bash
# ============================================================
# run-static-analysis.sh — Run cppcheck and clang-tidy locally
#
# Usage:
#   ./scripts/run-static-analysis.sh [--fix]
#
# Requires: cppcheck, clang-tidy-16, compile_commands.json in build/
# Phase 1.5 — OpenSSF Best Practices Badge
# ============================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
FIX_MODE=""

if [[ "${1:-}" == "--fix" ]]; then
    FIX_MODE="--fix"
    echo "Running in fix mode — will auto-fix where possible"
fi

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "============================================"
echo " Map2Check — Static Analysis"
echo "============================================"
echo ""

# ----------------------------------------------------------
# 1. cppcheck
# ----------------------------------------------------------
echo -e "${YELLOW}[1/3] Running cppcheck on C++ sources...${NC}"

CPPCHECK_EXIT=0
cppcheck \
    --enable=warning,performance,portability \
    --suppress=missingIncludeSystem \
    --suppress=unknownMacro \
    --suppress=unusedFunction \
    --suppressions-list="$PROJECT_DIR/.cppcheck-suppressions.txt" \
    --inline-suppr \
    --error-exitcode=2 \
    --std=c++17 \
    --language=c++ \
    -I "$PROJECT_DIR/modules/" \
    "$PROJECT_DIR/modules/backend/library/" \
    "$PROJECT_DIR/modules/frontend/" \
    "$PROJECT_DIR/tests/unit/" \
    2>&1 || CPPCHECK_EXIT=$?

echo -e "${YELLOW}[2/3] Running cppcheck on C sources...${NC}"

cppcheck \
    --enable=warning,performance,portability \
    --suppress=missingIncludeSystem \
    --suppress=unknownMacro \
    --suppress=unusedFunction \
    --suppressions-list="$PROJECT_DIR/.cppcheck-suppressions.txt" \
    --inline-suppr \
    --error-exitcode=2 \
    --std=c11 \
    --language=c \
    -I "$PROJECT_DIR/modules/" \
    "$PROJECT_DIR/modules/backend/library/" \
    2>&1 || CPPCHECK_EXIT=$?

if [[ $CPPCHECK_EXIT -eq 0 ]]; then
    echo -e "${GREEN}✅ cppcheck: No issues found${NC}"
elif [[ $CPPCHECK_EXIT -eq 2 ]]; then
    echo -e "${RED}❌ cppcheck: Issues found (exit code $CPPCHECK_EXIT)${NC}"
else
    echo -e "${RED}❌ cppcheck: Error (exit code $CPPCHECK_EXIT)${NC}"
fi

# ----------------------------------------------------------
# 2. clang-tidy (requires compile_commands.json)
# ----------------------------------------------------------
echo ""
echo -e "${YELLOW}[3/3] Running clang-tidy on C++ sources...${NC}"

if [[ ! -f "$BUILD_DIR/compile_commands.json" ]]; then
    echo -e "${RED}Error: compile_commands.json not found in $BUILD_DIR${NC}"
    echo "Run: cd build && cmake .. -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..."
    exit 1
fi

TIDY_EXIT=0
TIDY_FILES=$(find "$PROJECT_DIR/modules/frontend" -name '*.cpp' 2>/dev/null)
if [[ -z "$TIDY_FILES" ]]; then
    echo "No .cpp files found in modules/frontend"
else
    for f in $TIDY_FILES; do
        echo "  Checking: $(basename "$f")"
        clang-tidy-16 -p "$BUILD_DIR" \
            --config-file="$PROJECT_DIR/.clang-tidy" \
            $FIX_MODE \
            "$f" 2>&1 || TIDY_EXIT=$?
    done
fi

if [[ $TIDY_EXIT -eq 0 ]]; then
    echo -e "${GREEN}✅ clang-tidy: No issues found${NC}"
else
    echo -e "${YELLOW}⚠️  clang-tidy: Warnings found (exit code $TIDY_EXIT)${NC}"
fi

# ----------------------------------------------------------
# Summary
# ----------------------------------------------------------
echo ""
echo "============================================"
echo " Summary"
echo "============================================"
FINAL_EXIT=0
if [[ $CPPCHECK_EXIT -ne 0 ]]; then
    echo -e " cppcheck:   ${RED}FAIL${NC}"
    FINAL_EXIT=1
else
    echo -e " cppcheck:   ${GREEN}PASS${NC}"
fi
if [[ $TIDY_EXIT -ne 0 ]]; then
    echo -e " clang-tidy: ${YELLOW}WARN${NC}"
else
    echo -e " clang-tidy: ${GREEN}PASS${NC}"
fi
echo "============================================"

exit $FINAL_EXIT
