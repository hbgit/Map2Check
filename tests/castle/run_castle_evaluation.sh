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
TIMEOUT_SEC=360

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
  [369]="--check-overflow"
  # Memcleanup
  [401]="--memcleanup-property"
  # Assert
  [617]="--check-asserts"
  # Reachability (other)
  [628]="--target-function --target-function-name main"
  [674]="--target-function --target-function-name main"
  [770]="--target-function --target-function-name main"
  [835]="--target-function --target-function-name main"
  # Out of scope (still run for completeness, flagged N/A)
  [22]="--target-function --target-function-name main"
  [78]="--target-function --target-function-name main"
  [89]="--target-function --target-function-name main"
  [134]="--target-function --target-function-name main"
  [253]="--target-function --target-function-name main"
  [327]="--target-function --target-function-name main"
  [362]="--target-function --target-function-name main"
  [522]="--target-function --target-function-name main"
  [798]="--target-function --target-function-name main"
)

OUT_OF_SCOPE_CWES=(22 78 89 134 253 327 362 522 798)

is_out_of_scope() {
  local cwe="$1"
  for o in "${OUT_OF_SCOPE_CWES[@]}"; do
    [ "$cwe" = "$o" ] && return 0
  done
  return 1
}

# Classify map2check's raw output into a verdict.
# Note: a REAL timeout is signalled by map2check's "Forcing timeout" message
# (engine killed by the inner `timeout`). The coreutils `timeout` error
# "timeout: failed to run command ... No such file or directory" is an
# INFRASTRUCTURE failure (fuzzer/opt broken), not a timeout — classify as ERROR.
detect_verdict() {
  local output="$1"
  if echo "$output" | grep -q "FALSE-DEREF"; then
    echo "FALSE-DEREF"
  elif echo "$output" | grep -q "FALSE-FREE"; then
    echo "FALSE-FREE"
  elif echo "$output" | grep -q "FALSE-MEMTRACK"; then
    echo "FALSE-MEMTRACK"
  elif echo "$output" | grep -q "FALSE-OVERFLOW"; then
    echo "FALSE-OVERFLOW"
  elif echo "$output" | grep -q "FALSE-MEMCLEANUP"; then
    echo "FALSE-MEMCLEANUP"
  elif echo "$output" | grep -q "VERIFICATION FAILED"; then
    echo "FALSE"
  elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
    echo "TRUE"
  elif echo "$output" | grep -q "Forcing timeout"; then
    echo "TIMEOUT"
  elif echo "$output" | grep -qiE "failed to run command|No such file or directory|undefined reference|Unknown command line argument"; then
    echo "ERROR"
  else
    echo "UNKNOWN"
  fi
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
if [ ! -f "$RESULTS_DIR/castle_results.csv" ]; then
  echo "id,cwe,name,vulnerable,mode,verdict,invariants,expected,result,time_sec" > "$RESULTS_DIR/castle_results.csv"
fi

# Resume: skip ids already present in the CSV
declare -A DONE
while IFS=, read -r _cid _cwe _cname _vuln _mode _verdict _inv _exp _res _t; do
  [ -n "$_cid" ] && DONE["$_cid"]=1
done < <(tail -n +2 "$RESULTS_DIR/castle_results.csv" 2>/dev/null)

RUN=0
TP=0; TN=0; FP=0; FN=0; UNK=0; TO=0; NA=0; ERR=0

for c_file in "$CASTLE_DIR"/*.c; do
  name=$(basename "$c_file")
  id="${name%.c}"
  id_short=$(echo "$id" | sed 's/CASTLE-//')

  [ -n "${DONE[$id_short]:-}" ] && continue

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

  [ -n "$entry" ] || { echo "  SKIP: $name not found in JSON"; continue; }

  vulnerable=$(echo "$entry" | python3 -c "import sys,json; print(json.loads(sys.stdin.read())['vulnerable'])")
  cwe=$(echo "$entry" | python3 -c "import sys,json; print(json.loads(sys.stdin.read())['cwe'])")

  mode_flags="${CWE_MODE[$cwe]:---target-function --target-function-name main}"

  # --- Pass 1: direct ---
  start=$(date +%s%N)
  output=$(timeout "$TIMEOUT_SEC" "$MAP2CHECK" $mode_flags --timeout 300 "$bc_file" 2>&1) || true
  end=$(date +%s%N)
  elapsed=$(python3 -c "print(round(($end - $start) / 1000000000, 1))")

  used_invariants="no"
  verdict=$(detect_verdict "$output")

  # --- Pass 2: --add-invariants fallback ---
  if [ "$verdict" = "UNKNOWN" ] && [ "$mode_flags" != "--check-asserts" ]; then
    used_invariants="yes"
    start=$(date +%s%N)
    output=$(timeout "$TIMEOUT_SEC" "$MAP2CHECK" $mode_flags --add-invariants --timeout 300 "$bc_file" 2>&1) || true
    end=$(date +%s%N)
    elapsed=$(python3 -c "print(round(($end - $start) / 1000000000, 1))")

    verdict=$(detect_verdict "$output")
  fi

  # The violation line is only reported in the "Violated property" block as
  # "file map2check_property line N". A bare `grep 'line \d+' | head -1` would
  # instead match LibFuzzer's "inline 8-bit counters" (-> "line 8").
  reported_line=$(echo "$output" | grep -oP 'map2check_property line \K\d+' | head -1)

  # --- Classify ---
  if is_out_of_scope "$cwe"; then
    classification="N/A"
    NA=$((NA+1))
  elif [ "$verdict" = "TIMEOUT" ]; then
    classification="TIMEOUT"
    TO=$((TO+1))
  elif [ "$verdict" = "ERROR" ]; then
    classification="ERROR"
    ERR=$((ERR+1))
  elif [ "$verdict" = "UNKNOWN" ]; then
    classification="UNKNOWN"
    UNK=$((UNK+1))
  elif [ "$verdict" != "TRUE" ] && [ "$vulnerable" = "True" ]; then
    has_line_match="false"
    if [ -n "$reported_line" ]; then
      expected_lines=$(echo "$entry" | python3 -c "import sys,json; print(','.join(str(l) for l in json.loads(sys.stdin.read())['lines']))" 2>/dev/null || echo "")
      if [ -n "$expected_lines" ]; then
        for exp_line in $(echo "$expected_lines" | tr ',' ' '); do
          if [ "$reported_line" -eq "$exp_line" ] 2>/dev/null; then
            has_line_match="true"
            break
          fi
        done
      else
        has_line_match="true"
      fi
    else
      has_line_match="true"
    fi
    if [ "$has_line_match" = "true" ]; then
      classification="TP"
      TP=$((TP+1))
    else
      classification="FP"
      FP=$((FP+1))
    fi
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

cat > "$RESULTS_DIR/castle_summary.txt" << EOF
CASTLE C250 × Map2Check Evaluation Results
===========================================
Date: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
Map2Check binary: $MAP2CHECK

Totals:
  Total tests:           250
  In-scope results:      $TOTAL_SCOPE
  Out of scope (N/A):    $NA
  Timeouts:              $TO
  Errors (infra):        $ERR
  Unknown:               $UNK

Detection Metrics (in-scope):
  True Positives:        $TP
  False Positives:       $FP
  True Negatives:        $TN
  False Negatives:       $FN

EOF

  if [ $((TP + FP)) -gt 0 ]; then
    prec=$(python3 -c "print(f'{$TP/($TP+$FP)*100:.1f}%')")
    echo "  Precision:             $prec" >> "$RESULTS_DIR/castle_summary.txt"
  else
    echo "  Precision:             N/A (no positives)" >> "$RESULTS_DIR/castle_summary.txt"
  fi
  if [ $((TP + FN)) -gt 0 ]; then
    rec=$(python3 -c "print(f'{$TP/($TP+$FN)*100:.1f}%')")
    echo "  Recall:                $rec" >> "$RESULTS_DIR/castle_summary.txt"
  else
    echo "  Recall:                N/A (no condition positives)" >> "$RESULTS_DIR/castle_summary.txt"
  fi

  cat >> "$RESULTS_DIR/castle_summary.txt" << EOF

Results written to:
  CSV:  $RESULTS_DIR/castle_results.csv
EOF

cat "$RESULTS_DIR/castle_summary.txt"

echo ""
echo "Done. Results in $RESULTS_DIR/"
