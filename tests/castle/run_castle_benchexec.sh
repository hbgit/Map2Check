#!/bin/bash
# run_castle_benchexec.sh — CASTLE C250 via BenchExec inside map2check-dev Docker
set -e

REPO="/home/guilherme/github/Map2Check"
LOG="$REPO/tests/castle/results/benchexec_run.log"
mkdir -p "$(dirname "$LOG")"

echo "=== CASTLE BenchExec started: $(date) ===" | tee "$LOG"

docker run --rm -u root \
  --privileged \
  -v "$REPO:/workspace" \
  -w /workspace \
  ghcr.io/hbgit/map2check-dev:latest bash -c '
    set -e
    LOG=/workspace/tests/castle/results/benchexec_run.log

    echo "=== Install BenchExec: $(date)" | tee -a $LOG
    pip3 install benchexec 2>&1 | tail -3 | tee -a $LOG

    echo "=== Build Map2Check: $(date)" | tee -a $LOG
    rm -rf /workspace/build_be /workspace/release
    mkdir -p /workspace/build_be && cd /workspace/build_be
    cmake .. -G Ninja \
      -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
      -DENABLE_TEST=ON \
      -DMAP2CHECK_DYNAMIC_LINK=ON \
      -DCMAKE_INSTALL_PREFIX=/workspace/release
    ninja 2>&1 | tail -5 | tee -a $LOG
    ninja install 2>&1 | tail -5 | tee -a $LOG

    # Map2Check binary is installed at release/map2check directly
    cp /workspace/utils/map2check-wrapper.py /workspace/release/map2check-wrapper.py

    mkdir -p /workspace/release/lib/klee
    ln -sf /opt/klee/lib/klee/runtime /workspace/release/lib/klee/runtime
    ln -sf /usr/lib/llvm-16/lib/clang /workspace/release/lib/clang

    # Symlink Z3 for --smt-solver flag in wrapper
    ln -sf /opt/klee/lib/libz3* /workspace/release/lib/ 2>/dev/null || true

    echo "=== Run BenchExec: $(date)" | tee -a $LOG
    mkdir -p /workspace/release/results

    cd /workspace/release
    python3 -m benchexec.benchexec --no-container \
      ../tests/castle/benchexec/map2check_castle.xml 2>&1 | tee -a $LOG

    echo "=== Completed: $(date)" | tee -a $LOG
  ' 2>&1 | tee -a "$LOG"

echo "=== CASTLE BenchExec finished: $(date) ===" | tee -a "$LOG"
echo "Results in: release/results/"
ls -la "$REPO/release/results/"
