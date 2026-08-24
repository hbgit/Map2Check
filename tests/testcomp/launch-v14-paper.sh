#!/bin/bash
# launch-v14-paper.sh -- the run the paper reports.
#
# Shape, and the reasoning for each part:
#
#   cover-error      control + slice, 1087 tasks
#     1087 is the ENTIRE applicable pool, not a sample -- quota 400 caps only
#     ECA (421 applicable). Slicing runs here because here it has a criterion.
#
#   cover-branches   control only, 2765 tasks
#     Slicing needs something to slice TOWARDS and Cover-Branches has nothing:
#     every branch is the goal. Paying for a slice arm here would measure the
#     absence of a feature.
#
#   Juliet           PER_FAMILY=10, CASTLE full
#     Non-regression, on the same binary, at THEIR budgets rather than 300s --
#     this run answers "did the four fixes break anything", and moving two
#     variables at once would stop it answering that.
#
# 4939 Test-Comp tasks. BUDGET=300 is the competition figure; three of the four
# fixes this binary carries are about what happens at the deadline, so the
# deadline has to be the real one.
#
# The corpus is nested: quota 400 is a SUPERSET of the 40 and 150 used before,
# in the same order, and the manifest is round-robin across categories. A run
# truncated by its deadline therefore stays stratified -- partial results are
# read without bias, which matters because this one will be read before it ends.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${IMAGE:-map2check-testcov:latest}"
POLL="${POLL:-300}"
LOG="$REPO/tests/baseline-logs/v14-paper.log"
say() { echo "[$(date +%H:%M)] $*" | tee -a "$LOG"; }

say "waiting for v12 to finish"
while [ "$(docker ps -q --filter 'name=m2c-v12-' | wc -l)" -gt 0 ]; do sleep "$POLL"; done
say "v12 done"

COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v14"

launch() { # arm flags property manifest shard
  local arm="$1" flags="$2" prop="$3" man="$4" shard="$5"
  docker rm -f "m2c-v14-${arm}-s${shard}" >/dev/null 2>&1
  docker run -d --name "m2c-v14-${arm}-s${shard}" $COMMON --cpus=2 \
    -e MANIFEST="/workspace/tests/testcomp/corpus/${man}" \
    -e PROPERTY="$prop" -e GENERATOR=hybrid -e EXTRA_FLAGS="$flags" \
    -e RESULTS_DIR="/workspace/tests/testcomp/results_v14_${arm}_s${shard}" \
    -e SHARD="$shard" -e SHARDS=3 \
    -e BUDGET=300 -e TESTCOV_S=300 -e DEADLINE_S=172800 \
    "$IMAGE" bash tests/testcomp/run_testcomp_evaluation.sh >/dev/null
}

# Phase 1: cover-error, both arms. The smaller half, and the one carrying the
# ablation, so it lands first and can be read while the rest runs.
for i in 0 1 2; do launch ce-control ""        cover-error cover-error-q400.tsv "$i"; done
for i in 0 1 2; do launch ce-slice   "--slice" cover-error cover-error-q400.tsv "$i"; done
say "phase 1 launched: cover-error control+slice, 1087 tasks"

while [ "$(docker ps -q --filter 'name=m2c-v14-ce-' | wc -l)" -gt 0 ]; do sleep "$POLL"; done
say "phase 1 done"

# Phase 2: cover-branches, control only. Six shards rather than three: nothing
# else is running now, and this is the long half.
for i in 0 1 2; do launch cb-control "" cover-branches cover-branches-q400.tsv "$i"; done
say "phase 2 launched: cover-branches control, 2765 tasks"
