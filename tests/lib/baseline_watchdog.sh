#!/bin/bash
# baseline_watchdog.sh — storage guard for long baseline runs.
#
#   baseline_watchdog.sh <monitor_log> <min_free_gb> <interval_s> <container>...
#
# Samples disk, docker log sizes and results-dir sizes on every tick, appends a
# machine-readable line to the monitor log, and HALTS the named containers if
# free space falls under the floor. Observation alone is not enough: a baseline
# left unattended for hours can fill a disk between two glances, and a filled
# disk corrupts the CSV it was writing at the time. Stopping is recoverable
# because both runners resume from their CSV; a truncated CSV is not.
#
# Docker's default json-file driver is unbounded, so the containers must ALSO be
# started with --log-opt max-size/max-file. This watchdog reports the sizes so a
# missing rotation flag is visible rather than silent.

set -u

LOG="$1"; MIN_FREE_GB="$2"; INTERVAL="$3"; shift 3
CONTAINERS=("$@")
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

# Which run to count rows for. Everything else here is derived from arguments,
# so a hardcoded tag would make the progress counters read a stale directory and
# report 0 rows forever on the next run -- silently, while the disk guard kept
# working. A wrong monitoring number is worse than none, so the tag is explicit.
: "${BASELINE_TAG:=v5}"
JULIET_GLOB="$REPO/tests/juliet/results_${BASELINE_TAG}_*/juliet_scope_c_results.csv"
CASTLE_CSV="$REPO/tests/castle/results_${BASELINE_TAG}/castle_results.csv"

say() { printf '%s %s\n' "$(date -u +%H:%M:%S)" "$*" >> "$LOG"; }

say "WATCHDOG start floor=${MIN_FREE_GB}G interval=${INTERVAL}s containers=${CONTAINERS[*]}"

while :; do
  free_gb=$(df -BG --output=avail "$REPO" | tail -1 | tr -dc '0-9')
  # `du /var/lib/docker` needs root and silently under-reports without it, which
  # is exactly the kind of quietly-wrong number this script exists to avoid.
  # `docker system df` reports the same total over the API, no privileges needed.
  docker_gb=$(docker system df --format '{{.Size}}' 2>/dev/null \
                | awk '/GB/{s+=$1} /MB/{s+=$1/1024} END {printf "%d", s+0}')
  res_mb=$(du -sBM "$REPO/tests/castle" "$REPO/tests/juliet" 2>/dev/null \
             | awk '{s+=$1} END {print s+0}')

  # Container log sizes (the unbounded-by-default risk) and liveness.
  logs_mb=0; alive=0
  for c in "${CONTAINERS[@]}"; do
    st=$(docker inspect -f '{{.State.Status}}' "$c" 2>/dev/null || echo gone)
    [ "$st" = "running" ] && alive=$((alive+1))
    lf=$(docker inspect -f '{{.LogPath}}' "$c" 2>/dev/null || true)
    if [ -n "$lf" ] && [ -r "$lf" ]; then
      sz=$(du -sBM "$lf" 2>/dev/null | cut -f1 | tr -dc '0-9')
      logs_mb=$((logs_mb + ${sz:-0}))
    fi
  done

  # shellcheck disable=SC2086  # JULIET_GLOB must stay unquoted to expand
  rows=$(cat $JULIET_GLOB 2>/dev/null | grep -cv '^cwe,' || true)
  crows=$(($(wc -l < "$CASTLE_CSV" 2>/dev/null || echo 1) - 1))

  say "free=${free_gb}G docker=${docker_gb:-?}G results=${res_mb}M clogs=${logs_mb}M alive=${alive}/${#CONTAINERS[@]} juliet_rows=${rows:-0} castle_rows=${crows}"

  if [ "${free_gb:-9999}" -lt "$MIN_FREE_GB" ]; then
    say "ALERT free=${free_gb}G below floor ${MIN_FREE_GB}G -- STOPPING CONTAINERS"
    docker stop "${CONTAINERS[@]}" >/dev/null 2>&1
    say "HALTED. Both runners resume from their CSV once space is reclaimed."
    exit 1
  fi

  # Everything finished: nothing left to guard.
  if [ "$alive" -eq 0 ]; then
    say "all containers finished; watchdog exiting"
    exit 0
  fi

  sleep "$INTERVAL"
done
