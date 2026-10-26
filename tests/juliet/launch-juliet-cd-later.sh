#!/bin/bash
# Shards c and d wait for a and b. The Juliet harness appends to its CSV and
# skips what it already has, so a shard that starts late loses nothing.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${IMAGE:-map2check-testcov:latest}"
while [ "$(docker ps -q --filter 'name=m2c-v12-juliet' | wc -l)" -gt 0 ]; do sleep 300; done
COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace --cpus=1"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v12"
for pair in "c:190 191" "d:401 415 416 476"; do
  s="${pair%%:*}"; cwes="${pair#*:}"
  docker rm -f "m2c-v12-juliet-$s" >/dev/null 2>&1
  docker run -d --name "m2c-v12-juliet-$s" $COMMON \
    -e JULIET_CWES="$cwes" -e RESULTS_DIR="/workspace/tests/juliet/results_v12_$s" \
    "$IMAGE" bash tests/juliet/run_juliet_evaluation.sh >/dev/null
done
echo "juliet c+d launched"
