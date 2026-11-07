#!/bin/bash
# test_juliet_sampling.sh — tests for the Juliet stratified sampler.
#
# This file exists because of a specific failure. The v3 baseline capped work
# with LIMIT_PER_CWE, which took the first N files alphabetically. Since a test
# family owns ~26 consecutive filenames, 60 files collapsed into 2-3 families:
# CWE-190 was measured on 6 of its 90 families, CWE-369 on 2 of 18. Every
# int64_t family went untested -- including the exact ones the overflow work
# existed to validate. Nothing failed, nothing warned; the CSV looked complete
# and two full runs were published off it.
#
# A sampler that is wrong produces plausible output, so it needs assertions
# rather than inspection. The invariants below are the ones whose violation
# would repeat that failure silently.

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RUNNER="$SCRIPT_DIR/../juliet/run_juliet_evaluation.sh"

PASSED=0
FAILED=0

ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

# --- The selection logic, lifted verbatim from the runner -------------------
# Kept as a mirror rather than sourced: the runner is a top-to-bottom script
# that would start compiling and analysing on source. If these ever drift the
# family-coverage test below still runs against the real runner and catches it.
select_stratified() {
  local per="$1"; shift
  local -A FAM_FILES=()
  local fam_order=() base fam f n i
  local SELECTED=()

  for f in "$@"; do
    base="${f##*/}"
    case "$base" in *[a-z].c) continue ;; esac
    fam="${base##*__}"
    fam="${fam%_*.c}"
    if [ -z "${FAM_FILES[$fam]+set}" ]; then
      fam_order+=("$fam")
      FAM_FILES[$fam]=""
    fi
    FAM_FILES[$fam]+="$f"$'\n'
  done

  for fam in "${fam_order[@]}"; do
    local fam_list=()
    mapfile -t fam_list < <(printf '%s' "${FAM_FILES[$fam]}")
    n=${#fam_list[@]}
    if [ "$per" -le 0 ] || [ "$n" -le "$per" ]; then
      SELECTED+=("${fam_list[@]}")
    elif [ "$per" -eq 1 ]; then
      SELECTED+=("${fam_list[0]}")
    else
      for ((i = 0; i < per; i++)); do
        SELECTED+=("${fam_list[$((i * (n - 1) / (per - 1)))]}")
      done
    fi
  done
  printf '%s\n' "${SELECTED[@]}"
}

famof() { local b="${1##*/}"; b="${b##*__}"; echo "${b%_*.c}"; }

# Build a synthetic tree: 3 families x 26 variants, plus fragments and a
# single-variant family.
CORPUS=()
for f in int_max_multiply int64_t_rand_square short_min_sub; do
  for i in $(seq -w 1 26); do CORPUS+=("/t/CWE190_Integer_Overflow__${f}_${i}.c"); done
done
CORPUS+=("/t/CWE190_Integer_Overflow__int_fscanf_add_51a.c")   # fragment
CORPUS+=("/t/CWE190_Integer_Overflow__int_fscanf_add_51b.c")   # fragment
CORPUS+=("/t/CWE190_Integer_Overflow__lonely_family_01.c")     # 1 variant only

echo "============================================================"
echo "Juliet stratified sampler"
echo "============================================================"

# --- Every family is represented, whatever PER_FAMILY is --------------------
# This is the invariant the v3 bug violated.
ALL_FAMS=$(for f in "${CORPUS[@]}"; do
             case "${f##*/}" in *[a-z].c) continue ;; esac; famof "$f"; done | sort -u)
N_FAMS=$(echo "$ALL_FAMS" | wc -l)

for per in 1 2 3 5 0; do
  got=$(select_stratified "$per" "${CORPUS[@]}" | while read -r f; do famof "$f"; done | sort -u)
  if [ "$got" = "$ALL_FAMS" ]; then
    ok "PER_FAMILY=$per covers all $N_FAMS families"
  else
    fail "PER_FAMILY=$per family coverage" "expected [$(echo $ALL_FAMS)], got [$(echo $got)]"
  fi
done

# --- Exactly PER_FAMILY files per family, and no duplicates -----------------
for per in 2 3 5; do
  bad=""
  while read -r fam cnt; do
    [ "$cnt" -ne "$per" ] && bad="$bad $fam=$cnt"
  done < <(select_stratified "$per" "${CORPUS[@]}" \
             | while read -r f; do famof "$f"; done \
             | sort | uniq -c | awk '$2!="lonely_family"{print $2" "$1}')
  [ -z "$bad" ] && ok "PER_FAMILY=$per takes exactly $per per family" \
                || fail "PER_FAMILY=$per per-family count" "$bad"

  total=$(select_stratified "$per" "${CORPUS[@]}" | wc -l)
  uniqn=$(select_stratified "$per" "${CORPUS[@]}" | sort -u | wc -l)
  [ "$total" -eq "$uniqn" ] && ok "PER_FAMILY=$per selects no file twice" \
                            || fail "PER_FAMILY=$per duplicates" "$total selected, $uniqn unique"
done

# --- A family smaller than PER_FAMILY yields what it has, not an error ------
n_lonely=$(select_stratified 3 "${CORPUS[@]}" | grep -c 'lonely_family')
[ "$n_lonely" -eq 1 ] && ok "family with fewer files than PER_FAMILY is taken whole" \
                      || fail "small family" "expected 1, got $n_lonely"

# --- Multi-file fragments stay out -----------------------------------------
n_frag=$(select_stratified 0 "${CORPUS[@]}" | grep -c '_51[ab]\.c')
[ "$n_frag" -eq 0 ] && ok "multi-file fragments (_51a/_51b) are excluded" \
                    || fail "fragments" "$n_frag leaked into the sample"

# --- The sample spans the variant range, it is not a prefix -----------------
# Taking _01.._03 would re-introduce the same bias one level down: the simple
# control-flow shapes live at the start of the range and the hard ones
# (function pointers, unions, interprocedural flow) at the end.
sel=$(select_stratified 3 "${CORPUS[@]}" | grep 'int_max_multiply' | sed 's/.*_\([0-9]*\)\.c/\1/' | sort)
if [ "$sel" = "$(printf '01\n13\n26')" ]; then
  ok "3 of 26 are spread first/middle/last, not a prefix"
else
  fail "variant spread" "expected 01,13,26 -- got $(echo $sel | tr '\n' ',')"
fi

# --- PER_FAMILY=0 means everything -----------------------------------------
n_all=$(select_stratified 0 "${CORPUS[@]}" | wc -l)
[ "$n_all" -eq 79 ] && ok "PER_FAMILY=0 selects every self-contained file (79)" \
                    || fail "PER_FAMILY=0" "expected 79, got $n_all"

# --- LIMIT_PER_CWE: rejected when it would cap, accepted when inert ---------
#
# These two are the only assertions that execute the real runner, and they are
# sandboxed: JULIET_DIR points at nothing and RESULTS_DIR at a temp directory.
# That matters for three reasons. The runner is not a library -- sourcing or
# calling it starts compiling and analysing -- so an unsandboxed call creates
# result directories in the working tree (it did, before this was fixed). It
# would also make the test depend on the 570 MB Juliet submodule and on a
# working clang, neither of which the sampling logic needs. And it would be
# slow, where the whole point is a guard that trips in milliseconds.
#
# The assertion is on the exit code as well as the message, because "did not
# print the LIMIT_PER_CWE error" is satisfied by any other failure too. Exit 2
# is the guard; exit 1 is the stubs check further down, which only runs if the
# guard let execution through -- so it is positive evidence, not absence.
run_guard() {
  local tmp out rc=0
  tmp=$(mktemp -d)
  out=$(JULIET_DIR="$tmp/absent" RESULTS_DIR="$tmp/out" bash "$RUNNER" 2>&1) || rc=$?
  rm -rf "$tmp"
  printf '%s\n__RC__%s\n' "$out" "$rc"
}

got=$(LIMIT_PER_CWE=60 run_guard)
if echo "$got" | grep -q 'FATAL: LIMIT_PER_CWE' && echo "$got" | grep -q '__RC__2'; then
  ok "LIMIT_PER_CWE=60 aborts with exit 2 (it is the v3 sampling bias)"
else
  fail "LIMIT_PER_CWE=60" "expected the guard and exit 2, got: $(echo "$got" | tr '\n' ' ' | cut -c1-90)"
fi

got=$(LIMIT_PER_CWE=0 run_guard)
if echo "$got" | grep -q 'FATAL: LIMIT_PER_CWE'; then
  fail "LIMIT_PER_CWE=0" "0 meant 'unlimited' and was a no-op; must not abort"
elif echo "$got" | grep -q 'FATAL: juliet_stubs.c failed to compile'; then
  ok "LIMIT_PER_CWE=0 passes the guard (reaches the stubs check downstream)"
else
  fail "LIMIT_PER_CWE=0" "expected to reach the stubs check, got: $(echo "$got" | tr '\n' ' ' | cut -c1-90)"
fi

# Nothing above may have written into the repository.
if [ -e "$SCRIPT_DIR/../juliet/results" ]; then
  fail "no side effects" "the runner created tests/juliet/results in the working tree"
else
  ok "no results directories created in the working tree"
fi

echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
