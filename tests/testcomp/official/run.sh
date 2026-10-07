#!/bin/bash
# Runs a Test-Comp 2026 step (testcomp.py generate | validate) in the
# reproduction image.
#
#   tests/testcomp/official/run.sh generate --property cover-branches \
#       --tasks tests/testcomp/corpus/cover-branches-q400.tsv \
#       --archive map2check-testcomp.zip --suites OUT/suites --out OUT/gen
#   tests/testcomp/official/run.sh validate --property cover-branches \
#       --tasks tests/testcomp/corpus/cover-branches-q400.tsv \
#       --suites OUT/suites --out OUT/val
#
# Anything after -- goes to BenchExec (e.g. -- --timelimit 300s for a run
# deliberately shorter than the competition's).
#
# The image is map2check-testcomp-env (docker build -t map2check-testcomp-env
# tests/testcomp/official). BenchExec needs --privileged to manage cgroups and
# /dev/fuse for the overlays it creates.
#
# The repository is mounted at /m2c and $DATA_DIR (default: the repository's
# parent) at /data, and the paths given on the command line are translated: the
# container's /home must stay free, because BenchExec overlays it, and an
# overlay over a bind mount below it fails ("Failed to create overlay mount").
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
    *) echo "path outside the repository and DATA_DIR: $1" >&2; exit 2 ;;
  esac
}

args=()
expect_path=0
for arg in "$@"; do
  if [ "$expect_path" = 1 ]; then args+=("$(inside "$arg")"); expect_path=0; continue; fi
  case "$arg" in
    --tasks|--suites|--out|--archive) args+=("$arg"); expect_path=1 ;;
    *) args+=("$arg") ;;
  esac
done
step=${args[0]:?generate or validate}

exec docker run --rm --privileged --cap-drop=all --device /dev/fuse \
  -v "$REPO:/m2c" -v "$DATA_DIR:/data" -w /m2c -e HOST_OWNER="$(id -u):$(id -g)" \
  ${DOCKER_ARGS:-} "$IMAGE" \
  python3 /m2c/tests/testcomp/official/testcomp.py "$step" \
  --sv-benchmarks "$(inside "$SV")" "${args[@]:1}"
