#!/bin/bash
# launch-v12-regression.sh -- non-regression sweep for the weak-stub slicing fix.
#
# The fix only touches Caller::sliceWithRespectToTarget, which nothing outside
# --slice calls, so the expected result is TP/FP/FN identical to v9. That is
# exactly why it is worth running: a difference here means the change reached
# somewhere it had no business reaching.
#
# Sized to leave the v12 Test-Comp campaign its cores. That campaign is the
# measurement with a deadline; this one only has to finish before it is read.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${IMAGE:-map2check-testcov:latest}"
COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v12 --cpus=1"

# Same CWE split as v9, so the comparison is shard-for-shard.
launch_juliet() {
  docker rm -f "m2c-v12-juliet-$1" >/dev/null 2>&1
  docker run -d --name "m2c-v12-juliet-$1" $COMMON \
    -e JULIET_CWES="$2" -e RESULTS_DIR="/workspace/tests/juliet/results_v12_$1" \
    "$IMAGE" bash tests/juliet/run_juliet_evaluation.sh >/dev/null
}
launch_juliet a "121"
launch_juliet b "122 761"
launch_juliet c "190 191"
launch_juliet d "401 415 416 476"

docker rm -f m2c-v12-castle >/dev/null 2>&1
docker run -d --name m2c-v12-castle $COMMON \
  -e RESULTS_DIR="/workspace/tests/castle/results_v12" \
  "$IMAGE" bash tests/castle/run_castle_evaluation.sh >/dev/null

sleep 5
echo "regression launched: $(docker ps -q --filter 'name=m2c-v12-juliet' | wc -l) juliet + $(docker ps -q --filter 'name=m2c-v12-castle' | wc -l) castle"
