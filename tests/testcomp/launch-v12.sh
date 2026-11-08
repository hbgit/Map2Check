#!/bin/bash
# launch-v12.sh -- remeasures slicing after the weak-stub fix.
#
# v11 measured --slice at 24.4% against a 44.6% control and the cause turned
# out to be mundane: sbt-slicer drops the BODY of the criterion function, the
# native LibFuzzer link then fails with "undefined reference to reach_error",
# no fuzzed binary is produced, and the fuzzer stage silently does nothing.
# caller.cpp now links a weak definition back in after slicing.
#
# The control cell is re-run rather than reusing v11's, for the reason v11's
# own launcher gives: v11 ran twelve containers, v12 runs six, and comparing
# across that contention difference would carry the confound the internal
# control exists to remove.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${IMAGE:-map2check-testcov:latest}"

COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v12"

launch() {
  local arm="$1" flags="$2" shard="$3" shards="$4"
  docker rm -f "m2c-v12-${arm}-s${shard}" >/dev/null 2>&1
  docker run -d --name "m2c-v12-${arm}-s${shard}" $COMMON --cpus=2 \
    -e MANIFEST="/workspace/tests/testcomp/corpus/cover-error.tsv" \
    -e PROPERTY="cover-error" -e GENERATOR=hybrid -e EXTRA_FLAGS="$flags" \
    -e RESULTS_DIR="/workspace/tests/testcomp/results_v12_${arm}_s${shard}" \
    -e SHARD="$shard" -e SHARDS="$shards" \
    -e BUDGET=60 -e TESTCOV_S=90 -e DEADLINE_S=28800 \
    "$IMAGE" bash tests/testcomp/run_testcomp_evaluation.sh >/dev/null
}

for i in 0 1 2; do launch control ""        "$i" 3; done
for i in 0 1 2; do launch slice   "--slice" "$i" 3; done
sleep 5
echo "v12 launched: $(docker ps -q --filter 'name=m2c-v12-' | wc -l) containers"
