#!/bin/bash
# Non-regression on the paper binary. Runs BESIDE the Test-Comp phases, at one
# CPU each, because container count is what oversubscribes this machine.
#
# PER_FAMILY=10 rather than the 3 every previous baseline used. That buys much
# wider CWE coverage and costs the line-by-line comparison with v9 -- so the
# comparison is made on the INTERSECTION, which is still every case v9 ran.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${IMAGE:-map2check-testcov:latest}"
POLL="${POLL:-300}"
R="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace --cpus=1"
R="$R -e MAP2CHECK_PATH=/workspace/install_v15 -e PER_FAMILY=10"

while [ "$(docker ps -q --filter 'name=m2c-validate' | wc -l)" -gt 0 ]; do sleep 30; done


jul() {
  docker rm -f "m2c-v15-juliet-$1" >/dev/null 2>&1
  docker run -d --name "m2c-v15-juliet-$1" $R -e JULIET_CWES="$2" \
    -e RESULTS_DIR="/workspace/tests/juliet/results_v15_$1" \
    "$IMAGE" bash tests/juliet/run_juliet_evaluation.sh >/dev/null
}
docker rm -f m2c-v15-castle >/dev/null 2>&1
docker run -d --name m2c-v15-castle $R \
  -e RESULTS_DIR="/workspace/tests/castle/results_v15" \
  "$IMAGE" bash tests/castle/run_castle_evaluation.sh >/dev/null
jul a "121"
jul b "122 761"
echo "castle + juliet a,b launched"

while [ "$(docker ps -q --filter 'name=m2c-v15-juliet' | wc -l)" -gt 0 ]; do sleep "$POLL"; done
jul c "190 191"
jul d "401 415 416 476"
echo "juliet c,d launched"
