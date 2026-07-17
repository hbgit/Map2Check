#!/bin/bash
# run_castle_evaluation.sh — Run all 250 CASTLE C250 benchmarks through Map2Check
# Two-pass strategy: direct, then --add-invariants fallback on UNKNOWN
set -e

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CASTLE_DIR="$SCRIPT_DIR/CASTLE-Benchmark/datasets/CASTLE-C250"
JSON_FILE="$SCRIPT_DIR/CASTLE-Benchmark/datasets/CASTLE-C250.min.json"
RESULTS_DIR="$SCRIPT_DIR/results"
TIMEOUT_SEC=120

# CWE → mode mapping (single-pass flags, --add-invariants added on UNKNOWN)
declare -A CWE_MODE=(
  # Memory safety (memtrack)
  [125]="--memtrack"
  [415]="--memtrack"
  [416]="--memtrack"
  [476]="--memtrack"
  [761]="--memtrack"
  [787]="--memtrack"
  [822]="--memtrack"
  [843]="--memtrack"
  # Overflow
  [190]="--check-overflow"
  # Memcleanup
  [401]="--memcleanup-property"
  # Assert
  [134]="--check-asserts"
  [617]="--check-asserts"
  # Reachability (other)
  [253]="--target-function --target-function-name main"
  [362]="--target-function --target-function-name main"
  [369]="--target-function --target-function-name main"
  [628]="--target-function --target-function-name main"
  [674]="--target-function --target-function-name main"
  [770]="--target-function --target-function-name main"
  [835]="--target-function --target-function-name main"
  # Out of scope (still run for completeness, flagged N/A)
  [22]="--target-function --target-function-name main"
  [78]="--target-function --target-function-name main"
  [89]="--target-function --target-function-name main"
  [327]="--target-function --target-function-name main"
  [522]="--target-function --target-function-name main"
  [798]="--target-function --target-function-name main"
)

OUT_OF_SCOPE_CWES=(22 78 89 327 522 798)

is_out_of_scope() {
  local cwe="$1"
  for o in "${OUT_OF_SCOPE_CWES[@]}"; do
    [ "$cwe" = "$o" ] && return 0
  done
  return 1
}

mkdir -p "$RESULTS_DIR"

echo "CASTLE C250 × Map2Check Evaluation"
echo "=================================="

# --- Phase 1: Compile all ---
echo ""
echo "Phase 1: Compiling C → LLVM bitcode..."
mkdir -p "$RESULTS_DIR/bc"
COMPILED=0
COMPILE_FAILED=0

for c_file in "$CASTLE_DIR"/*.c; do
  name=$(basename "$c_file")
  bc_file="$RESULTS_DIR/bc/${name%.c}.bc"
  if "$CLANG" -c -emit-llvm -g -O0 -Wno-everything "$c_file" -o "$bc_file" 2>/dev/null; then
    COMPILED=$((COMPILED+1))
  else
    COMPILE_FAILED=$((COMPILE_FAILED+1))
    echo "  FAIL: $name"
  fi
done

echo "  Compiled: $COMPILED, Failed: $COMPILE_FAILED"

# --- Phase 2: Verify + Score ---
echo ""
echo "Phase 2: Running Map2Check on each benchmark..."
echo "id,cwe,name,vulnerable,mode,verdict,invariants,expected,result,time_sec" > "$RESULTS_DIR/castle_results.csv"

RUN=0
TP=0; TN=0; FP=0; FN=0; UNK=0; TO=0; NA=0

for c_file in "$CASTLE_DIR"/*.c; do
  name=$(basename "$c_file")
  id="${name%.c}"
  id_short=$(echo "$id" | sed 's/CASTLE-//')

  bc_file="$RESULTS_DIR/bc/${id}.bc"
  [ -f "$bc_file" ] || continue

  RUN=$((RUN+1))

  # Extract ground truth from JSON
  entry=$(python3 -c "
import json
with open('$JSON_FILE') as f:
    data = json.load(f)
for t in data['tests']:
    if t['name'] == '$name':
        print(json.dumps({'vulnerable': t['vulnerable'], 'cwe': t['cwe'], 'lines': t.get('lines',[])}))
        break
" 2>/dev/null)

  vulnerable=$(echo "$entry" | python3 -c "import sys,json; print(json.loads(sys.stdin.read())['vulnerable'])")
  cwe=$(echo "$entry" | python3 -c "import sys,json; print(json.loads(sys.stdin.read())['cwe'])")

  mode_flags="${CWE_MODE[$cwe]:---target-function --target-function-name main}"

  # --- Pass 1: direct ---
  start=$(date +%s%N)
  output=$(timeout "$TIMEOUT_SEC" "$MAP2CHECK" $mode_flags --timeout 60 "$bc_file" 2>&1) || true
  end=$(date +%s%N)
  elapsed=$(echo "scale=1; ($end - $start) / 1000000000" | bc)

  used_invariants="no"
  if echo "$output" | grep -qi "TIMEOUT\|timed out"; then
    verdict="TIMEOUT"
  elif echo "$output" | grep -q "FALSE-DEREF"; then
    verdict="FALSE-DEREF"
  elif echo "$output" | grep -q "FALSE-FREE"; then
    verdict="FALSE-FREE"
  elif echo "$output" | grep -q "FALSE-MEMTRACK"; then
    verdict="FALSE-MEMTRACK"
  elif echo "$output" | grep -q "FALSE-OVERFLOW"; then
    verdict="FALSE-OVERFLOW"
  elif echo "$output" | grep -q "FALSE-MEMCLEANUP"; then
    verdict="FALSE-MEMCLEANUP"
  elif echo "$output" | grep -q "VERIFICATION FAILED"; then
    verdict="FALSE"
  elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
    verdict="TRUE"
  else
    verdict="UNKNOWN"
  fi

  # --- Pass 2: --add-invariants fallback ---
  if [ "$verdict" = "UNKNOWN" ] && [ "$mode_flags" != "--check-asserts" ]; then
    used_invariants="yes"
    start=$(date +%s%N)
    output=$(timeout "$TIMEOUT_SEC" "$MAP2CHECK" $mode_flags --add-invariants --timeout 60 "$bc_file" 2>&1) || true
    end=$(date +%s%N)
    elapsed=$(echo "scale=1; ($end - $start) / 1000000000" | bc)

    if echo "$output" | grep -qi "TIMEOUT\|timed out"; then
      verdict="TIMEOUT"
    elif echo "$output" | grep -q "FALSE-DEREF"; then
      verdict="FALSE-DEREF"
    elif echo "$output" | grep -q "FALSE-FREE"; then
      verdict="FALSE-FREE"
    elif echo "$output" | grep -q "FALSE-MEMTRACK"; then
      verdict="FALSE-MEMTRACK"
    elif echo "$output" | grep -q "FALSE-OVERFLOW"; then
      verdict="FALSE-OVERFLOW"
    elif echo "$output" | grep -q "FALSE-MEMCLEANUP"; then
      verdict="FALSE-MEMCLEANUP"
    elif echo "$output" | grep -q "VERIFICATION FAILED"; then
      verdict="FALSE"
    elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
      verdict="TRUE"
    else
      verdict="UNKNOWN"
    fi
  fi

  # --- Classify ---
  if is_out_of_scope "$cwe"; then
    classification="N/A"
    NA=$((NA+1))
  elif [ "$verdict" = "TIMEOUT" ]; then
    classification="TIMEOUT"
    TO=$((TO+1))
  elif [ "$verdict" = "UNKNOWN" ]; then
    classification="UNKNOWN"
    UNK=$((UNK+1))
  elif [ "$verdict" != "TRUE" ] && [ "$vulnerable" = "True" ]; then
    classification="TP"
    TP=$((TP+1))
  elif [ "$verdict" != "TRUE" ] && [ "$vulnerable" = "False" ]; then
    classification="FP"
    FP=$((FP+1))
  elif [ "$verdict" = "TRUE" ] && [ "$vulnerable" = "False" ]; then
    classification="TN"
    TN=$((TN+1))
  elif [ "$verdict" = "TRUE" ] && [ "$vulnerable" = "True" ]; then
    classification="FN"
    FN=$((FN+1))
  else
    classification="UNCLASSIFIED"
    UNK=$((UNK+1))
  fi

  echo "$id_short,$cwe,$name,$vulnerable,$mode_flags,$verdict,$used_invariants,$vulnerable,$classification,$elapsed" >> "$RESULTS_DIR/castle_results.csv"
  echo "  [$RUN/250] $name (CWE-$cwe): $verdict → $classification (${elapsed}s)"
done

# --- Phase 3: Summary ---
echo ""
echo "Phase 3: Generating summary..."
TOTAL_SCOPE=$((TP+TN+FP+FN))
DETECTABLE=$((TOTAL_SCOPE - NA))

cat > "$RESULTS_DIR/castle_summary.txt" << EOF
CASTLE C250 × Map2Check Evaluation Results
===========================================
Date: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
Map2Check binary: $MAP2CHECK

Totals:
  Total tests:           250
  Detected (in-scope):   $DETECTABLE
  Out of scope (N/A):    $NA
  Timeouts:              $TO
  Unknown:               $UNK

Detection Metrics (in-scope):
  True Positives:        $TP
  False Positives:       $FP
  True Negatives:        $TN
  False Negatives:       $FN

  Precision:             $(python3 -c "print(f'{$TP/($TP+$FP)*100:.1f}%')" 2>/dev/null || echo "N/A")
  Recall:                $(python3 -c "print(f'{$TP/($TP+$FN)*100:.1f}%')" 2>/dev/null || echo "N/A")

Results written to:
  CSV:  $RESULTS_DIR/castle_results.csv
EOF

cat "$RESULTS_DIR/castle_summary.txt"

echo ""
echo "Done. Results in $RESULTS_DIR/"
