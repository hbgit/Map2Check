#!/bin/bash
# run_heap_inside.sh — Run C.coverage-error-call.Heap inside a single Docker container
# Follows TestComp 2026 guidelines for the Heap subcategory
# Usage: This script runs INSIDE the Docker container via:
#   docker run --rm -v $(pwd):/workspace map2check-dev bash /workspace/test-comp2026/simulation/run_heap_inside.sh
set -e

MAP2CHECK_PATH=/workspace/test-comp2026/simulation/release
SV_BENCH=/workspace/test-comp2026/simulation/sv-benchmarks/c
RESULTS_DIR=/workspace/test-comp2026/simulation/resultados_de_testes/heap_coverage_error_call
TIMEOUT=300
INNER_TIMEOUT=$((TIMEOUT - 10))
PROP_FILE=/workspace/test-comp2026/simulation/coverage-error-call.prp

export KLEE_RUNTIME_LIBRARY_PATH="$MAP2CHECK_PATH/lib/klee/runtime"
export MAP2CHECK_PATH

mkdir -p "$RESULTS_DIR"

CSV="$RESULTS_DIR/heap_results_v8.csv"
echo "set_file,subdir,file,time_s,result" > "$CSV"

TOTAL=0; T_FALSE=0; T_TRUE=0; T_UNKNOWN=0; T_TIMEOUT=0; T_ERROR=0
SCORE=0

# Update binaries from build
echo "[SETUP] Updating release binaries from build..."
cp /workspace/build/modules/frontend/map2check $MAP2CHECK_PATH/map2check 2>/dev/null || true
cp /workspace/build/modules/backend/pass/lib*.so $MAP2CHECK_PATH/lib/ 2>/dev/null || true
mkdir -p $MAP2CHECK_PATH/lib/klee/runtime
cp /opt/klee/lib/klee/runtime/*.bca $MAP2CHECK_PATH/lib/klee/runtime/ 2>/dev/null || true
chmod +x $MAP2CHECK_PATH/map2check

run_task() {
    local c_file="$1"
    local set_name="$2"
    local subdir="$3"
    local basename=$(basename "$c_file")

    cd /tmp && rm -rf m2c_heap_run && mkdir -p m2c_heap_run && cd m2c_heap_run

    local start_time=$(date +%s)

    # Use the wrapper (which handles the full pipeline)
    local output
    output=$(cd $MAP2CHECK_PATH && timeout "$TIMEOUT" python3 map2check-wrapper.py \
        -p "$PROP_FILE" "$c_file" 2>&1 || true)
    local exit_code=$?

    local end_time=$(date +%s)
    local elapsed=$((end_time - start_time))

    # Parse result from wrapper output
    local result="UNKNOWN"
    while IFS= read -r line; do
        line=$(echo "$line" | xargs)
        case "$line" in
            FALSE|FALSE_FREE|FALSE_DEREF|FALSE_MEMTRACK|FALSE_MEMCLEANUP|FALSE_OVERFLOW)
                result="$line" ;;
            TRUE)
                result="TRUE" ;;
            UNKNOWN)
                result="UNKNOWN" ;;
            "Timed out")
                result="TIMEOUT" ;;
        esac
    done <<< "$output"

    # If elapsed >= TIMEOUT, mark as timeout
    if [ "$elapsed" -ge "$TIMEOUT" ] && [ "$result" = "UNKNOWN" ]; then
        result="TIMEOUT"
    fi

    printf "  %-55s %-12s %4ds\n" "$subdir/$basename" "$result" "$elapsed"
    echo "$set_name,$subdir,$basename,$elapsed,$result" >> "$CSV"

    TOTAL=$((TOTAL + 1))
    case "$result" in
        FALSE*)  T_FALSE=$((T_FALSE + 1)); SCORE=$((SCORE + 1)) ;;
        TRUE)    T_TRUE=$((T_TRUE + 1)) ;;
        TIMEOUT) T_TIMEOUT=$((T_TIMEOUT + 1)) ;;
        UNKNOWN) T_UNKNOWN=$((T_UNKNOWN + 1)) ;;
        *)       T_ERROR=$((T_ERROR + 1)) ;;
    esac
}

process_set() {
    local set_file="$1"
    local set_name=$(basename "$set_file" .set)

    [ ! -f "$set_file" ] && { echo "[WARN] $set_file not found"; return; }

    echo ""
    echo "============================================================"
    echo "[SET] $set_name ($set_file)"
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

            local subdir_name=$(basename "$dir")

            # Check if program has reach_error (required for coverage-error-call)
            if ! grep -q "reach_error" "$c_file" 2>/dev/null; then
                # Some tasks define reach_error via extern or in included headers
                # Still run them — the wrapper handles this
                :
            fi

            run_task "$c_file" "$set_name" "$subdir_name"
            count=$((count + 1))
        done
    done < "$set_file"

    echo "  [$count tasks completed for $set_name]"
}

echo "============================================================"
echo "Map2Check v8 — TestComp 2026"
echo "Category: C.coverage-error-call.Heap"
echo "============================================================"
echo "Timeout: ${TIMEOUT}s | Solver: z3 | Target: reach_error"
echo "KLEE runtime: $KLEE_RUNTIME_LIBRARY_PATH"
echo "Start time: $(date)"
echo ""

START_GLOBAL=$(date +%s)

# Process Heap.set
process_set "$SV_BENCH/Heap.set"

# Process LinkedLists.set
process_set "$SV_BENCH/LinkedLists.set"

END_GLOBAL=$(date +%s)
ELAPSED_GLOBAL=$((END_GLOBAL - START_GLOBAL))

echo ""
echo "============================================================"
echo "SUMMARY — C.coverage-error-call.Heap"
echo "============================================================"
echo "Total tasks:    $TOTAL"
echo "FALSE (bug):    $T_FALSE"
echo "TRUE:           $T_TRUE"
echo "UNKNOWN:        $T_UNKNOWN"
echo "TIMEOUT:        $T_TIMEOUT"
echo "ERROR:          $T_ERROR"
echo "Score:          $SCORE / $TOTAL"
echo "Total time:     ${ELAPSED_GLOBAL}s ($(( ELAPSED_GLOBAL / 3600 ))h $(( (ELAPSED_GLOBAL % 3600) / 60 ))m)"
echo "End time:       $(date)"
echo ""
echo "CSV: $CSV"
