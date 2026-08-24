#!/bin/bash
# launch-v14-when-v12-done.sh -- the four fixes, at competition budget.
#
# BUDGET=300 rather than 60. Everything measured so far ran at 60s, which is a
# throughput choice, not a competition one -- and three of the four fixes this
# arm carries are about what happens when a run reaches its deadline, so the
# deadline is exactly the variable that must be right.
#
# The consequence has to be stated plainly: a COVERED rate at 300s is NOT
# comparable to v12's at 60s. More budget finds more bugs on its own. That is
# why slice still runs beside its own control here -- the slice-vs-control
# comparison stays valid because both arms move together, while the absolute
# number becomes the competition-conditions one.
#
# Chained behind v12 for the reason v11's launcher gives: two campaigns sharing
# a machine experience different contention.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${IMAGE:-map2check-testcov:latest}"
POLL="${POLL:-300}"

while [ "$(docker ps -q --filter 'name=m2c-v12-control' --filter 'name=m2c-v12-slice' | wc -l)" -gt 0 ]; do
  sleep "$POLL"
done
echo "[$(date +%H:%M)] v12 finished, launching v14 at 300s"

COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v14"

launch() {
  local arm="$1" flags="$2" shard="$3"
  docker rm -f "m2c-v14-${arm}-s${shard}" >/dev/null 2>&1
  docker run -d --name "m2c-v14-${arm}-s${shard}" $COMMON --cpus=2 \
    -e MANIFEST="/workspace/tests/testcomp/corpus/cover-error.tsv" \
    -e PROPERTY="cover-error" -e GENERATOR=hybrid -e EXTRA_FLAGS="$flags" \
    -e RESULTS_DIR="/workspace/tests/testcomp/results_v14_${arm}_s${shard}" \
    -e SHARD="$shard" -e SHARDS=3 \
    -e BUDGET=300 -e TESTCOV_S=300 -e DEADLINE_S=43200 \
    "$IMAGE" bash tests/testcomp/run_testcomp_evaluation.sh >/dev/null
}
for i in 0 1 2; do launch control ""        "$i"; done
for i in 0 1 2; do launch slice   "--slice" "$i"; done

# CASTLE and Juliet on the same binary. Their own harnesses carry their own
# budgets -- 300s inner for CASTLE already, 60s for Juliet -- and those are
# not the Test-Comp budget, so they stay as they are: this run answers "did
# the fixes break anything", and changing two variables would stop it doing so.
#
# Two Juliet shards, not four. Container count is what oversubscribes this
# machine, not container CPU quota; c and d chain behind a and b.
RCOMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace --cpus=1"
RCOMMON="$RCOMMON -e MAP2CHECK_PATH=/workspace/install_v14"
docker rm -f m2c-v14-juliet-a m2c-v14-juliet-b m2c-v14-castle >/dev/null 2>&1
docker run -d --name m2c-v14-juliet-a $RCOMMON -e JULIET_CWES="121" \
  -e RESULTS_DIR="/workspace/tests/juliet/results_v14_a" \
  "$IMAGE" bash tests/juliet/run_juliet_evaluation.sh >/dev/null
docker run -d --name m2c-v14-juliet-b $RCOMMON -e JULIET_CWES="122 761" \
  -e RESULTS_DIR="/workspace/tests/juliet/results_v14_b" \
  "$IMAGE" bash tests/juliet/run_juliet_evaluation.sh >/dev/null
docker run -d --name m2c-v14-castle $RCOMMON \
  -e RESULTS_DIR="/workspace/tests/castle/results_v14" \
  "$IMAGE" bash tests/castle/run_castle_evaluation.sh >/dev/null

echo "v14 launched: $(docker ps -q --filter 'name=m2c-v14-' | wc -l) containers"

# Juliet c and d wait for a and b. The harness appends and skips what it
# already has, so a late shard loses nothing.
while [ "$(docker ps -q --filter 'name=m2c-v14-juliet' | wc -l)" -gt 0 ]; do sleep "$POLL"; done
docker run -d --name m2c-v14-juliet-c $RCOMMON -e JULIET_CWES="190 191" \
  -e RESULTS_DIR="/workspace/tests/juliet/results_v14_c" \
  "$IMAGE" bash tests/juliet/run_juliet_evaluation.sh >/dev/null
docker run -d --name m2c-v14-juliet-d $RCOMMON -e JULIET_CWES="401 415 416 476" \
  -e RESULTS_DIR="/workspace/tests/juliet/results_v14_d" \
  "$IMAGE" bash tests/juliet/run_juliet_evaluation.sh >/dev/null
echo "[$(date +%H:%M)] juliet c+d launched"
