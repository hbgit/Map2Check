#!/bin/bash
# verify_verdicts.sh — Verify Map2Check results against sv-benchmarks expected_verdict
# Usage: docker run --rm -v $(pwd):/workspace map2check-dev bash /workspace/test-comp2026/simulation/verify_verdicts.sh
set -e

SV_BENCH=/workspace/test-comp2026/simulation/sv-benchmarks/c
RESULTS_DIR=/workspace/test-comp2026/simulation/resultados_de_testes/heap_coverage_error_call
CSV="$RESULTS_DIR/heap_results_v8.csv"
VERIFY_CSV="$RESULTS_DIR/verdict_verification.csv"

echo "set_name,subdir,file,map2check_result,expected_verdict,match" > "$VERIFY_CSV"

CORRECT=0; INCORRECT=0; INCONCLUSIVE=0; TOTAL_VERIFY=0

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
        INCONCLUSIVE=$((INCONCLUSIVE + 1))
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
        INCONCLUSIVE=$((INCONCLUSIVE + 1))
        continue
    fi

    # Determine match
    match="INCONCLUSIVE"
    case "$result" in
        FALSE|FALSE_FREE|FALSE_DEREF|FALSE_MEMTRACK|FALSE_MEMCLEANUP|FALSE_OVERFLOW)
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
            match="INCONCLUSIVE"
            INCONCLUSIVE=$((INCONCLUSIVE + 1))
            ;;
        *)
            match="INCONCLUSIVE"
            INCONCLUSIVE=$((INCONCLUSIVE + 1))
            ;;
    esac

    echo "$set_name,$subdir,$file_name,$result,$expected,$match" >> "$VERIFY_CSV"
done < "$CSV"

echo ""
echo "============================================================"
echo "VERDICT VERIFICATION RESULTS"
echo "============================================================"
echo "Total tasks:        $TOTAL_VERIFY"
echo "CORRECT:            $CORRECT"
echo "INCORRECT:          $INCORRECT"
echo "INCONCLUSIVE/N/A:   $INCONCLUSIVE"
echo ""
DECISIVE=$((CORRECT + INCORRECT))
if [ "$DECISIVE" -gt 0 ]; then
    PCT=$((CORRECT * 100 / DECISIVE))
    echo "Accuracy (decisive): $CORRECT / $DECISIVE ($PCT%)"
else
    echo "Accuracy: N/A (no decisive results)"
fi
echo ""

# Print INCORRECT verdicts for review
if [ "$INCORRECT" -gt 0 ]; then
    echo "============================================================"
    echo "⚠️  INCORRECT VERDICTS (need review):"
    echo "============================================================"
    grep "INCORRECT" "$VERIFY_CSV"
fi

# Summary by result type
echo ""
echo "============================================================"
echo "BREAKDOWN BY RESULT TYPE"
echo "============================================================"
echo "--- CORRECT ---"
grep "CORRECT" "$VERIFY_CSV" | grep -v "INCORRECT" | cut -d, -f4 | sort | uniq -c | sort -rn
echo ""
echo "--- INCORRECT ---"
grep "INCORRECT" "$VERIFY_CSV" | cut -d, -f4 | sort | uniq -c | sort -rn 2>/dev/null || echo "  (none)"
echo ""
echo "Verdict CSV saved to: $VERIFY_CSV"
