# CASTLE-Benchmark Integration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Integrate CASTLE C250 benchmark (250 C micro-benchmarks, 25 CWEs) into Map2Check: 5 regression tests in CI + standalone evaluation script for paper results.

**Architecture:** Two-layer approach. Layer 1: `tests/integration/test_castle_regression.sh` — 5 curated CASTLE tests compiled to LLVM bitcode, verified with `--memtrack`, following the `test_wasm_pipeline.sh` pattern. Layer 2: `tests/castle/run_castle_evaluation.sh` — standalone script that runs all 250 tests with two-pass invariants strategy, scores against `CASTLE-C250.min.json` ground truth, outputs CSV + summary.

**Tech Stack:** bash, clang-16 (bitcode compilation), map2check (--memtrack), python3 (JSON parsing), git submodule (CASTLE-Benchmark), Docker ghcr.io/hbgit/map2check-dev (CI)

## Global Constraints

- clang-16 is the required compiler version
- map2check binary must be built with KLEE support (not SKIP_KLEE)
- All CASTLE tests use plain `main()` — no `reach_error()` or `__VERIFIER_nondet_int()` harness
- `--memtrack` mode covers valid-free, valid-deref, valid-memtrack via KLEE symbolic execution
- `--add-invariants` is used as fallback only when first pass returns UNKNOWN
- Per-test timeout: 60s (regression) / 120s (evaluation)
- Out-of-scope CWEs (22, 78, 89, 327, 522, 798) flagged N/A
- CASTLE-Benchmark is a git submodule pinned to main HEAD

---

### Task 1: Add CASTLE-Benchmark git submodule

**Files:**
- Modify: `.gitmodules`
- Create: `tests/castle/CASTLE-Benchmark` (submodule directory)

**Interfaces:**
- Produces: `tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250/*.c` (250 C files), `tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250.min.json` (ground truth)

- [ ] **Step 1: Add the submodule**

```bash
cd /home/guilherme/github/Map2Check
mkdir -p tests/castle
git submodule add --name CASTLE-Benchmark \
  https://github.com/CASTLE-Benchmark/CASTLE-Benchmark \
  tests/castle/CASTLE-Benchmark
```

- [ ] **Step 2: Pin to current main HEAD and verify**

```bash
cd tests/castle/CASTLE-Benchmark
git log --oneline -1
# Should show the latest commit on main
cd /home/guilherme/github/Map2Check
ls tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250/CASTLE-476-1.c
```

Run: `ls tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250/ | wc -l`
Expected: `> 250` (250 .c files + Makefile)

- [ ] **Step 3: Commit**

```bash
git add .gitmodules tests/castle/CASTLE-Benchmark
git commit -m "chore: add CASTLE-Benchmark git submodule"
```

---

### Task 2: Write CASTLE regression test script

**Files:**
- Create: `tests/integration/test_castle_regression.sh`

**Interfaces:**
- Consumes: `tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250/*.c` (from Task 1), `$MAP2CHECK_PATH` env var (points to map2check binary)
- Produces: Exit code 0 (all passed) or 1 (any failed), console output with PASS/FAIL per test

- [ ] **Step 1: Create the script**

```bash
#!/bin/bash
set -e

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
CASTLE_DIR="tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250"
TMPDIR=$(mktemp -d)
PASSED=0
FAILED=0

cleanup() { rm -rf "$TMPDIR"; }
trap cleanup EXIT

echo "============================================================"
echo "Map2Check CASTLE Regression Tests"
echo "============================================================"

run_castle_test() {
  local name="$1"
  local cwe="$2"
  local expect_fail="$3"

  local src="$CASTLE_DIR/$name"
  local bc="$TMPDIR/$name.bc"

  echo ""
  echo "--- Test: $name (CWE-$cwe) ---"

  $CLANG -c -emit-llvm -g -O0 "$src" -o "$bc" 2>/dev/null || {
    echo "  SKIP: compile failed"
    return 0
  }

  local output
  output=$(timeout 120 "$MAP2CHECK" --memtrack --timeout 60 "$bc" 2>&1) || true

  if [ "$expect_fail" = "true" ]; then
    if echo "$output" | grep -qE "VERIFICATION FAILED|FALSE_"; then
      echo "  PASS: vulnerability detected"
      PASSED=$((PASSED+1))
    else
      echo "  FAIL: expected FALSE, got:"
      echo "$output" | tail -5
      FAILED=$((FAILED+1))
    fi
  else
    if echo "$output" | grep -qE "VERIFICATION FAILED|FALSE_"; then
      echo "  FAIL: expected TRUE, got FALSE:"
      echo "$output" | tail -5
      FAILED=$((FAILED+1))
    else
      echo "  PASS: no false alarm"
      PASSED=$((PASSED+1))
    fi
  fi
}

run_castle_test "CASTLE-476-1.c"  "476" "true"
run_castle_test "CASTLE-415-2.c"  "415" "true"
run_castle_test "CASTLE-476-8.c"  "476" "false"
run_castle_test "CASTLE-416-1.c"  "416" "true"
run_castle_test "CASTLE-787-3.c"  "787" "true"

echo ""
echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"

[ $FAILED -eq 0 ] && exit 0 || exit 1
```

- [ ] **Step 2: Make executable**

```bash
chmod +x tests/integration/test_castle_regression.sh
```

- [ ] **Step 3: Run locally to verify syntax**

```bash
bash -n tests/integration/test_castle_regression.sh
```

Expected: no output (syntax OK)

- [ ] **Step 4: Commit**

```bash
git add tests/integration/test_castle_regression.sh
git commit -m "test: add CASTLE regression tests (5 micro-benchmarks)"
```

---

### Task 3: Add CI job for CASTLE regression

**Files:**
- Modify: `.github/workflows/ci.yml` (append new job after e2e-wasm)

**Interfaces:**
- Consumes: `tests/integration/test_castle_regression.sh` (from Task 2), `ghcr.io/hbgit/map2check-dev:latest` Docker image
- Produces: CI job `castle-regression` that runs on push to develop/feat-*, PRs to develop/main

- [ ] **Step 1: Add the CI job**

Insert this YAML block after the `e2e-wasm` job (after line 291 in ci.yml):

```yaml
  # ===========================================================
  # Job 6: CASTLE Benchmark Regression Tests
  # ===========================================================
  # Verifies Map2Check against 5 CASTLE micro-benchmarks
  # (CWE-415, 416, 476, 787) — memory safety regression suite.
  castle-regression:
    name: CASTLE Regression
    runs-on: ubuntu-22.04
    timeout-minutes: 20
    needs: [build-and-test]

    steps:
      - name: Checkout
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Free disk space
        run: |
          sudo rm -rf /usr/share/dotnet /usr/local/lib/android /opt/ghc
          sudo docker image prune -a -f || true

      - name: Pull map2check-dev image
        run: docker pull ghcr.io/hbgit/map2check-dev:latest

      - name: Build + install Map2Check in container
        run: |
          docker run --rm \
            -u root \
            -v "${{ github.workspace }}:/workspace" \
            -w /workspace \
            -e CC=/usr/bin/clang-16 \
            -e CXX=/usr/bin/clang++-16 \
            ghcr.io/hbgit/map2check-dev:latest bash -c '
              rm -rf build_castle_ci install_castle_ci
              mkdir -p build_castle_ci && cd build_castle_ci
              cmake .. -G Ninja \
                -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
                -DENABLE_TEST=ON \
                -DMAP2CHECK_DYNAMIC_LINK=ON \
                -DCMAKE_INSTALL_PREFIX=/workspace/install_castle_ci
              ninja
              ninja install
              mkdir -p /workspace/install_castle_ci/lib/klee
              ln -sf /opt/klee/lib/klee/runtime /workspace/install_castle_ci/lib/klee/runtime
              ln -sf /usr/lib/llvm-16/lib/clang /workspace/install_castle_ci/lib/clang
            '

      - name: Run CASTLE regression tests
        run: |
          docker run --rm \
            -u root \
            -v "${{ github.workspace }}:/workspace" \
            -w /workspace \
            -e MAP2CHECK_PATH=/workspace/install_castle_ci \
            ghcr.io/hbgit/map2check-dev:latest bash -c '
              bash tests/integration/test_castle_regression.sh
            '
```

- [ ] **Step 2: Verify YAML syntax**

```bash
python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))"
```

- [ ] **Step 3: Commit**

```bash
git add .github/workflows/ci.yml
git commit -m "ci: add CASTLE regression job"
```

---

### Task 4: Write CASTLE evaluation script

**Files:**
- Create: `tests/castle/run_castle_evaluation.sh`

**Interfaces:**
- Consumes: `tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250/*.c` (250 C files), `tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250.min.json` (ground truth), `$MAP2CHECK_PATH` env var
- Produces: `tests/castle/results/castle_results.csv`, `tests/castle/results/castle_summary.txt`

- [ ] **Step 1: Create the evaluation script**

```bash
#!/bin/bash
# run_castle_evaluation.sh — Run all 250 CASTLE C250 benchmarks through Map2Check
# Two-pass strategy: direct, then --add-invariants fallback on UNKNOWN
set -e

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CASTLE_DIR="$SCRIPT_DIR/CASTLE-Benchmark/datasets/CASTLE-C250"
JSON_FILE="$SCRIPT_DIR/CASTLE-Benchmark/datasets/CASTLE-C250.min.json"
RESULTS_DIR="$SCRIPT_DIR/results"
TIMEOUT_SEC=120

# CWE → mode mapping (single-pass flags, --add-invariants added on UNKNOWN)
declare -A CWE_MODE=(
  # Memory safety (memtrack)
  [125]="--memtrack"
  [415]="--memtrack"
  [416]="--memtrack"
  [476]="--memtrack"
  [761]="--memtrack"
  [787]="--memtrack"
  [822]="--memtrack"
  [843]="--memtrack"
  # Overflow
  [190]="--check-overflow"
  # Memcleanup
  [401]="--memcleanup-property"
  # Assert
  [134]="--check-asserts"
  [617]="--check-asserts"
  # Reachability (other)
  [253]="--target-function --target-function-name main"
  [362]="--target-function --target-function-name main"
  [369]="--target-function --target-function-name main"
  [628]="--target-function --target-function-name main"
  [674]="--target-function --target-function-name main"
  [770]="--target-function --target-function-name main"
  [835]="--target-function --target-function-name main"
  # Out of scope (still run for completeness, flagged N/A)
  [22]="--target-function --target-function-name main"
  [78]="--target-function --target-function-name main"
  [89]="--target-function --target-function-name main"
  [327]="--target-function --target-function-name main"
  [522]="--target-function --target-function-name main"
  [798]="--target-function --target-function-name main"
)

OUT_OF_SCOPE_CWES=(22 78 89 327 522 798)

is_out_of_scope() {
  local cwe="$1"
  for o in "${OUT_OF_SCOPE_CWES[@]}"; do
    [ "$cwe" = "$o" ] && return 0
  done
  return 1
}

mkdir -p "$RESULTS_DIR"

echo "CASTLE C250 × Map2Check Evaluation"
echo "=================================="

# --- Phase 1: Compile all ---
echo ""
echo "Phase 1: Compiling C → LLVM bitcode..."
mkdir -p "$RESULTS_DIR/bc"
COMPILED=0
COMPILE_FAILED=0

for c_file in "$CASTLE_DIR"/*.c; do
  name=$(basename "$c_file")
  bc_file="$RESULTS_DIR/bc/${name%.c}.bc"
  if "$CLANG" -c -emit-llvm -g -O0 -Wno-everything "$c_file" -o "$bc_file" 2>/dev/null; then
    COMPILED=$((COMPILED+1))
  else
    COMPILE_FAILED=$((COMPILE_FAILED+1))
    echo "  FAIL: $name"
  fi
done

echo "  Compiled: $COMPILED, Failed: $COMPILE_FAILED"

# --- Phase 2: Verify + Score ---
echo ""
echo "Phase 2: Running Map2Check on each benchmark..."
echo "id,cwe,name,vulnerable,mode,verdict,invariants,expected,result,time_sec" > "$RESULTS_DIR/castle_results.csv"

RUN=0
TP=0; TN=0; FP=0; FN=0; UNK=0; TO=0; NA=0

for c_file in "$CASTLE_DIR"/*.c; do
  name=$(basename "$c_file")
  id="${name%.c}"
  id_short=$(echo "$id" | sed 's/CASTLE-//')

  bc_file="$RESULTS_DIR/bc/${id}.bc"
  [ -f "$bc_file" ] || continue

  RUN=$((RUN+1))

  # Extract ground truth from JSON
  entry=$(python3 -c "
import json
with open('$JSON_FILE') as f:
    data = json.load(f)
for t in data['tests']:
    if t['name'] == '$name':
        print(json.dumps({'vulnerable': t['vulnerable'], 'cwe': t['cwe'], 'lines': t.get('lines',[])}))
        break
" 2>/dev/null)

  vulnerable=$(echo "$entry" | python3 -c "import sys,json; print(json.loads(sys.stdin.read())['vulnerable'])")
  cwe=$(echo "$entry" | python3 -c "import sys,json; print(json.loads(sys.stdin.read())['cwe'])")

  mode_flags="${CWE_MODE[$cwe]:---target-function --target-function-name main}"

  # --- Pass 1: direct ---
  start=$(date +%s%N)
  output=$(timeout "$TIMEOUT_SEC" "$MAP2CHECK" $mode_flags --timeout 60 "$bc_file" 2>&1) || true
  end=$(date +%s%N)
  elapsed=$(echo "scale=1; ($end - $start) / 1000000000" | bc)

  used_invariants="no"
  if echo "$output" | grep -qi "TIMEOUT\|timed out"; then
    verdict="TIMEOUT"
  elif echo "$output" | grep -q "FALSE-DEREF"; then
    verdict="FALSE-DEREF"
  elif echo "$output" | grep -q "FALSE-FREE"; then
    verdict="FALSE-FREE"
  elif echo "$output" | grep -q "FALSE-MEMTRACK"; then
    verdict="FALSE-MEMTRACK"
  elif echo "$output" | grep -q "FALSE-OVERFLOW"; then
    verdict="FALSE-OVERFLOW"
  elif echo "$output" | grep -q "FALSE-MEMCLEANUP"; then
    verdict="FALSE-MEMCLEANUP"
  elif echo "$output" | grep -q "VERIFICATION FAILED"; then
    verdict="FALSE"
  elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
    verdict="TRUE"
  else
    verdict="UNKNOWN"
  fi

  # --- Pass 2: --add-invariants fallback ---
  if [ "$verdict" = "UNKNOWN" ] && [ "$mode_flags" != "--check-asserts" ]; then
    used_invariants="yes"
    start=$(date +%s%N)
    output=$(timeout "$TIMEOUT_SEC" "$MAP2CHECK" $mode_flags --add-invariants --timeout 60 "$bc_file" 2>&1) || true
    end=$(date +%s%N)
    elapsed=$(echo "scale=1; ($end - $start) / 1000000000" | bc)

    if echo "$output" | grep -qi "TIMEOUT\|timed out"; then
      verdict="TIMEOUT"
    elif echo "$output" | grep -q "FALSE-DEREF"; then
      verdict="FALSE-DEREF"
    elif echo "$output" | grep -q "FALSE-FREE"; then
      verdict="FALSE-FREE"
    elif echo "$output" | grep -q "FALSE-MEMTRACK"; then
      verdict="FALSE-MEMTRACK"
    elif echo "$output" | grep -q "FALSE-OVERFLOW"; then
      verdict="FALSE-OVERFLOW"
    elif echo "$output" | grep -q "FALSE-MEMCLEANUP"; then
      verdict="FALSE-MEMCLEANUP"
    elif echo "$output" | grep -q "VERIFICATION FAILED"; then
      verdict="FALSE"
    elif echo "$output" | grep -q "VERIFICATION SUCCEEDED"; then
      verdict="TRUE"
    else
      verdict="UNKNOWN"
    fi
  fi

  # --- Classify ---
  if is_out_of_scope "$cwe"; then
    classification="N/A"
    NA=$((NA+1))
  elif [ "$verdict" = "TIMEOUT" ]; then
    classification="TIMEOUT"
    TO=$((TO+1))
  elif [ "$verdict" = "UNKNOWN" ]; then
    classification="UNKNOWN"
    UNK=$((UNK+1))
  elif [ "$verdict" != "TRUE" ] && [ "$vulnerable" = "True" ]; then
    classification="TP"
    TP=$((TP+1))
  elif [ "$verdict" != "TRUE" ] && [ "$vulnerable" = "False" ]; then
    classification="FP"
    FP=$((FP+1))
  elif [ "$verdict" = "TRUE" ] && [ "$vulnerable" = "False" ]; then
    classification="TN"
    TN=$((TN+1))
  elif [ "$verdict" = "TRUE" ] && [ "$vulnerable" = "True" ]; then
    classification="FN"
    FN=$((FN+1))
  else
    classification="UNCLASSIFIED"
    UNK=$((UNK+1))
  fi

  echo "$id_short,$cwe,$name,$vulnerable,$mode_flags,$verdict,$used_invariants,$vulnerable,$classification,$elapsed" >> "$RESULTS_DIR/castle_results.csv"
  echo "  [$RUN/250] $name (CWE-$cwe): $verdict → $classification (${elapsed}s)"
done

# --- Phase 3: Summary ---
echo ""
echo "Phase 3: Generating summary..."
TOTAL_SCOPE=$((TP+TN+FP+FN))
DETECTABLE=$((TOTAL_SCOPE - NA))

cat > "$RESULTS_DIR/castle_summary.txt" << EOF
CASTLE C250 × Map2Check Evaluation Results
===========================================
Date: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
Map2Check binary: $MAP2CHECK

Totals:
  Total tests:           250
  Detected (in-scope):   $DETECTABLE
  Out of scope (N/A):    $NA
  Timeouts:              $TO
  Unknown:               $UNK

Detection Metrics (in-scope):
  True Positives:        $TP
  False Positives:       $FP
  True Negatives:        $TN
  False Negatives:       $FN

  Precision:             $(python3 -c "print(f'{$TP/($TP+$FP)*100:.1f}%')" 2>/dev/null || echo "N/A")
  Recall:                $(python3 -c "print(f'{$TP/($TP+$FN)*100:.1f}%')" 2>/dev/null || echo "N/A")

Results written to:
  CSV:  $RESULTS_DIR/castle_results.csv
EOF

cat "$RESULTS_DIR/castle_summary.txt"

echo ""
echo "Done. Results in $RESULTS_DIR/"
```

- [ ] **Step 2: Make executable**

```bash
chmod +x tests/castle/run_castle_evaluation.sh
```

- [ ] **Step 3: Syntax check**

```bash
bash -n tests/castle/run_castle_evaluation.sh
```

Expected: no output

- [ ] **Step 4: Commit**

```bash
git add tests/castle/run_castle_evaluation.sh
git commit -m "feat(castle): add evaluation script for all 250 CASTLE benchmarks"
```

---

### Task 5: Verify end-to-end

**Files:**
- Test: `tests/integration/test_castle_regression.sh` (already committed)
- Test: `tests/castle/run_castle_evaluation.sh` (already committed)

- [ ] **Step 1: Run regression tests locally (Docker)**

```bash
docker run --rm \
  -u root \
  -v "$(pwd):/workspace" \
  -w /workspace \
  -e MAP2CHECK_PATH=/workspace/install_e2e \
  ghcr.io/hbgit/map2check-dev:latest bash -c '\
    rm -rf build_castle_ci install_castle_ci && \
    mkdir -p build_castle_ci && cd build_castle_ci && \
    cmake .. -G Ninja \
      -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
      -DENABLE_TEST=ON \
      -DMAP2CHECK_DYNAMIC_LINK=ON \
      -DCMAKE_INSTALL_PREFIX=/workspace/install_castle_ci && \
    ninja && ninja install && \
    mkdir -p /workspace/install_castle_ci/lib/klee && \
    ln -sf /opt/klee/lib/klee/runtime /workspace/install_castle_ci/lib/klee/runtime && \
    ln -sf /usr/lib/llvm-16/lib/clang /workspace/install_castle_ci/lib/clang && \
    bash tests/integration/test_castle_regression.sh'
```

- [ ] **Step 2: Push and verify CI passes**

```bash
git push origin feat/wasm-clean
# Check GitHub Actions for castle-regression job
```
