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
RESULTS_DIR="${RESULTS_DIR:-$SCRIPT_DIR/results}"
TIMEOUT_SEC=360

# CWE → mode mapping (single-pass flags, --add-invariants added on UNKNOWN)
#
# Only CWEs Map2Check actually supports carry a mode here. Everything else is
# declared in OUT_OF_SCOPE_CWES and skipped, because the old fallback for those
# was `--target-function --target-function-name main`, which is degenerate
# (finding B): TargetPass instruments call sites whose callee is the named
# function, a program never calls its own main, so nothing was instrumented and
# every vulnerable case was reported TRUE. A TRUE from an oracle that can only
# say "correct" earned 12 fake true negatives and 5 guaranteed false negatives
# on the v6 baseline.
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
)

# Unsupported CWEs. 628/674/770/835 are termination/resource properties (loop,
# recursion, unbounded workers) that Map2Check does not model; the rest are
# injection/crypto categories outside SV-COMP's scorable set for this tool.
# These are NOT run -- running them under the degenerate reachability mode only
# produced misleading TRUEs and burned a full budget per case for nothing.
OUT_OF_SCOPE_CWES=(22 78 89 134 253 327 362 522 628 674 770 798 835)

is_out_of_scope() {
  local cwe="$1"
  for o in "${OUT_OF_SCOPE_CWES[@]}"; do
    [ "$cwe" = "$o" ] && return 0
  done
  return 1
}

# Verdict classification is shared with the Juliet runner and covered by
# tests/integration/test_verdict_classifier.sh — see that file for why
# "Forcing timeout" is not a timeout signal.
# shellcheck source=../lib/verdict_classifier.sh
. "$SCRIPT_DIR/../lib/verdict_classifier.sh"
# shellcheck source=../lib/isolated_run.sh
. "$SCRIPT_DIR/../lib/isolated_run.sh"

INNER_TIMEOUT=300   # map2check's own budget, passed as --timeout below

mkdir -p "$RESULTS_DIR"
RAW_DIR="$RESULTS_DIR/raw"
mkdir -p "$RAW_DIR"

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

  # Unsupported CWEs are skipped outright, not run under a degenerate mode.
  # Recorded as N/A so the row exists for completeness and for the resume map,
  # but no budget is spent and no verdict is invented.
  if is_out_of_scope "$cwe"; then
    echo "$id_short,$cwe,$name,$vulnerable,N/A,,,,N/A,0.0" >> "$RESULTS_DIR/castle_results.csv"
    NA=$((NA+1))
    echo "  [$RUN/250] $name (CWE-$cwe): N/A (out of scope)"
    continue
  fi

  mode_flags="${CWE_MODE[$cwe]:-}"
  if [ -z "$mode_flags" ]; then
    echo "  ERROR: no mode mapping for CWE-$cwe ($name) -- fix CWE_MODE or OUT_OF_SCOPE_CWES" >&2
    echo "$id_short,$cwe,$name,$vulnerable,UNMAPPED,,,,ERROR,0.0" >> "$RESULTS_DIR/castle_results.csv"
    ERR=$((ERR+1))
    continue
  fi

  # run_isolated gives each invocation a private CWD and captures to a file.
  #
  # The private CWD matters here beyond the usual hygiene: map2check names its
  # scratch "<sha1-of-input-bitcode>.map2check/" relative to the CWD
  # (caller.cpp:63), i.e. by input CONTENT rather than by run. Pass 1 and the
  # --add-invariants pass below analyse the SAME .bc and so resolve to the SAME
  # directory name, and a directory orphaned by an aborted run is reused the
  # next time that input is analysed, carrying stale artefacts in. It also keeps
  # the repo working tree free of scratch, which this runner used to litter
  # because its CWD was the repo root.
  raw="$RAW_DIR/${name%.c}.txt"

  # --- Pass 1: direct ---
  start=$(date +%s%N)
  rc=0
  run_isolated "$raw" "$TIMEOUT_SEC" \
    "$MAP2CHECK" $mode_flags --timeout "$INNER_TIMEOUT" "$bc_file" || rc=$?
  end=$(date +%s%N)
  elapsed=$(python3 -c "print(round(($end - $start) / 1000000000, 1))")
  output=$(cat "$raw")

  used_invariants="no"
  verdict=$(classify_map2check_verdict "$output" "$rc" "$elapsed" "$INNER_TIMEOUT")

  # --- Pass 2: --add-invariants fallback ---
  # The flag is still requested unconditionally, on purpose: this pass exists to
  # retry UNKNOWN cases with abstract-interpretation invariants, and map2check is
  # supposed to be able to supply them. What is handled here is the documented
  # exit code for "the generator is not installed" (3), and only because pass 2
  # REPLACES pass 1's verdict -- without this, a run made while Clam is missing
  # would write ERROR over a perfectly good UNKNOWN and quietly corrupt the CSV
  # instead of failing visibly.
  #
  # Reading v5 data: through that baseline the flag was accepted and ignored on
  # the .bc path, so this pass was a byte-identical re-run of pass 1. It burned a
  # second full budget per UNKNOWN row and recorded used_invariants=yes for runs
  # that used none. See docs/reports/2026-08-16-crabllvm-review.md.
  if [ "$verdict" = "UNKNOWN" ] && [ "$mode_flags" != "--check-asserts" ]; then
    start=$(date +%s%N)
    rc=0
    run_isolated "$raw" "$TIMEOUT_SEC" \
      "$MAP2CHECK" $mode_flags --add-invariants --timeout "$INNER_TIMEOUT" "$bc_file" || rc=$?
    end=$(date +%s%N)

    if [ "$rc" -eq 3 ]; then
      # Capability absent. Keep pass 1's verdict, elapsed time and raw output;
      # say so in the CSV rather than pretending invariants were tried.
      used_invariants="unavailable"
      printf '%s' "$output" > "$raw"
    else
      used_invariants="yes"
      output=$(cat "$raw")
      elapsed=$(python3 -c "print(round(($end - $start) / 1000000000, 1))")
      verdict=$(classify_map2check_verdict "$output" "$rc" "$elapsed" "$INNER_TIMEOUT")
    fi
  fi

  # run_isolated already wrote "$raw"; pass 2, when it runs, overwrites it so
  # the file always matches the verdict finally recorded. Keeping it means a
  # future classifier bug is repaired by reclassifying rather than re-running.

  # The violation line is only reported in the "Violated property" block as
  # "file map2check_property line N". A bare `grep 'line \d+' | head -1` would
  # instead match LibFuzzer's "inline 8-bit counters" (-> "line 8").
  reported_line=$(echo "$output" | grep -oP 'map2check_property line \K\d+' | head -1)

  # --- Classify ---
  # Out-of-scope CWEs were skipped above (continue), so every case reaching
  # here is in scope and gets a verdict-driven classification.
  if [ "$verdict" = "TIMEOUT" ]; then
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
