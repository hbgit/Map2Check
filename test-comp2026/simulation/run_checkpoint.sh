#!/bin/bash
# run_checkpoint.sh — Run TestComp 2026 checkpoint inside Docker
# Usage: ./run_checkpoint.sh [--smoke-test]

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
RESULTS_DIR="$SCRIPT_DIR/resultados_de_testes"
RELEASE_DIR="$SCRIPT_DIR/release"
SV_BENCH="$SCRIPT_DIR/sv-benchmarks/c"
TIMEOUT=300
SMOKE_TEST=false

if [ "$1" = "--smoke-test" ]; then
    SMOKE_TEST=true
fi

mkdir -p "$RESULTS_DIR"

echo "============================================================"
echo "TestComp 2026 Checkpoint — Map2Check v8 (LLVM 16)"
echo "============================================================"
echo "Project root: $PROJECT_ROOT"
echo "Smoke test:   $SMOKE_TEST"
echo "Timeout:      ${TIMEOUT}s per task"
echo ""

# Step 1: Update release binaries inside Docker
echo "[SETUP] Updating release binaries..."
docker run --rm -v "$PROJECT_ROOT":/workspace map2check-dev bash -c '
MAP2CHECK_PATH=/workspace/test-comp2026/simulation/release
cp /workspace/build/modules/frontend/map2check $MAP2CHECK_PATH/map2check
cp /workspace/build/modules/backend/pass/lib*.so $MAP2CHECK_PATH/lib/
mkdir -p $MAP2CHECK_PATH/lib/klee/runtime
cp /opt/klee/lib/klee/runtime/*.bca $MAP2CHECK_PATH/lib/klee/runtime/
chmod +x $MAP2CHECK_PATH/map2check
echo "Done."
'

# Step 2: Run tests
run_test() {
    local c_file="$1"
    local category="$2"
    local prop="$3"
    local basename=$(basename "$c_file")

    # Run inside Docker
    local result
    result=$(docker run --rm \
        -v "$PROJECT_ROOT":/workspace \
        -e KLEE_RUNTIME_LIBRARY_PATH=/workspace/test-comp2026/simulation/release/lib/klee/runtime \
        map2check-dev bash -c "
            cd /tmp && mkdir -p m2c_run && cd m2c_run
            timeout ${TIMEOUT} /workspace/test-comp2026/simulation/release/map2check \
                --timeout $((TIMEOUT - 10)) \
                --smt-solver z3 \
                --target-function \
                --target-function-name reach_error \
                /workspace/test-comp2026/simulation/$c_file 2>&1 | tail -3
        " 2>/dev/null || echo "DOCKER_ERROR")

    # Parse result
    local verdict="UNKNOWN"
    if echo "$result" | grep -q "VERIFICATION FAILED"; then
        verdict="FALSE"
    elif echo "$result" | grep -q "VERIFICATION SUCCEEDED"; then
        verdict="TRUE"
    elif echo "$result" | grep -q "VERIFICATION UNKNOWN"; then
        verdict="UNKNOWN"
    elif echo "$result" | grep -q "DOCKER_ERROR"; then
        verdict="ERROR"
    fi

    echo "$verdict"
}

# Collect .c files from .set categories
collect_files() {
    local set_file="$1"
    local files=()

    while IFS= read -r line; do
        line=$(echo "$line" | xargs)  # trim
        [[ -z "$line" || "$line" == \#* ]] && continue

        # Expand glob
        for yml in $SV_BENCH/$line; do
            if [ -f "$yml" ] && [ -s "$yml" ]; then
                # Extract input_files from yml
                local input_file
                input_file=$(grep "input_files:" "$yml" | head -1 | sed "s/.*: *'\\(.*\\)'/\\1/")
                if [ -n "$input_file" ]; then
                    local dir=$(dirname "$yml")
                    local rel_dir=$(echo "$dir" | sed "s|.*sv-benchmarks/c/||")
                    # Check if it has coverage-error-call property
                    if grep -q "coverage-error-call" "$yml"; then
                        files+=("sv-benchmarks/c/${rel_dir}/${input_file}")
                    fi
                fi
            fi
        done
    done < "$set_file"

    printf '%s\n' "${files[@]}"
}

# Categories
CATEGORIES=("Loops" "Arrays" "ControlFlow")
TOTAL_TASKS=0
TOTAL_TRUE=0
TOTAL_FALSE=0
TOTAL_UNKNOWN=0
TOTAL_ERROR=0
CSV_FILE="$RESULTS_DIR/summary_report_v8.csv"
echo "category,file,time_s,result" > "$CSV_FILE"

for cat in "${CATEGORIES[@]}"; do
    set_file="$SV_BENCH/${cat}.set"
    if [ ! -f "$set_file" ]; then
        echo "[WARN] Set file not found: $set_file"
        continue
    fi

    echo ""
    echo "============================================================"
    echo "[CAT] $cat (cover-error-call)"
    echo "============================================================"

    cat_dir="$RESULTS_DIR/$cat"
    mkdir -p "$cat_dir"

    mapfile -t test_files < <(collect_files "$set_file")
    echo "[INFO] Found ${#test_files[@]} tasks with coverage-error-call"

    task_count=0
    for c_file in "${test_files[@]}"; do
        [ -z "$c_file" ] && continue
        full_path="$SCRIPT_DIR/$c_file"
        [ ! -f "$full_path" ] && continue

        basename_file=$(basename "$c_file")
        printf "[RUN] %-50s " "$basename_file"

        start_time=$(date +%s)
        verdict=$(run_test "$c_file" "$cat" "coverage-error-call")
        end_time=$(date +%s)
        elapsed=$((end_time - start_time))

        printf "%-10s (%ds)\n" "$verdict" "$elapsed"

        echo "$cat,$basename_file,$elapsed,$verdict" >> "$CSV_FILE"
        echo "$basename_file: ${elapsed}s, Result: $verdict" >> "$cat_dir/tempos.txt"

        TOTAL_TASKS=$((TOTAL_TASKS + 1))
        case "$verdict" in
            TRUE)    TOTAL_TRUE=$((TOTAL_TRUE + 1)) ;;
            FALSE)   TOTAL_FALSE=$((TOTAL_FALSE + 1)) ;;
            UNKNOWN) TOTAL_UNKNOWN=$((TOTAL_UNKNOWN + 1)) ;;
            ERROR)   TOTAL_ERROR=$((TOTAL_ERROR + 1)) ;;
        esac

        task_count=$((task_count + 1))
        if [ "$SMOKE_TEST" = true ] && [ "$task_count" -ge 5 ]; then
            echo "[INFO] Smoke test limit (5 tasks per category). Stopping."
            break
        fi
    done
done

echo ""
echo "============================================================"
echo "SUMMARY REPORT"
echo "============================================================"
echo "Total tasks:  $TOTAL_TASKS"
echo "TRUE:         $TOTAL_TRUE"
echo "FALSE:        $TOTAL_FALSE"
echo "UNKNOWN:      $TOTAL_UNKNOWN"
echo "ERROR:        $TOTAL_ERROR"
echo "Report saved: $CSV_FILE"
