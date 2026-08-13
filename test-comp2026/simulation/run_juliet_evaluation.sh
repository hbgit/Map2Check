#!/bin/bash
# run_juliet_evaluation.sh — Run Juliet scope-C through Map2Check (sequential, resumable)
#
# Scope C: CWEs 121, 122, 415, 416, 476, 761 (--memtrack), 190 (--check-overflow),
#          401 (--memcleanup-property).
#
# Juliet here is the RAW NIST tree under test-comp2026/juliet/testcases/ (not the
# sv-benchmarks tasks). Each .c file is compiled TWICE to separate the variants:
#   bad  = -DINCLUDEMAIN -DOMITGOOD  -> vulnerable (expect FALSE)
#   good = -DINCLUDEMAIN -DOMITBAD   -> safe       (expect TRUE)
#
# Two harness adaptations are required (see docs/superpowers/specs/*-castle-juliet-baseline.md):
#   1. The KLEE/LibFuzzer wrapper forwards argc/argv to __map2check_main__ (fixed in
#      NonDetGeneratorKlee.c / NonDetGeneratorLibFuzzy.c — main(argc,argv) is now supported).
#   2. testcasesupport/io.c I/O (printf/time/rand) is replaced by juliet_stubs.c so
#      KLEE can exhaust safe programs (otherwise "partially completed paths" -> UNKNOWN).
#
# Resumable: rows already present in the CSV are skipped (key = <name>|<bad|good>).
# Each run executes in an isolated CWD (map2check writes CWD-relative artifacts).
#
# Env: MAP2CHECK_PATH (dir with map2check binary), JULIET_DIR (default ../juliet),
#      RESULTS_DIR, OUTER_TIMEOUT, INNER_TIMEOUT, LIMIT (max files, for smoke tests).

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_castle_ci}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
LLVM_LINK="${LLVM_LINK:-/usr/bin/llvm-link-16}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
JULIET_DIR="${JULIET_DIR:-$SCRIPT_DIR/../juliet}"
SUPPORT_DIR="$JULIET_DIR/testcasesupport"
TESTCASES_DIR="$JULIET_DIR/testcases"
STUBS="$SCRIPT_DIR/juliet_stubs.c"
RESULTS_DIR="${RESULTS_DIR:-$SCRIPT_DIR/resultados_de_testes/juliet_scope_c}"
OUTER_TIMEOUT="${OUTER_TIMEOUT:-300}"
INNER_TIMEOUT="${INNER_TIMEOUT:-60}"
LIMIT="${LIMIT:-0}"                       # 0 = unlimited, counted across all CWEs
LIMIT_PER_CWE="${LIMIT_PER_CWE:-0}"       # 0 = unlimited, counted within each CWE

declare -A CWE_MODE=(
  [121]="--memtrack"
  [122]="--memtrack"
  [415]="--memtrack"
  [416]="--memtrack"
  [476]="--memtrack"
  [761]="--memtrack"
  [190]="--check-overflow"
  [191]="--check-overflow"
  [369]="--check-overflow"
  [401]="--memcleanup-property"
)
# CWE-191 (underflow) is the same no-overflow property as 190, and CWE-369
# (divide by zero) is reported as FALSE-DIVBYZERO; both are covered by
# --check-overflow. Override with e.g. JULIET_CWES="190 191 369" to run a
# targeted slice.
DEFAULT_SCOPE_CWES=(121 122 415 416 476 761 190 191 369 401)
read -r -a SCOPE_CWES <<< "${JULIET_CWES:-${DEFAULT_SCOPE_CWES[*]}}"

CSV="$RESULTS_DIR/juliet_scope_c_results.csv"
LOG="$RESULTS_DIR/run.log"
BC_DIR="$RESULTS_DIR/bc"
mkdir -p "$RESULTS_DIR" "$BC_DIR"

# --- resume map ---
declare -A DONE
if [ -f "$CSV" ]; then
  while IFS=, read -r _cwe _name _variant _vuln _mode _verdict _elapsed _cls; do
    DONE["$_name|$_variant"]=1
  done < <(tail -n +2 "$CSV" 2>/dev/null)
fi
[ -f "$CSV" ] || echo "cwe,name,variant,vulnerable,mode,verdict,elapsed_s,classification" > "$CSV"

# --- precompile stubs ---
STUBS_BC="$BC_DIR/juliet_stubs.bc"
"$CLANG" -c -emit-llvm -g -O0 -I "$SUPPORT_DIR" "$STUBS" -o "$STUBS_BC" 2>/dev/null \
  || { echo "FATAL: juliet_stubs.c failed to compile"; exit 1; }

log() { echo "[$(date -u +%H:%M:%S)] $*" | tee -a "$LOG"; }

classify_verdict() {
  local output="$1"
  if echo "$output" | grep -qE "FALSE-DEREF|FALSE-FREE|FALSE-MEMTRACK|FALSE-OVERFLOW|FALSE-DIVBYZERO|FALSE-MEMCLEANUP|VERIFICATION FAILED"; then
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

PROCESSED=0
SKIP=0
CF=0

for cwe in "${SCOPE_CWES[@]}"; do
  mode="${CWE_MODE[$cwe]:-}"
  [ -n "$mode" ] || { log "CWE-$cwe: no mode mapping, skipping"; continue; }
  cwe_dir=$(ls -d "$TESTCASES_DIR"/CWE${cwe}_* 2>/dev/null | head -1)
  [ -n "$cwe_dir" ] || { log "CWE-$cwe: no testcase dir, skipping"; continue; }
  PROCESSED_CWE=0

  while IFS= read -r -d '' f; do
    base=$(basename "$f")
    case "$base" in
      *[a-z].c) continue ;;   # multi-file fragments (_22a/_51b/...), not self-contained
    esac
    rel="${f#$TESTCASES_DIR/}"

    for variant in bad:OMITGOOD:True good:OMITBAD:False; do
      IFS=: read -r vtag def vuln <<< "$variant"
      key="$rel|$vtag"
      [ -n "${DONE[$key]:-}" ] && { SKIP=$((SKIP+1)); continue; }

      bc="$BC_DIR/${rel//\//__}.$vtag.bc"
      "$CLANG" -c -emit-llvm -g -O0 -DINCLUDEMAIN -D"$def" \
          -I "$SUPPORT_DIR" "$f" -o "$bc" 2>/dev/null || {
        echo "$cwe,$rel,$vtag,$vuln,$mode,COMPILE-FAIL,0,COMPILE-FAIL" >> "$CSV"
        CF=$((CF+1)); continue
      }

      combined="$BC_DIR/${rel//\//__}.$vtag.combined.bc"
      "$LLVM_LINK" "$bc" "$STUBS_BC" -o "$combined" 2>/dev/null || {
        echo "$cwe,$rel,$vtag,$vuln,$mode,LINK-FAIL,0,LINK-FAIL" >> "$CSV"
        CF=$((CF+1)); continue
      }

      work=$(mktemp -d)
      (
        cd "$work"
        start=$(date +%s%N)
        output=$(timeout "$OUTER_TIMEOUT" "$MAP2CHECK" $mode --timeout "$INNER_TIMEOUT" "$combined" 2>&1) || true
        end=$(date +%s%N)
        elapsed=$(python3 -c "print(round(($end - $start) / 1000000000, 1))")
        verdict=$(classify_verdict "$output")
        if [ "$verdict" = "FALSE" ]; then
          [ "$vuln" = "True" ] && cls="TP" || cls="FP"
        elif [ "$verdict" = "TRUE" ]; then
          [ "$vuln" = "True" ] && cls="FN" || cls="TN"
        elif [ "$verdict" = "TIMEOUT" ]; then
          cls="TIMEOUT"
        elif [ "$verdict" = "ERROR" ]; then
          cls="ERROR"
        else
          cls="UNKNOWN"
        fi
        echo "$cwe,$rel,$vtag,$vuln,$mode,$verdict,$elapsed,$cls" >> "$CSV"
        log "CWE-$cwe $rel/$vtag => $verdict [$cls] ${elapsed}s"
      )
      rm -rf "$work"
    done

    PROCESSED=$((PROCESSED+1))
    PROCESSED_CWE=$((PROCESSED_CWE+1))
    if [ "$LIMIT_PER_CWE" -gt 0 ] && [ "$PROCESSED_CWE" -ge "$LIMIT_PER_CWE" ]; then
      log "LIMIT_PER_CWE reached ($LIMIT_PER_CWE files) for CWE-$cwe; next CWE."
      break
    fi
    if [ "$LIMIT" -gt 0 ] && [ "$PROCESSED" -ge "$LIMIT" ]; then
      log "LIMIT reached ($LIMIT files); stopping."
      break 2
    fi
    # Sorted so a truncated run is a reproducible prefix rather than whatever
    # order the filesystem happened to hand back.
  done < <(find "$cwe_dir" -type f -name "*.c" -print0 | sort -z)
done

log "DONE: files_processed=$PROCESSED skipped=$SKIP compile_fail=$CF (authoritative tallies: analyze $CSV)"
log "Results: $CSV"
