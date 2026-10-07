#!/bin/bash
# Validates test suites with the five Test-Comp 2026 validators, under
# BenchExec, in the reproduction image. See validate.py for what is official
# and what is rewritten.
#
#   tests/testcomp/official/validate.sh --property cover-branches \
#       --tasks tests/testcomp/corpus/cover-branches-q400.tsv \
#       --suites /path/to/suites --out /path/to/out [-- --timelimit 60]
#
# The image is map2check-testcomp-env (docker build -t map2check-testcomp-env
# tests/testcomp/official). BenchExec needs --privileged to manage cgroups, and
# /dev/fuse for the overlay it puts on /home.
#
# The repository is mounted at /m2c and $DATA_DIR (default: the repository's
# parent) at /data, and paths given on the command line are translated: the
# container's /home must stay free, since BenchExec overlays it and an overlay
# over a bind mount under it fails ("Failed to create overlay mount").
set -euo pipefail
REPO=$(cd "$(dirname "$0")/../../.." && pwd)
DATA_DIR=$(cd "${DATA_DIR:-$REPO/..}" && pwd)
IMAGE=${IMAGE:-map2check-testcomp-env}
SV=${SV_BENCHMARKS:-$REPO/tests/testcomp/bench/sv-benchmarks/c}

inside() {
  local path
  path=$(realpath -m "$1")
  case "$path" in
    "$REPO"|"$REPO"/*) echo "/m2c${path#"$REPO"}" ;;
    "$DATA_DIR"/*) echo "/data${path#"$DATA_DIR"}" ;;
    *) echo "$1" ;;
  esac
}

args=()
expect_path=0
for arg in "$@"; do
  if [ "$expect_path" = 1 ]; then args+=("$(inside "$arg")"); expect_path=0; continue; fi
  case "$arg" in
    --tasks|--suites|--out) args+=("$arg"); expect_path=1 ;;
    *) args+=("$arg") ;;
  esac
done

exec docker run --rm --privileged --cap-drop=all --device /dev/fuse \
  -v "$REPO:/m2c" -v "$DATA_DIR:/data" -w /m2c -e HOST_OWNER="$(id -u):$(id -g)" "$IMAGE" \
  python3 /m2c/tests/testcomp/official/validate.py --sv-benchmarks "$(inside "$SV")" "${args[@]}"
