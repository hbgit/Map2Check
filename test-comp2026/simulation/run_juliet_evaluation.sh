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
#      RESULTS_DIR, OUTER_TIMEOUT, INNER_TIMEOUT, LIMIT (max files, for smoke tests),
#      PER_FAMILY (files sampled per test family — see "SAMPLING" below).
#
# ---------------------------------------------------------------------------
# SAMPLING: stratified by test family, NOT by alphabetical prefix
# ---------------------------------------------------------------------------
#
# Juliet names every testcase CWE<n>_<Title>__<family>_<NN>.c, where:
#   <family> encodes WHAT is being tested — the data type, the input source and
#            the operation, e.g. `int64_t_max_multiply`, `short_min_sub`,
#            `int_zero_divide`. This is the axis the verifier's behaviour
#            actually varies along.
#   <NN>     (01..26+) is the "flow variant" — the same bug wrapped in a
#            different control-flow shape (if(1), static const guard, function
#            pointer, union, goto...). It varies how hard the bug is to REACH,
#            not what the bug IS.
#
# The v3 baseline capped work with LIMIT_PER_CWE, which took the first N files
# in alphabetical order. Because a family owns ~26 consecutive filenames, 60
# files collapsed into 2-3 families. Measured coverage was:
#
#     CWE-190:  6 of 90 families   (6.7%)
#     CWE-191:  5 of 69 families   (7.2%)
#     CWE-369:  2 of 18 families   (11.1%)
#
# The consequences were not subtle. Every `int64_t_*` family went untested — so
# the width-aware i64 overflow work the baseline existed to validate was never
# exercised once. In CWE-369 only `float_*` families were reached, so every
# genuine integer divide-by-zero (`int_zero_divide`, `int_zero_modulo`, ...)
# was missed while the sample filled with float division, which yields IEEE
# infinity and is not undefined behaviour at all. The resulting "0% recall on
# CWE-369" measured the sampler, not the tool.
#
# So: sample per FAMILY, and cover every family.
#
# How many files per family? The v3 data answers this empirically. Grouping its
# rows by (family, bad|good) gave 26 groups, and ALL 26 were perfectly
# homogeneous — every flow variant in a family produced the identical
# classification, with `short_min_multiply` alone covering 25 of its 26
# variants and never once disagreeing with itself. Within-family variance was
# zero. Spending 26 runs to re-measure one answer, while 84 other families go
# untouched, is the worst possible allocation of a fixed time budget.
#
# PER_FAMILY=3 is therefore the default: enough to catch a family that is NOT
# homogeneous (which the v3 slice cannot rule out for the ~85% of families it
# never saw), cheap enough to cover all 177 families across the three CWEs.
#
# The three files are spread ACROSS the variant range (first / middle / last)
# rather than taken from the front. Variants _01.._09 are the simple
# control-flow shapes; the harder ones (function pointers, unions, inter-
# procedural flow) live in the _1x/_2x range. Taking a prefix here would
# re-introduce, at the variant level, the exact bias being removed at the
# family level.
#
# Set PER_FAMILY=0 to disable sampling and run every file in the CWE.

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
PER_FAMILY="${PER_FAMILY:-3}"             # files sampled per family; 0 = all
# LIMIT_PER_CWE is deliberately gone: it was the alphabetical-prefix cap that
# produced the v3 sampling failure documented in the header. A per-CWE ceiling
# cannot be expressed without reintroducing that bias, so the knob is
# PER_FAMILY instead, which scales coverage while keeping it uniform.
# Only a value that would actually have capped anything is an error: 0 was the
# documented "unlimited" default, so an old caller passing LIMIT_PER_CWE=0 was
# asking for exactly what it gets now and must not be failed.
if [ -n "${LIMIT_PER_CWE:-}" ] && [ "${LIMIT_PER_CWE:-0}" -ne 0 ] 2>/dev/null; then
  echo "FATAL: LIMIT_PER_CWE is no longer supported (it caused the v3 sampling" >&2
  echo "       bias: 6/90 families on CWE-190). Use PER_FAMILY instead." >&2
  exit 2
fi

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
RAW_DIR="$RESULTS_DIR/raw"
mkdir -p "$RESULTS_DIR" "$BC_DIR" "$RAW_DIR"

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

# Verdict classification is shared with the CASTLE runner and covered by
# tests/integration/test_verdict_classifier.sh — see that file for why
# "Forcing timeout" is not a timeout signal. Juliet only needs the coarse
# FALSE/TRUE distinction, so the fine-grained FALSE-* verdicts collapse here.
# shellcheck source=../../tests/lib/verdict_classifier.sh
. "$SCRIPT_DIR/../../tests/lib/verdict_classifier.sh"
# shellcheck source=../../tests/lib/isolated_run.sh
. "$SCRIPT_DIR/../../tests/lib/isolated_run.sh"

classify_verdict() {
  local verdict
  verdict=$(classify_map2check_verdict "$1" "${2:-0}" "${3:-0}" "${4:-0}")
  case "$verdict" in
    FALSE-*) echo "FALSE" ;;
    *)       echo "$verdict" ;;
  esac
}

PROCESSED=0
SKIP=0
CF=0

for cwe in "${SCOPE_CWES[@]}"; do
  mode="${CWE_MODE[$cwe]:-}"
  [ -n "$mode" ] || { log "CWE-$cwe: no mode mapping, skipping"; continue; }
  cwe_dir=$(ls -d "$TESTCASES_DIR"/CWE${cwe}_* 2>/dev/null | head -1)
  [ -n "$cwe_dir" ] || { log "CWE-$cwe: no testcase dir, skipping"; continue; }

  # --- Stratified selection (see SAMPLING in the header) --------------------
  # Bucket every self-contained testcase by family, preserving alphabetical
  # order inside each bucket so the choice is deterministic and reproducible.
  unset FAM_FILES; declare -A FAM_FILES
  fam_order=()
  while IFS= read -r -d '' f; do
    base="${f##*/}"
    case "$base" in
      *[a-z].c) continue ;;   # multi-file fragments (_22a/_51b/...), not self-contained
    esac
    fam="${base##*__}"        # drop "CWE190_Integer_Overflow__"
    fam="${fam%_*.c}"         # drop the "_01.c" flow-variant suffix
    if [ -z "${FAM_FILES[$fam]+set}" ]; then
      fam_order+=("$fam")
      FAM_FILES[$fam]=""
    fi
    FAM_FILES[$fam]+="$f"$'\n'
  done < <(find "$cwe_dir" -type f -name "*.c" -print0 | sort -z)

  # Pick PER_FAMILY files per family, spread evenly across the variant range so
  # the sample spans simple and hard control-flow shapes alike.
  SELECTED=()
  for fam in "${fam_order[@]}"; do
    mapfile -t fam_list < <(printf '%s' "${FAM_FILES[$fam]}")
    n=${#fam_list[@]}
    if [ "$PER_FAMILY" -le 0 ] || [ "$n" -le "$PER_FAMILY" ]; then
      SELECTED+=("${fam_list[@]}")
    elif [ "$PER_FAMILY" -eq 1 ]; then
      SELECTED+=("${fam_list[0]}")
    else
      for ((i = 0; i < PER_FAMILY; i++)); do
        SELECTED+=("${fam_list[$((i * (n - 1) / (PER_FAMILY - 1)))]}")
      done
    fi
  done
  log "CWE-$cwe: ${#fam_order[@]} families, sampling ${#SELECTED[@]} files (PER_FAMILY=$PER_FAMILY)"

  for f in "${SELECTED[@]}"; do
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

      (
        # The raw output is written first and read back: run_isolated captures
        # to a file precisely so no pipe can be held open by an orphaned
        # backend (see tests/lib/isolated_run.sh). Keeping it also means a
        # future classifier bug is repaired by reclassifying, not by re-running.
        raw="$RAW_DIR/${rel//\//__}.$vtag.txt"
        start=$(date +%s%N)
        rc=0
        run_isolated "$raw" "$OUTER_TIMEOUT" \
          "$MAP2CHECK" $mode --timeout "$INNER_TIMEOUT" "$combined" || rc=$?
        end=$(date +%s%N)
        elapsed=$(python3 -c "print(round(($end - $start) / 1000000000, 1))")
        output=$(cat "$raw")
        verdict=$(classify_verdict "$output" "$rc" "$elapsed" "$INNER_TIMEOUT")
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
    done

    PROCESSED=$((PROCESSED+1))
    # LIMIT is a smoke-test escape hatch only. Unlike the removed LIMIT_PER_CWE
    # it is never used for a real baseline, so the prefix bias it introduces is
    # acceptable — but it is logged loudly so a truncated run can never be
    # mistaken for a complete one.
    if [ "$LIMIT" -gt 0 ] && [ "$PROCESSED" -ge "$LIMIT" ]; then
      log "LIMIT reached ($LIMIT files): run is TRUNCATED and NOT a valid baseline."
      break 2
    fi
  done
done

log "DONE: files_processed=$PROCESSED skipped=$SKIP compile_fail=$CF (authoritative tallies: analyze $CSV)"
log "Results: $CSV"
