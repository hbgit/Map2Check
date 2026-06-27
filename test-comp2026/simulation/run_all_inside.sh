#!/bin/bash
# run_all_inside.sh — Runs inside the Docker container
# Executes map2check on all test files from the .set categories
set -e

MAP2CHECK_PATH=/workspace/test-comp2026/simulation/release
SV_BENCH=/workspace/test-comp2026/simulation/sv-benchmarks/c
RESULTS_DIR=/workspace/test-comp2026/simulation/resultados_de_testes
TIMEOUT=300
INNER_TIMEOUT=$((TIMEOUT - 10))

export KLEE_RUNTIME_LIBRARY_PATH="$MAP2CHECK_PATH/lib/klee/runtime"

mkdir -p "$RESULTS_DIR"

CSV="$RESULTS_DIR/summary_report_v8.csv"
echo "category,file,time_s,result,expected" > "$CSV"

TOTAL=0; T_TRUE=0; T_FALSE=0; T_UNKNOWN=0; T_ERROR=0; T_CORRECT=0

run_test() {
    local c_file="$1"
    local category="$2"
    local expected="$3"
    local basename=$(basename "$c_file")

    cd /tmp && rm -rf m2c_run && mkdir -p m2c_run && cd m2c_run

    local start_time=$(date +%s)
    local output
    output=$(timeout "$TIMEOUT" "$MAP2CHECK_PATH/map2check" \
        --timeout "$INNER_TIMEOUT" \
        --smt-solver z3 \
        --target-function \
        --target-function-name reach_error \
        "$c_file" 2>&1 || true)
    local end_time=$(date +%s)
    local elapsed=$((end_time - start_time))

    local verdict="UNKNOWN"
    if echo "$output" | grep -q "VERIFICATION FAILED"; then
        verdict="FALSE"
    elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
        verdict="TRUE"
    fi

    local correct="?"
    if [ "$expected" = "$verdict" ]; then
        correct="OK"
    elif [ "$verdict" = "UNKNOWN" ]; then
        correct="UNK"
    else
        correct="WRONG"
    fi

    printf "  %-45s %-8s %-6s %3ds  [exp: %-5s]\n" "$basename" "$verdict" "$correct" "$elapsed" "$expected"
    echo "$category,$basename,$elapsed,$verdict,$expected" >> "$CSV"

    TOTAL=$((TOTAL + 1))
    case "$verdict" in
        TRUE)    T_TRUE=$((T_TRUE + 1)) ;;
        FALSE)   T_FALSE=$((T_FALSE + 1)) ;;
        *)       T_UNKNOWN=$((T_UNKNOWN + 1)) ;;
    esac
    if [ "$correct" = "OK" ]; then
        T_CORRECT=$((T_CORRECT + 1))
    fi
}

process_category() {
    local cat_name="$1"
    local set_file="$SV_BENCH/${cat_name}.set"

    [ ! -f "$set_file" ] && { echo "[WARN] $set_file not found"; return; }

    echo ""
    echo "============================================================"
    echo "[CAT] $cat_name"
    echo "============================================================"

    local count=0

    while IFS= read -r line; do
        line=$(echo "$line" | xargs)
        [[ -z "$line" || "$line" == \#* ]] && continue

        for yml in $SV_BENCH/$line; do
            [ ! -f "$yml" ] && continue

            # Extract input file
            local input_file
            input_file=$(grep "^input_files:" "$yml" | head -1 | sed "s/.*: *'\\(.*\\)'/\\1/")
            [ -z "$input_file" ] && continue

            local dir=$(dirname "$yml")
            local c_file="$dir/$input_file"
            [ ! -f "$c_file" ] && continue

            # Check for unreach-call property and expected verdict
            if ! grep -q "unreach-call\|coverage-error-call" "$yml"; then
                continue
            fi

            local expected="UNKNOWN"
            if grep -A1 "unreach-call\|coverage-error-call" "$yml" | grep -q "expected_verdict: false"; then
                expected="FALSE"
            elif grep -A1 "unreach-call\|coverage-error-call" "$yml" | grep -q "expected_verdict: true"; then
                expected="TRUE"
            fi

            run_test "$c_file" "$cat_name" "$expected"
            count=$((count + 1))

            # Limit for smoke test mode
            if [ "${SMOKE_TEST:-false}" = "true" ] && [ "$count" -ge 5 ]; then
                echo "  [smoke-test limit reached]"
                return
            fi
        done
    done < "$set_file"

    echo "  [$count tasks completed for $cat_name]"
}

echo "============================================================"
echo "Map2Check v8 — TestComp 2026 Checkpoint"
echo "============================================================"
echo "Timeout: ${TIMEOUT}s | Solver: z3 | Target: reach_error"
echo "KLEE runtime: $KLEE_RUNTIME_LIBRARY_PATH"
echo ""

for cat in Loops Arrays ControlFlow; do
    process_category "$cat"
done

echo ""
echo "============================================================"
echo "SUMMARY"
echo "============================================================"
echo "Total:   $TOTAL"
echo "TRUE:    $T_TRUE"
echo "FALSE:   $T_FALSE"
echo "UNKNOWN: $T_UNKNOWN"
echo "CORRECT: $T_CORRECT / $TOTAL"
echo ""
echo "CSV: $CSV"
