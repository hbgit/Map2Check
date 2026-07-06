#!/bin/bash
# run_heap_resume.sh — Resume C.coverage-error-call.Heap from where it stopped
# Reads existing CSV to skip completed tasks, then verifies verdicts at the end
# Usage: docker run --rm -v $(pwd):/workspace map2check-dev bash /workspace/test-comp2026/simulation/run_heap_resume.sh
set -e

MAP2CHECK_PATH=/workspace/test-comp2026/simulation/release
SV_BENCH=/workspace/test-comp2026/simulation/sv-benchmarks/c
RESULTS_DIR=/workspace/test-comp2026/simulation/resultados_de_testes/heap_coverage_error_call
TIMEOUT=300
PROP_FILE=/workspace/test-comp2026/simulation/coverage-error-call.prp

export KLEE_RUNTIME_LIBRARY_PATH="$MAP2CHECK_PATH/lib/klee/runtime"
export MAP2CHECK_PATH

mkdir -p "$RESULTS_DIR"

CSV="$RESULTS_DIR/heap_results_v8.csv"

# === RESUME LOGIC: Build set of already-completed tasks ===
declare -A COMPLETED
SKIPPED=0
if [ -f "$CSV" ]; then
    while IFS=, read -r set_name subdir file_name rest; do
        [ "$set_name" = "set_file" ] && continue  # skip header
        COMPLETED["${set_name}::${subdir}::${file_name}"]=1
    done < "$CSV"
    echo "[RESUME] Found ${#COMPLETED[@]} already-completed tasks in CSV"
else
    echo "set_file,subdir,file,time_s,result" > "$CSV"
    echo "[RESUME] No existing CSV, starting fresh"
fi

TOTAL=0; T_FALSE=0; T_TRUE=0; T_UNKNOWN=0; T_TIMEOUT=0; T_ERROR=0; NEW_TASKS=0
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

    # Skip if already completed
    if [ -n "${COMPLETED[${set_name}::${subdir}::${basename}]}" ]; then
        SKIPPED=$((SKIPPED + 1))
        return
    fi

    cd /tmp && rm -rf m2c_heap_run && mkdir -p m2c_heap_run && cd m2c_heap_run

    local start_time=$(date +%s)

    local output
    output=$(cd $MAP2CHECK_PATH && timeout "$TIMEOUT" python3 map2check-wrapper.py \
        -p "$PROP_FILE" "$c_file" 2>&1 || true)

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

    if [ "$elapsed" -ge "$TIMEOUT" ] && [ "$result" = "UNKNOWN" ]; then
        result="TIMEOUT"
    fi

    printf "  %-55s %-12s %4ds\n" "$subdir/$basename" "$result" "$elapsed"
    echo "$set_name,$subdir,$basename,$elapsed,$result" >> "$CSV"

    TOTAL=$((TOTAL + 1))
    NEW_TASKS=$((NEW_TASKS + 1))
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

            local input_file
            input_file=$(grep "^input_files:" "$yml" | head -1 | sed "s/.*: *'\\(.*\\)'/\\1/")
            [ -z "$input_file" ] && continue

            local dir=$(dirname "$yml")
            local c_file="$dir/$input_file"
            [ ! -f "$c_file" ] && continue

            local subdir_name=$(basename "$dir")

            run_task "$c_file" "$set_name" "$subdir_name"
            count=$((count + 1))
        done
    done < "$set_file"

    echo "  [$count tasks processed for $set_name, $SKIPPED skipped (already done)]"
}

echo "============================================================"
echo "Map2Check v8 — TestComp 2026 (RESUME)"
echo "Category: C.coverage-error-call.Heap"
echo "============================================================"
echo "Timeout: ${TIMEOUT}s | Solver: z3 | Target: reach_error"
echo "KLEE runtime: $KLEE_RUNTIME_LIBRARY_PATH"
echo "Resuming from: ${#COMPLETED[@]} completed tasks"
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
echo "EXECUTION SUMMARY (this session)"
echo "============================================================"
echo "New tasks run:  $NEW_TASKS"
echo "Skipped (prev): $SKIPPED"
echo "FALSE (new):    $T_FALSE"
echo "TRUE (new):     $T_TRUE"
echo "UNKNOWN (new):  $T_UNKNOWN"
echo "TIMEOUT (new):  $T_TIMEOUT"
echo "ERROR (new):    $T_ERROR"
echo "Session time:   ${ELAPSED_GLOBAL}s ($(( ELAPSED_GLOBAL / 3600 ))h $(( (ELAPSED_GLOBAL % 3600) / 60 ))m)"
echo ""

# ============================================================
# VERDICT VERIFICATION against sv-benchmarks expected_verdict
# ============================================================
echo "============================================================"
echo "VERDICT VERIFICATION"
echo "============================================================"

VERIFY_CSV="$RESULTS_DIR/verdict_verification.csv"
echo "set_name,subdir,file,map2check_result,expected_verdict,match" > "$VERIFY_CSV"

CORRECT=0; INCORRECT=0; UNVERIFIABLE=0; TOTAL_VERIFY=0

while IFS=, read -r set_name subdir file_name time_s result; do
    [ "$set_name" = "set_file" ] && continue

    # Find the yml file for this task
    yml_file=""
    for candidate in "$SV_BENCH/$subdir"/*.yml; do
        [ ! -f "$candidate" ] && continue
        inp=$(grep "^input_files:" "$candidate" | head -1 | sed "s/.*: *'\\(.*\\)'/\\1/")
        if [ "$inp" = "$file_name" ]; then
            yml_file="$candidate"
            break
        fi
    done

    if [ -z "$yml_file" ]; then
        echo "$set_name,$subdir,$file_name,$result,N/A,UNVERIFIABLE" >> "$VERIFY_CSV"
        UNVERIFIABLE=$((UNVERIFIABLE + 1))
        TOTAL_VERIFY=$((TOTAL_VERIFY + 1))
        continue
    fi

    # Extract expected_verdict for unreach-call (coverage-error-call inverse)
    # unreach-call: false => reach_error IS reachable => coverage-error-call expects FALSE (bug found)
    # unreach-call: true  => reach_error NOT reachable => coverage-error-call expects TRUE
    expected=$(grep -A1 "unreach-call.prp" "$yml_file" | grep "expected_verdict:" | head -1 | awk '{print $2}')

    TOTAL_VERIFY=$((TOTAL_VERIFY + 1))

    if [ -z "$expected" ]; then
        # No expected verdict for unreach-call — might be coverage-only task
        echo "$set_name,$subdir,$file_name,$result,N/A,UNVERIFIABLE" >> "$VERIFY_CSV"
        UNVERIFIABLE=$((UNVERIFIABLE + 1))
        continue
    fi

    # Map expected to our result space:
    #   expected=false (unreach-call) => bug IS reachable => map2check should say FALSE
    #   expected=true  (unreach-call) => no bug => map2check should say TRUE (or UNKNOWN/TIMEOUT is not wrong, just inconclusive)
    local match="INCONCLUSIVE"
    case "$result" in
        FALSE*)
            if [ "$expected" = "false" ]; then
                match="CORRECT"
                CORRECT=$((CORRECT + 1))
            else
                match="INCORRECT"
                INCORRECT=$((INCORRECT + 1))
            fi
            ;;
        TRUE)
            if [ "$expected" = "true" ]; then
                match="CORRECT"
                CORRECT=$((CORRECT + 1))
            else
                match="INCORRECT"
                INCORRECT=$((INCORRECT + 1))
            fi
            ;;
        UNKNOWN|TIMEOUT)
            # Not wrong, just inconclusive
            match="INCONCLUSIVE"
            UNVERIFIABLE=$((UNVERIFIABLE + 1))
            ;;
        *)
            match="INCONCLUSIVE"
            UNVERIFIABLE=$((UNVERIFIABLE + 1))
            ;;
    esac

    echo "$set_name,$subdir,$file_name,$result,$expected,$match" >> "$VERIFY_CSV"
done < "$CSV"

echo ""
echo "Total verified:     $TOTAL_VERIFY"
echo "CORRECT:            $CORRECT"
echo "INCORRECT:          $INCORRECT"
echo "INCONCLUSIVE/N/A:   $UNVERIFIABLE"
echo ""
echo "Accuracy (excl. inconclusive): $CORRECT / $((CORRECT + INCORRECT))"
echo ""
echo "Verdict CSV: $VERIFY_CSV"
echo ""

# Print any INCORRECT verdicts for review
if [ "$INCORRECT" -gt 0 ]; then
    echo "============================================================"
    echo "⚠️  INCORRECT VERDICTS (need review):"
    echo "============================================================"
    grep "INCORRECT" "$VERIFY_CSV"
fi

echo ""
echo "End time: $(date)"
echo "CSV: $CSV"
echo "Verification: $VERIFY_CSV"
