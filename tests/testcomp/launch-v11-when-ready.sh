#!/bin/bash
# launch-v11-when-ready.sh -- chains the v11 measurement behind two conditions.
#
# Not run in parallel with v10, deliberately. v11 exists to answer "what do
# slicing and seed exchange buy", which is a COMPARISON against v10 -- and two
# campaigns sharing a machine experience different contention, so a difference
# between them would not be attributable to the features. The tool's LibFuzzer
# phase runs with -jobs=8, so ten containers already put this machine at twice
# its core count; adding ten more would make both numbers meaningless.
#
# Waits for:
#   1. every v10 container to finish
#   2. the republished image to actually carry the slicer
#
# The second is not a formality. --slice degrades gracefully when sbt-slicer is
# absent -- it warns and analyses the whole program -- so launching against an
# image without it would produce a full campaign that silently measures
# nothing. Checking is the difference between an answer and a wasted night.

set -u

REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE_SRC="${IMAGE_SRC:-ghcr.io/hbgit/map2check-dev:latest}"
IMAGE="${IMAGE:-map2check-testcov:latest}"
LOG="$REPO/tests/baseline-logs/v11-launcher.log"
POLL="${POLL:-300}"

say() { echo "[$(date +%H:%M)] $*" | tee -a "$LOG"; }

say "waiting for the v10 campaign to finish"
while [ "$(docker ps -q --filter 'name=m2c-v10-' | wc -l)" -gt 0 ]; do
  sleep "$POLL"
done
say "v10 finished"

say "waiting for an image that carries the slicer"
while true; do
  docker pull -q "$IMAGE_SRC" >/dev/null 2>&1 || true
  if docker run --rm "$IMAGE_SRC" test -x /opt/sbt-slicer/bin/sbt-slicer >/dev/null 2>&1; then
    say "the published image has sbt-slicer"
    break
  fi
  say "  not yet -- the PR has to merge and Publish Docker Image has to run"
  sleep "$POLL"
done

# The testcov derivative has to be rebuilt on top of the new base, or the runs
# would use the old one and the wait above would have been pointless.
say "rebuilding $IMAGE on top of the new base"
docker build -q -t "$IMAGE" -f - "$REPO" >>"$LOG" 2>&1 <<EOF
FROM $IMAGE_SRC
USER root
RUN apt-get update -qq && \
    apt-get install -y -qq python3-pip zip gcc gcc-multilib lcov && \
    pip3 install --quiet testcov && rm -rf /var/lib/apt/lists/*
EOF
docker run --rm "$IMAGE" test -x /opt/sbt-slicer/bin/sbt-slicer || {
  say "FATAL: the rebuilt image lost the slicer"; exit 1; }
say "image ready"

COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v9"

# Two arms, so each feature is separable rather than entangled:
#   slice        vs v10  -> what slicing buys
#   slice+seed   vs slice -> what the exchange adds on top
launch() {
  local arm="$1" flags="$2" prop="$3" shard="$4" shards="$5"
  docker rm -f "m2c-v11-${arm}-${prop}-s${shard}" >/dev/null 2>&1
  docker run -d --name "m2c-v11-${arm}-${prop}-s${shard}" $COMMON --cpus=1.5 \
    -e MANIFEST="/workspace/tests/testcomp/corpus/${prop}.tsv" \
    -e PROPERTY="$prop" -e GENERATOR=hybrid -e EXTRA_FLAGS="$flags" \
    -e RESULTS_DIR="/workspace/tests/testcomp/results_v11_${arm}_${prop}_s${shard}" \
    -e SHARD="$shard" -e SHARDS="$shards" \
    -e BUDGET=60 -e TESTCOV_S=90 -e DEADLINE_S=28800 \
    "$IMAGE" bash tests/testcomp/run_testcomp_evaluation.sh >/dev/null
}

# cover-error only. Slicing needs a criterion to slice towards, and
# Cover-Branches has none -- every branch is the goal.
for i in 0 1 2 3; do launch slice      "--slice" cover-error "$i" 4; done
for i in 0 1 2 3; do launch slice-seed "--slice --seed-exchange" cover-error "$i" 4; done

sleep 5
say "v11 launched: $(docker ps -q --filter 'name=m2c-v11-' | wc -l) containers"
