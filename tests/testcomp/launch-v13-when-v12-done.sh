#!/bin/bash
# launch-v13-when-v12-done.sh -- chains the FALSE-FAILED guard measurement.
#
# v13 adds one change on top of v12: a recorded violation is no longer
# reported as FAILED when no input vector can be recovered. The claim is that
# this is score-neutral (an empty suite covers nothing under either label) and
# precision-positive. Both halves are checkable, and this run checks them.
#
# Chained rather than parallel for the reason v11's launcher gives: v12 and
# v13 differ by one commit, so a contention difference between them would be
# indistinguishable from the effect being measured.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${IMAGE:-map2check-testcov:latest}"
POLL="${POLL:-300}"

while [ "$(docker ps -q --filter 'name=m2c-v12-control' --filter 'name=m2c-v12-slice' | wc -l)" -gt 0 ]; do
  sleep "$POLL"
done
echo "[$(date +%H:%M)] v12 finished, launching v13"

COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v13 --cpus=2"
for i in 0 1 2; do
  docker rm -f "m2c-v13-guard-s${i}" >/dev/null 2>&1
  docker run -d --name "m2c-v13-guard-s${i}" $COMMON \
    -e MANIFEST="/workspace/tests/testcomp/corpus/cover-error.tsv" \
    -e PROPERTY="cover-error" -e GENERATOR=hybrid -e EXTRA_FLAGS="" \
    -e RESULTS_DIR="/workspace/tests/testcomp/results_v13_guard_s${i}" \
    -e SHARD="$i" -e SHARDS=3 \
    -e BUDGET=60 -e TESTCOV_S=90 -e DEADLINE_S=28800 \
    "$IMAGE" bash tests/testcomp/run_testcomp_evaluation.sh >/dev/null
done
echo "v13 launched: $(docker ps -q --filter 'name=m2c-v13-' | wc -l) containers"
