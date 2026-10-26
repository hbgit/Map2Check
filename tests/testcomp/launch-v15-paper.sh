#!/bin/bash
# launch-v15-paper.sh -- the run the paper reports.
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
LOG="$REPO/tests/baseline-logs/v15-paper.log"
say() { echo "[$(date +%H:%M)] $*" | tee -a "$LOG"; }

say "waiting for the validation sweep to finish"
while [ "$(docker ps -q --filter 'name=m2c-validate' | wc -l)" -gt 0 ]; do sleep 30; done
say "validation done"

COMMON="--restart=on-failure:100 --memory=4g -u 0 -v $REPO:/workspace -w /workspace"
COMMON="$COMMON -e MAP2CHECK_PATH=/workspace/install_v15"

launch() { # arm flags property manifest shard shards
  local arm="$1" flags="$2" prop="$3" man="$4" shard="$5" shards="${6:-3}"
  docker rm -f "m2c-v15-${arm}-s${shard}" >/dev/null 2>&1
  docker run -d --name "m2c-v15-${arm}-s${shard}" $COMMON --cpus=2 \
    -e MANIFEST="/workspace/tests/testcomp/corpus/${man}" \
    -e PROPERTY="$prop" -e GENERATOR=hybrid -e EXTRA_FLAGS="$flags" \
    -e RESULTS_DIR="/workspace/tests/testcomp/results_v15_${arm}_s${shard}" \
    -e SHARD="$shard" -e SHARDS="$shards" \
    -e BUDGET=300 -e TESTCOV_S=300 -e DEADLINE_S=259200 \
    "$IMAGE" bash tests/testcomp/run_testcomp_evaluation.sh >/dev/null
}

# Phase 1: cover-error, both arms. The smaller half, and the one carrying the
# ablation, so it lands first and can be read while the rest runs.
for i in 0 1 2; do launch ce-control ""        cover-error cover-error-q400.tsv "$i"; done
for i in 0 1 2; do launch ce-slice   "--slice" cover-error cover-error-q400.tsv "$i"; done
say "phase 1 launched: cover-error control+slice, 1087 tasks"

while [ "$(docker ps -q --filter 'name=m2c-v15-ce-' | wc -l)" -gt 0 ]; do sleep "$POLL"; done
say "phase 1 done"

# Phase 2: cover-branches, control only, SIX shards -- phase 1 has released
# its cores and this is the half that has to fit.
#
# It is also the expensive half per task, and for a structural reason: a
# Cover-Branches run has no early exit. Cover-Error stops at the first
# violating path, so its median task takes 18s of a 300s budget; branch
# harvesting runs KLEE to the deadline every time, and then TestCov validates
# a suite of up to 50 vectors. Budget the pair at up to 600s per task, not 300.
#
# 2765 tasks over six shards at up to 600s is ~77h worst case and far less in
# the mean, inside the 72h deadline with the tail truncating rather than the
# body. The manifest is round-robin, so what truncates stays stratified.
for i in 0 1 2 3 4 5; do launch cb-control "" cover-branches cover-branches-q400.tsv "$i" 6; done
say "phase 2 launched: cover-branches control, 2765 tasks over 6 shards"
