#!/bin/bash
# run_memsafety_evaluation.sh -- Map2Check over a stratified SV-COMP MemSafety,
# MemCleanup or NoOverflows corpus, scoring each verdict against the task's expected
# verdict AND subproperty.
#
# Built for tacasv2b (slicing for the memory properties): the same corpus is
# run twice, with and without EXTRA_FLAGS=--slice, on the same build. What the
# comparison must show first is that slicing introduces no wrong answer, so
# the classes keep the two kinds of error apart:
#
#   correct-true   expected true, TRUE
#   correct-false  expected false, FALSE of the task's subproperty
#   wrong-true     expected false, TRUE -- the dangerous one
#   wrong-false    expected true and FALSE, or FALSE of the wrong kind
#   unknown        UNKNOWN or TIMEOUT
#   error          the tool failed
#
# Resumable: the CSV is the state, a task already in it is skipped.
#
# Environment:
#   MANIFEST      from build_corpus.py --property memsafety|memcleanup (required)
#   PROPERTY      memsafety | memcleanup | overflow (required)
#   RESULTS_DIR   where the CSV and raw logs go (required)
#   MAP2CHECK_PATH  install to run (required)
#   SHARD/SHARDS  process every SHARDS-th task starting at SHARD (default 0/1)
#   BUDGET        seconds given to map2check per task (default 120)
#   DEADLINE_S    stop starting new tasks after this many seconds (default 18000)
#   EXTRA_FLAGS   passed verbatim to map2check (e.g. "--slice")
set -u
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BENCH="$SCRIPT_DIR/../testcomp/bench/sv-benchmarks/c"
. "$SCRIPT_DIR/../lib/verdict_classifier.sh"
. "$SCRIPT_DIR/../lib/memsafety_classifier.sh"

MANIFEST="${MANIFEST:?set MANIFEST}"
PROPERTY="${PROPERTY:?set PROPERTY to memsafety or memcleanup}"
RESULTS_DIR="${RESULTS_DIR:?set RESULTS_DIR}"
MAP2CHECK_DIR="${MAP2CHECK_PATH:?set MAP2CHECK_PATH}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
SHARD="${SHARD:-0}"
SHARDS="${SHARDS:-1}"
BUDGET="${BUDGET:-120}"
DEADLINE_S="${DEADLINE_S:-18000}"
EXTRA_FLAGS="${EXTRA_FLAGS:-}"

case "$PROPERTY" in
  memsafety)  MODE_FLAGS="--memtrack" ;;
  memcleanup) MODE_FLAGS="--memcleanup-property" ;;
  overflow)   MODE_FLAGS="--check-overflow" ;;
  *) echo "unknown PROPERTY: $PROPERTY" >&2; exit 2 ;;
esac

mkdir -p "$RESULTS_DIR/raw"
CSV="$RESULTS_DIR/results.csv"
if [ ! -f "$CSV" ]; then
  echo "category,program,data_model,expected,subproperty,verdict,class,elapsed_s,slice" > "$CSV"
fi

# Already-done set, read once (per-task re-reads make a resume O(n^2)).
declare -A DONE
while IFS=, read -r _cat prog _rest; do
  [ -n "$prog" ] && DONE["$prog"]=1
done < <(tail -n +2 "$CSV")

started=$(date +%s)
index=-1
processed=0
skipped=0

# fd 3, not stdin: map2check inherits stdin, and consuming it would eat the
# rest of the manifest.
while IFS=$'\t' read -r category program data_model expected subproperty <&3; do
  case "$category" in ''|\#*) continue ;; esac
  index=$((index + 1))
  [ $((index % SHARDS)) -eq "$SHARD" ] || continue

  if [ -n "${DONE[$program]:-}" ]; then
    skipped=$((skipped + 1))
    continue
  fi

  now=$(date +%s)
  if [ $((now - started)) -ge "$DEADLINE_S" ]; then
    echo "[deadline] stopping after ${processed} tasks ($(( (now-started)/60 )) min)"
    break
  fi

  src="$BENCH/$program"
  if [ ! -f "$src" ]; then
    echo "$category,$program,$data_model,$expected,$subproperty,MISSING,error,0," >> "$CSV"
    continue
  fi

  # A private directory per task: map2check names its scratch by the input's
  # hash, and a directory left behind by an aborted run would be reused.
  work=$(mktemp -d)
  cp "$src" "$work/" 2>/dev/null || { rm -rf "$work"; continue; }
  name=$(basename "$src")
  arch=$([ "$data_model" = "LP64" ] && echo 64bit || echo 32bit)

  t0=$(date +%s)
  rc=0
  # shellcheck disable=SC2086  # the flag strings are word lists on purpose
  ( cd "$work" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 $((BUDGET + 30)) \
      "$MAP2CHECK" $MODE_FLAGS $EXTRA_FLAGS --architecture "$arch" \
      --timeout "$BUDGET" "$name" ) > "$work/map2check.log" 2>&1 </dev/null 3<&- || rc=$?
  t1=$(date +%s)

  output=$(cat "$work/map2check.log")
  verdict=$(classify_map2check_verdict "$output" "$rc" "$((t1 - t0))" "$BUDGET")
  class=$(classify_memsafety_result "$expected" "$subproperty" "$verdict")
  # The slice line, commas stripped so the CSV stays well-formed.
  slice=$(grep -aoE "Sliced with respect to [^:]*: [^(]*" "$work/map2check.log" \
          | tail -1 | sed 's/.*: //; s/ *$//; s/,/;/g')

  echo "$category,$program,$data_model,$expected,$subproperty,$verdict,$class,$((t1-t0)),$slice" >> "$CSV"
  printf '[%s] %-12s %-52s %-17s %-13s %s\n' \
      "$(date +%H:%M:%S)" "$category" "$(basename "$program")" "$verdict" "$class" "${slice:-—}"

  # Raw logs only for the cases worth reading: every wrong answer and error.
  case "$class" in
    wrong-*|error)
      cp "$work/map2check.log" "$RESULTS_DIR/raw/$(echo "$program" | tr '/' '_').m2c.log" 2>/dev/null ;;
  esac

  rm -rf "$work"
  processed=$((processed + 1))
done 3< "$MANIFEST"

echo "=========================================================="
echo "shard $SHARD/$SHARDS: $processed processed, $skipped already done"
echo "CSV: $CSV"
echo "=========================================================="
