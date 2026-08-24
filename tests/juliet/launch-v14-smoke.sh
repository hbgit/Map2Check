#!/bin/bash
# A smoke, not a campaign. The verdict guard is gated on --generate-test-suite,
# reachability mode and the KLEE generator; Juliet and CASTLE use none of the
# three, so the guard cannot reach them. This run is there to prove that claim
# rather than assert it, which is cheap at one file per family.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${IMAGE:-map2check-testcov:latest}"
while [ "$(docker ps -q --filter 'name=m2c-v12-juliet' --filter 'name=m2c-v12-castle' | wc -l)" -gt 0 ]; do sleep 300; done
COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace --cpus=1"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v14"
docker rm -f m2c-v14-juliet-smoke m2c-v14-castle-smoke >/dev/null 2>&1
docker run -d --name m2c-v14-juliet-smoke $COMMON -e PER_FAMILY=1 \
  -e RESULTS_DIR="/workspace/tests/juliet/results_v14_smoke" \
  "$IMAGE" bash tests/juliet/run_juliet_evaluation.sh >/dev/null
docker run -d --name m2c-v14-castle-smoke $COMMON \
  -e RESULTS_DIR="/workspace/tests/castle/results_v14_smoke" \
  "$IMAGE" bash tests/castle/run_castle_evaluation.sh >/dev/null
echo "v14 smoke launched"
