# `--add-invariants` Revival Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Stop `--add-invariants` from being silently ignored, then restore the
capability by migrating from the abandoned crab-llvm fork to Clam `dev16`, with a CI
test that fails today and passes once the migration lands.

**Architecture:** Two phases with a measurement gate between them. Phase A makes the
missing capability loud on every input path and repairs the three call sites that pass
the flag today. Phase B builds Clam `dev16` into the development image behind
`-DENABLE_CLAM=ON`, rewrites the invocation for Clam's renamed options, and promotes
the capability only after a differential run proves no detection was lost. The
consumption side needs no work: `NonDetPass.cpp:95` already maps `verifier.assume` to
`map2check_crab_assume`, and Clam still emits that symbol.

**Tech Stack:** C++17, CMake, Docker (`Dockerfile.dev`), bash, Clam `dev16`
(Apache-2.0), GitHub Actions.

**Spec:** [docs/reports/2026-08-16-crabllvm-review.md](../../reports/2026-08-16-crabllvm-review.md)
— the systematic review that establishes the dependency's real state, the flag
renames, and the soundness hazard that decides the default.

## Global Constraints

- **Clam `dev16`**, `https://github.com/seahorn/clam.git`, Apache-2.0. Last commit on
  that branch `302bf49` (2026-08-07). `CMakeLists.txt` pins `CLAM_LLVM_VERSION 16`.
- **The one renamed flag:** `--crab-add-invariants=block-entry` becomes
  `--crab-opt=add-invariants --crab-opt-invariants-loc=block-entry`.
  `--crab-track=num`, `--crab-inter`, `--crab-promote-assume`, `--llvm-pp-loops`,
  `-o`, `-m`, `-g` are unchanged.
- **`verifier.assume` is preserved** in Clam (`lib/Clam/Optimizer/Optimizer.cc:667`).
  `modules/backend/pass/NonDetPass.cpp:95` must not change.
- **Extra system packages:** `libmpfr-dev`, `libflint-dev`. `libgmp-dev` and
  `libboost-all-dev` are already in `Dockerfile.dev`.
- **Off by default.** An unsound invariant produces a wrong TRUE, not an error: under
  KLEE `klee_assume` prunes a reachable state, and under LibFuzzer `nondet_assume`
  calls `pthread_exit` and the execution dies silently. Promotion to default requires
  the differential evidence in Task 7.
- **The CI image lags the Dockerfile.** CI jobs `docker pull
  ghcr.io/hbgit/map2check-dev:latest`, which `docker-publish.yml` republishes on push
  to `develop`. Clam will not be in the pulled image during this work. The capability
  test must therefore *detect* which state it is in rather than assume one — that
  detection is what turns it from a regression guard into a progress detector.
- C++ follows Google style; run `./check_code_style.py -p`.

---

## PHASE A — stop the silence

### Task 1: Fail loudly when invariants are requested but unavailable

**Files:**
- Modify: `modules/frontend/map2check.cpp` (exit code, capability check, dispatch)
- Modify: `modules/frontend/utils/tools.hpp` (the Clam binary path)
- Test: `tests/integration/test_add_invariants.sh`
- Modify: `.github/workflows/ci.yml`

**Interfaces:**
- Consumes: nothing.
- Produces:
  - exit code `3` (`ERROR_UNAVAILABLE_CAPABILITY`) when `--add-invariants` is passed
    and no invariant generator is installed;
  - `bool Map2Check::invariantGeneratorAvailable()` in `map2check.cpp`'s anonymous
    namespace — Task 5 replaces its body, and Task 6's test keys off its effect.

The current guard is in `map2check.cpp:227-245`, inside `if (!is_llvmir_in)`. With
`.c` input it warns; with `.bc` input — how every baseline run and the CASTLE harness
invoke the tool — the whole block is skipped and not even the warning appears. A flag
that is accepted, ignored, and leaves no trace is a silently wrong configuration, and
on a competition cluster a warning on stderr disappears into the BenchExec log.

- [ ] **Step 1: Write the failing test**

```bash
#!/bin/bash
# test_add_invariants.sh -- the --add-invariants contract.
#
# This file has two jobs, and the second is unusual.
#
# The first is the ordinary one: --add-invariants must never be accepted and
# then ignored without a trace. That was the state issue #54 describes, and it
# held on the .bc input path -- which is how the entire v5 baseline and the
# CASTLE harness invoke the tool.
#
# The second is to detect PROGRESS. The invariant generator is absent today, so
# the "unavailable" assertions below are what passes. Once Clam dev16 is built
# into the image, the same run must instead succeed and inject verifier.assume
# calls. The script probes which state it is in and asserts the contract for
# that state, reporting the state loudly either way. A green run therefore
# means "the contract holds", and the printed CAPABILITY line says which
# contract that was -- so the day the capability comes back is visible in the
# CI log rather than inferred.

set -u

MAP2CHECK="${MAP2CHECK_PATH:-/workspace/install_e2e}/bin/map2check"

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

cat > "$WORK/loop.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  int i = 0;
  int sum = 0;
  if (n < 0 || n > 100) return 0;
  while (i < n) { sum += i; i++; }
  if (sum < 0) { reach_error(); }
  return 0;
}
EOF

# The .bc path is the one that was silent, so it is the one under test.
"${MAP2CHECK_PATH:-/workspace/install_e2e}/bin/../bin/map2check" --version >/dev/null 2>&1
clang-16 -c -emit-llvm -g -o "$WORK/loop.bc" "$WORK/loop.c" 2>/dev/null

echo "============================================================"
echo "--add-invariants contract"
echo "============================================================"

run_flag() {
  local input="$1" rc=0
  ( cd "$WORK" && timeout -k 10 180 "$MAP2CHECK" --target-function \
      --target-function-name reach_error --add-invariants \
      --nondet-generator klee --timeout 60 "$input" ) > "$WORK/out.txt" 2>&1 || rc=$?
  echo "$rc"
}

rc_bc=$(run_flag loop.bc)
out_bc=$(cat "$WORK/out.txt")

if [ "$rc_bc" -eq 3 ]; then
  CAPABILITY="ABSENT"
elif [ "$rc_bc" -eq 0 ]; then
  CAPABILITY="PRESENT"
else
  CAPABILITY="BROKEN"
fi
echo "  CAPABILITY: invariant generator is $CAPABILITY (exit $rc_bc)"

case "$CAPABILITY" in
  ABSENT)
    # The contract while Clam is not built.
    echo "$out_bc" | grep -qi "add-invariants" \
      && ok "the failure names the flag that caused it" \
      || fail "diagnostic" "exit 3 with no mention of --add-invariants"

    echo "$out_bc" | grep -qiE "not built|unavailable|not available" \
      && ok "the failure says the capability is unavailable" \
      || fail "diagnostic" "no explanation of why it failed"

    # The silence bug itself: .c input already warned, .bc did not. Both must
    # behave identically now.
    rc_c=$(run_flag loop.c)
    [ "$rc_c" -eq 3 ] \
      && ok ".c input fails the same way .bc does (issue #54)" \
      || fail "input path parity" ".bc exited 3 but .c exited $rc_c"

    # Without the flag, nothing changes.
    rc_plain=0
    ( cd "$WORK" && timeout -k 10 180 "$MAP2CHECK" --target-function \
        --target-function-name reach_error --nondet-generator klee \
        --timeout 60 loop.bc ) > "$WORK/plain.txt" 2>&1 || rc_plain=$?
    [ "$rc_plain" -ne 3 ] \
      && ok "runs without --add-invariants are unaffected" \
      || fail "collateral" "a run without the flag also exited 3"
    ;;

  PRESENT)
    # The contract once Clam dev16 is in place. Reaching this branch is the
    # progress signal: it means the migration landed.
    ok "--add-invariants completes successfully"

    # Succeeding is not enough -- it must actually inject invariants, or the
    # flag is a no-op wearing a success exit code.
    bc=$(find "$WORK" -name '*-compiled.bc' -o -name '*-crab.bc' | head -1)
    if [ -n "$bc" ] && llvm-dis-16 -o - "$bc" 2>/dev/null | grep -q 'verifier.assume'; then
      ok "the produced bitcode contains verifier.assume calls"
    else
      fail "invariant injection" "exit 0 but no verifier.assume in the bitcode"
    fi
    ;;

  BROKEN)
    fail "capability probe" "exit $rc_bc is neither 3 (absent) nor 0 (present)"
    echo "$out_bc" | tail -20
    ;;
esac

echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed  [capability: $CAPABILITY]"
echo "============================================================"
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
```

- [ ] **Step 2: Run it and verify it fails**

```bash
MAP2CHECK_PATH=$PWD/release bash tests/integration/test_add_invariants.sh
```
Expected: `CAPABILITY: invariant generator is BROKEN (exit 0)` — today the flag is
accepted and ignored, so the run succeeds and the probe rejects it.

- [ ] **Step 3: Add the exit code and the capability check**

In `modules/frontend/map2check.cpp`, next to the other codes (line 44):

```cpp
const size_t ERROR_UNAVAILABLE_CAPABILITY = 3;
```

In the same anonymous namespace, add:

```cpp
/** Whether an invariant generator is installed alongside this build.
 *
 * Clam (formerly crab-llvm) is an external tool invoked as a subprocess, so
 * the answer is a filesystem question and not a compile-time one: a binary
 * built with -DENABLE_CLAM=ON still has to find the installed script. */
bool invariantGeneratorAvailable() {
  const char* m2cPath = getenv("MAP2CHECK_PATH");
  if (m2cPath == nullptr) return false;
  return fs::exists(std::string(m2cPath) + "/bin/crabllvm/bin/crabllvm.py");
}
```

- [ ] **Step 4: Replace the dispatch so both input paths are covered**

Replace the whole `if (!is_llvmir_in) { ... } else { ... }` block at
`modules/frontend/map2check.cpp:226-247` with:

```cpp
  // The capability check comes before the input-kind branch on purpose. It used
  // to live inside the !is_llvmir_in arm, which meant that with bitcode input
  // -- how the baselines and the CASTLE harness invoke the tool -- the flag was
  // accepted, ignored, and left no trace at all (issue #54).
  if (args.invCrabLlvm && !invariantGeneratorAvailable()) {
    Map2Check::Log::Error(
        "--add-invariants was requested but no invariant generator is "
        "installed. Crab-LLVM was renamed to Clam and this build does not "
        "ship it; rebuild with -DENABLE_CLAM=ON, or drop the flag. Refusing "
        "to continue rather than silently analysing without invariants.");
    return ERROR_UNAVAILABLE_CAPABILITY;
  }

  if (!is_llvmir_in && args.invCrabLlvm) {
    caller->compileToCrabLlvm();
  } else {
    caller->compileCFile(is_llvmir_in);
  }
```

Note that with bitcode input and the capability present, the tool still takes the
`compileCFile` path — Clam needs source, and instrumenting already-compiled bitcode
with invariants is a separate question that Task 5 addresses.

- [ ] **Step 5: Verify `Log::Error` exists and is fatal-looking**

```bash
grep -n "static void Error\|void Error" modules/frontend/utils/log.hpp
```
If there is no `Error` level, use `Map2Check::Log::Warning` with the same text and
keep the exit code — the exit code is the contract the test asserts; the log level is
presentation.

- [ ] **Step 6: Build and run the test**

```bash
cd build && ninja && ninja install && cd ..
MAP2CHECK_PATH=$PWD/release bash tests/integration/test_add_invariants.sh
```
Expected: `CAPABILITY: invariant generator is ABSENT (exit 3)` and
`Results: 4 passed, 0 failed`.

- [ ] **Step 7: Add it to CI**

In `.github/workflows/ci.yml`, in the `castle-regression` job's `Run regression tests`
step, after `bash tests/integration/test_modes.sh`:

```yaml
              bash tests/integration/test_add_invariants.sh
```

- [ ] **Step 8: Commit**

```bash
./check_code_style.py -p
git add modules/frontend/map2check.cpp tests/integration/test_add_invariants.sh \
        .github/workflows/ci.yml
git commit -m "fix(invariants): refuse --add-invariants instead of ignoring it"
```

---

### Task 2: Let the three call sites keep requesting invariants

**Files:**
- Modify: `utils/map2check-wrapper.py:73` (comment only)
- Modify: `utils/map2check-wrapper-reg-test.py:75` (comment only)
- Modify: `tests/castle/run_castle_evaluation.sh:167-180`
- Modify: `docs/reports/2026-08-12-castle-juliet-findings.md`

**Interfaces:**
- Consumes: exit code 3 from Task 1.
- Produces: no new interface. The CASTLE CSV's `used_invariants` column gains a third
  legal value, `unavailable`.

Three call sites pass `--add-invariants` today: the two BenchExec wrappers
(`map2check-wrapper.py`, `map2check-wrapper-reg-test.py`) and the CASTLE harness's
second pass. Task 1 turns the silent no-op into a hard failure, so all three start
failing until Phase B lands.

**That breakage is intended and the flag stays in all three.** They are asking for a
capability the tool is supposed to have; Clam is the repair, not deletion. Removing the
request would hide the gap and then require remembering to restore it — the same class
of silent drift that produced issue #54 in the first place. Leaving it in makes the
three call sites part of the progress signal: they fail while the capability is absent
and work the day it returns.

One exception, and it is about data integrity rather than about the flag. The CASTLE
harness's pass 2 re-runs the tool on UNKNOWN and **replaces** pass 1's verdict with the
result. If pass 2 now exits 3, the classifier records `ERROR` on top of a perfectly
good `UNKNOWN`, so a baseline run during the gap would produce a corrupted CSV rather
than a visible failure. The harness therefore learns to recognise exit 3 as
"invariants unavailable" and keeps pass 1's verdict. That is handling a documented
contract, not withdrawing the request.

Worth recording while here: because the flag was ignored on the `.bc` path, **pass 2
has been a byte-identical re-run of pass 1** — a second full timeout budget spent
reaching the same conclusion, with `used_invariants="yes"` written to the CSV as if
invariants had been applied. That column is wrong in the v5 data.

- [ ] **Step 1: Document the dependency in the BenchExec wrappers**

The flag stays. Add above line 73 of `utils/map2check-wrapper.py`:

```python
  # --add-invariants requires Clam (formerly crab-llvm), which is not in every
  # build. map2check exits 3 rather than ignoring the flag, so this wrapper
  # fails loudly until Clam ships -- deliberately. Do not drop the flag to make
  # the failure go away; the fix is to build with -DENABLE_CLAM=ON. See
  # docs/reports/2026-08-16-crabllvm-review.md.
```

Add the same comment above line 75 of `utils/map2check-wrapper-reg-test.py`.

- [ ] **Step 2: Verify both wrappers still parse and still pass the flag**

```bash
python3 -c "import ast; ast.parse(open('utils/map2check-wrapper.py').read()); \
            ast.parse(open('utils/map2check-wrapper-reg-test.py').read()); print('ok')"
grep -c "add-invariants" utils/map2check-wrapper.py
```
Expected: `ok`, and a count of 2 (the comment plus the still-present flag).

- [ ] **Step 3: Stop pass 2 from overwriting a good verdict with exit 3**

In `tests/castle/run_castle_evaluation.sh`, replace the pass 2 block
(lines 167-179) with:

```bash
  # --- Pass 2: --add-invariants fallback ---
  # The flag is requested unconditionally on purpose: this pass exists to retry
  # UNKNOWN cases with abstract-interpretation invariants, and map2check is
  # supposed to provide them. What is handled here is the documented exit code
  # for "the generator is not installed" (3), because pass 2 REPLACES pass 1's
  # verdict -- without this, a run during the Clam gap would write ERROR over a
  # perfectly good UNKNOWN and corrupt the CSV instead of failing visibly.
  #
  # Note for anyone reading v5 data: through that baseline the flag was silently
  # ignored on .bc input, so pass 2 was a byte-identical re-run of pass 1 and
  # used_invariants=yes did not mean invariants were applied.
  if [ "$verdict" = "UNKNOWN" ] && [ "$mode_flags" != "--check-asserts" ]; then
    start=$(date +%s%N)
    rc=0
    run_isolated "$raw" "$TIMEOUT_SEC" \
      "$MAP2CHECK" $mode_flags --add-invariants --timeout "$INNER_TIMEOUT" "$bc_file" || rc=$?
    end=$(date +%s%N)

    if [ "$rc" -eq 3 ]; then
      # Capability absent. Keep pass 1's verdict and its raw output; say so in
      # the CSV rather than pretending invariants were tried.
      used_invariants="unavailable"
      printf '%s' "$output" > "$raw"
    else
      used_invariants="yes"
      output=$(cat "$raw")
      elapsed=$(python3 -c "print(round(($end - $start) / 1000000000, 1))")
      verdict=$(classify_map2check_verdict "$output" "$rc" "$elapsed" "$INNER_TIMEOUT")
    fi
  fi
```

- [ ] **Step 4: Check the runner still parses and the column has three values**

```bash
bash -n tests/castle/run_castle_evaluation.sh && echo "syntax ok"
grep -n 'used_invariants=' tests/castle/run_castle_evaluation.sh
```
Expected: `syntax ok`, and `used_invariants` assigned in exactly three places — `no`,
`yes`, `unavailable`.

- [ ] **Step 5: Correct the record in the findings report**

Add to `docs/reports/2026-08-12-castle-juliet-findings.md`, in its own section:

```markdown
## Correction: the CASTLE second pass never applied invariants

The runner retried every UNKNOWN with `--add-invariants`. That flag was accepted
and ignored on the bitcode input path (issue #54), so pass 2 re-ran the identical
analysis on the identical input and could only differ through nondeterminism. Two
consequences for the v5 data:

- The `used_invariants=yes` column does not mean what it says. No run in the v5
  baseline used invariants.
- Every UNKNOWN case cost two full timeout budgets instead of one, which inflates
  the wall-clock totals for the affected rows without changing any verdict.

No verdict in the published table changes: a second identical run cannot turn an
UNKNOWN into a decision except by chance, and the verdicts were taken from the
last run either way. What changes is the interpretation of the column and of the
run times.
```

- [ ] **Step 6: Commit**

```bash
git add utils/map2check-wrapper.py utils/map2check-wrapper-reg-test.py \
        tests/castle/run_castle_evaluation.sh \
        docs/reports/2026-08-12-castle-juliet-findings.md
git commit -m "fix(castle): keep pass 1's verdict when invariants are unavailable"
```

---

## PHASE B — bring the capability back

### Task 3: Build Clam dev16 into the development image

**Files:**
- Modify: `Dockerfile.dev`
- Modify: `CLAUDE.md`

**Interfaces:**
- Consumes: nothing.
- Produces: `/opt/clam/bin/clam.py` inside the image, and the environment variable
  `CLAM_DIR=/opt/clam`. Tasks 4 and 5 depend on that path.

Clam is added to the image rather than to `FindCrabLlvm.cmake`'s `ExternalProject_Add`
because it is a standalone tool invoked as a subprocess, not a library the frontend
links. Building it per CMake configure would add tens of minutes to every clean build
of Map2Check, for a capability that is off by default.

- [ ] **Step 1: Add the build stage**

Insert into `Dockerfile.dev` after section 7 (LibFuzzer) and before section 8 (WABT),
so the WABT and WASI verification steps stay at the end:

```dockerfile
# ============================================================
# 7b. Clam (formerly crab-llvm) — abstract-interpretation invariants
# ============================================================
# Clam rewrites the bitcode, inserting verifier.assume(inv) calls that
# NonDetPass maps to map2check_crab_assume and the runtime forwards to
# klee_assume. Branch dev16 targets LLVM 16, which is this image's LLVM.
#
# Built here rather than as a CMake ExternalProject: it is a subprocess tool,
# not a linked library, and building it per configure would add tens of minutes
# to every clean build for a capability that is off by default.
RUN apt-get update && apt-get install -y \
    libmpfr-dev \
    libflint-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 -b dev16 https://github.com/seahorn/clam.git /tmp/clam && \
    cd /tmp/clam && mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCLAM_LLVM_VERSION=16 \
             -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
             -DCMAKE_INSTALL_PREFIX=/opt/clam && \
    cmake --build . --target extra && cmake .. && \
    cmake --build . --target crab && cmake .. && \
    cmake --build . -j$(nproc) && \
    cmake --build . --target install && \
    rm -rf /tmp/clam

ENV CLAM_DIR=/opt/clam
```

The three-step `extra` / `crab` / full build mirrors the sequence the old
`build_crabllvm.py` used and that Clam's README still documents: the dependency targets
must be built and the cache re-read before the main build can see them.

- [ ] **Step 2: Build the image and verify Clam runs**

```bash
docker build -t map2check-dev -f Dockerfile.dev . 2>&1 | tail -20
docker run --rm map2check-dev bash -c '/opt/clam/bin/clam.py --help | head -5'
```
Expected: Clam's usage text. If the build fails on a missing domain library, drop the
optional domains — `USE_LDD` and `USE_APRON` were on in the old script and are not
required for the interval domain that `--crab-track=num` uses.

- [ ] **Step 3: Confirm the renamed flag combination is accepted**

```bash
docker run --rm map2check-dev bash -c '
  printf "int main(){int i=0;while(i<10){i++;}return i;}" > /tmp/t.c
  /opt/clam/bin/clam.py -o /tmp/t.bc --crab-opt=add-invariants \
    --crab-opt-invariants-loc=block-entry --crab-track=num --crab-inter \
    --crab-promote-assume --llvm-pp-loops /tmp/t.c
  llvm-dis-16 -o - /tmp/t.bc | grep -c "verifier.assume"'
```
Expected: a non-zero count. A count of zero means the invariants are being computed but
not injected, and the `--crab-opt-invariants-loc` value is the first thing to check.

Record the exact working command line; Task 5 hardcodes it.

- [ ] **Step 4: Document it and commit**

Add to `CLAUDE.md`, under the build-environment description:

```markdown
`Dockerfile.dev` also builds **Clam `dev16`** (the renamed crab-llvm) into
`/opt/clam`. It supplies the abstract-interpretation invariants behind
`--add-invariants`, which is off unless Map2Check is configured with
`-DENABLE_CLAM=ON`. See [the dependency review](docs/reports/2026-08-16-crabllvm-review.md)
for why it is opt-in.
```

```bash
git add Dockerfile.dev CLAUDE.md
git commit -m "build(docker): build Clam dev16 for LLVM 16 invariants"
```

---

### Task 4: The `ENABLE_CLAM` build option

**Files:**
- Modify: `CMakeLists.txt`
- Modify: `modules/frontend/utils/tools.hpp`
- Delete: `cmake/FindCrabLlvm.cmake`
- Delete: `utils/build_crabllvm.py`

**Interfaces:**
- Consumes: `/opt/clam/bin/clam.py` from Task 3.
- Produces: the CMake option `ENABLE_CLAM` (default `OFF`), the compile definition
  `MAP2CHECK_ENABLE_CLAM`, and `Map2Check::clamBinary` replacing
  `Map2Check::crabBinary`. Task 5 uses both.

`FindCrabLlvm.cmake` clones `seahorn/crab-llvm` with no branch pinned and is not
referenced from any `CMakeLists.txt`; `build_crabllvm.py` hardcodes
`/llvm/release/llvm600` and `g++-5`. Both describe an environment that has not existed
since LLVM 6, and leaving them in place invites someone to run them.

- [ ] **Step 1: Add the option**

In the top-level `CMakeLists.txt`, next to the other options (after line 10):

```cmake
option(ENABLE_CLAM "Enable --add-invariants via Clam (needs Clam dev16 installed)" OFF)
```

And where compile definitions are set:

```cmake
if(ENABLE_CLAM)
  # A build-time switch and a runtime path check are both needed: this says the
  # build intends to support invariants, the runtime check says the installed
  # tree actually has the tool. Neither implies the other.
  add_compile_definitions(MAP2CHECK_ENABLE_CLAM)
  message(STATUS "Clam invariants: ENABLED")
else()
  message(STATUS "Clam invariants: disabled (-DENABLE_CLAM=ON to enable)")
endif()
```

- [ ] **Step 2: Point the binary constant at Clam**

In `modules/frontend/utils/tools.hpp`, replace the `crabBinary` constant (lines 36-38):

```cpp
/** Path to the Clam driver (formerly crab-llvm). Clam is installed into the
 * image at /opt/clam and is overridable with CLAM_DIR for local builds. */
constexpr char const* clamBinary = "${CLAM_DIR}/bin/clam.py";
```

- [ ] **Step 3: Remove the dead build machinery**

```bash
git rm cmake/FindCrabLlvm.cmake utils/build_crabllvm.py
grep -rn "FindCrabLlvm\|build_crabllvm\|crabBinary" --include=*.txt --include=*.cmake \
     --include=*.cpp --include=*.hpp --include=*.py --include=*.sh . \
     --exclude-dir=.git
```
Expected: the only remaining matches are in `caller.cpp`, which Task 5 rewrites.

- [ ] **Step 4: Confirm both configurations still configure**

```bash
cd build && cmake .. -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
  -DSKIP_LIB_FUZZER=ON -DSKIP_KLEE=ON 2>&1 | grep "Clam invariants"
cmake .. -DENABLE_CLAM=ON 2>&1 | grep "Clam invariants"
```
Expected: `disabled` then `ENABLED`.

- [ ] **Step 5: Commit**

```bash
git add CMakeLists.txt modules/frontend/utils/tools.hpp
git commit -m "build: add ENABLE_CLAM and retire the LLVM 6 crab machinery"
```

---

### Task 5: Invoke Clam with its current option names

**Files:**
- Modify: `modules/frontend/caller.cpp:574-660` (`compileToCrabLlvm`)
- Modify: `modules/frontend/caller.hpp:75` (rename the method)
- Modify: `modules/frontend/map2check.cpp` (capability check and call site)

**Interfaces:**
- Consumes: `Map2Check::clamBinary` and `MAP2CHECK_ENABLE_CLAM` from Task 4.
- Produces: `void Caller::compileWithClam()` replacing `compileToCrabLlvm()`, and an
  `invariantGeneratorAvailable()` that checks the Clam path. Task 6 asserts the effect.

- [ ] **Step 1: Rewrite the invocation**

Replace the body of `compileToCrabLlvm` in `modules/frontend/caller.cpp` and rename it
to `compileWithClam`. The command becomes:

```cpp
  // Clam renamed exactly one of the options this call used to pass:
  //   crab-llvm:  --crab-add-invariants=block-entry
  //   Clam:       --crab-opt=add-invariants --crab-opt-invariants-loc=block-entry
  // Everything else -- track, inter, promote-assume, llvm-pp-loops -- kept its
  // name. See docs/reports/2026-08-16-crabllvm-review.md.
  command << Map2Check::clamBinary << " -o " << compiledFile
          << " -m 64 -g"
          << " --crab-track=num"
          << " --crab-inter"
          << " --crab-promote-assume"
          << " --llvm-pp-loops"
          << " --crab-opt=add-invariants"
          << " --crab-opt-invariants-loc=block-entry"
          << " " << programHash << "-preprocessed.c";
```

Keep the existing `LD_LIBRARY_PATH` export, retargeted from
`${MAP2CHECK_PATH}/bin/crabllvm/lib` to `${CLAM_DIR}/lib`.

Substitute the flag list with whatever Task 3 Step 3 actually validated — that step
exists precisely so this one is transcription and not guesswork.

- [ ] **Step 2: Update the header**

In `modules/frontend/caller.hpp`, line 75:

```cpp
  /** Compiles the input through Clam so the bitcode carries
   * verifier.assume(invariant) calls. Requires Clam dev16 installed; the caller
   * must have checked availability first. */
  void compileWithClam();
```

- [ ] **Step 3: Point the capability check at Clam**

In `modules/frontend/map2check.cpp`, replace the body of
`invariantGeneratorAvailable()`:

```cpp
bool invariantGeneratorAvailable() {
#ifndef MAP2CHECK_ENABLE_CLAM
  return false;
#else
  const char* clamDir = getenv("CLAM_DIR");
  std::string path =
      (clamDir ? std::string(clamDir) : std::string("/opt/clam")) +
      "/bin/clam.py";
  return fs::exists(path);
#endif
}
```

and change the dispatch call from `caller->compileToCrabLlvm();` to
`caller->compileWithClam();`.

- [ ] **Step 4: Build both ways and confirm the check flips**

```bash
docker run --rm -v "$PWD:/workspace" -w /workspace -u root map2check-dev bash -c '
  rm -rf build_clam && mkdir build_clam && cd build_clam
  cmake .. -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
    -DENABLE_CLAM=ON -DMAP2CHECK_DYNAMIC_LINK=ON \
    -DCMAKE_INSTALL_PREFIX=/workspace/install_clam
  ninja && ninja install'
```
Then run the Task 1 test against that install and expect
`CAPABILITY: invariant generator is PRESENT`.

- [ ] **Step 5: Style-check and commit**

```bash
./check_code_style.py -p
git add modules/frontend/caller.cpp modules/frontend/caller.hpp \
        modules/frontend/map2check.cpp
git commit -m "feat(invariants): invoke Clam dev16 with its current option names"
```

---

### Task 6: Prove the capability came back, in CI

**Files:**
- Modify: `.github/workflows/ci.yml`

**Interfaces:**
- Consumes: `tests/integration/test_add_invariants.sh` (Task 1), the `ENABLE_CLAM`
  build (Task 4), and the Clam image (Task 3).
- Produces: a CI job named `Invariants (Clam)`.

The Task 1 test already asserts both contracts and prints which one it checked. What is
missing is a CI job that builds *with* `-DENABLE_CLAM=ON`, so the `PRESENT` branch is
actually exercised. Until the published image carries Clam, that job reports `ABSENT`
and stays green; the day the image is republished it flips to `PRESENT` and starts
asserting invariant injection. That transition, visible in the CI log, is the proof the
capability returned.

- [ ] **Step 1: Add the job**

```yaml
  # ===========================================================
  # Job: Invariants — the --add-invariants contract
  # ===========================================================
  # Runs the same test in whichever state the image is in. While the published
  # image has no Clam, the test asserts that the flag fails loudly (issue #54).
  # Once the image carries Clam, the very same test asserts that the flag
  # succeeds AND injects verifier.assume. The CAPABILITY line in the log says
  # which contract was checked, so the migration landing is visible rather than
  # inferred.
  invariants-clam:
    name: Invariants (Clam)
    runs-on: ubuntu-22.04
    timeout-minutes: 25
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

      - name: Build with -DENABLE_CLAM=ON
        run: |
          docker run --rm \
            -u root \
            -v "${{ github.workspace }}:/workspace" \
            -w /workspace \
            -e CC=/usr/bin/clang-16 \
            -e CXX=/usr/bin/clang++-16 \
            ghcr.io/hbgit/map2check-dev:latest bash -c '
              rm -rf build_clam_ci install_clam_ci
              mkdir -p build_clam_ci && cd build_clam_ci
              cmake .. -G Ninja \
                -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
                -DENABLE_CLAM=ON \
                -DMAP2CHECK_DYNAMIC_LINK=ON \
                -DCMAKE_INSTALL_PREFIX=/workspace/install_clam_ci
              ninja
              ninja install
              mkdir -p /workspace/install_clam_ci/lib/klee
              ln -sf /opt/klee/lib/klee/runtime /workspace/install_clam_ci/lib/klee/runtime
              ln -sf /usr/lib/llvm-16/lib/clang /workspace/install_clam_ci/lib/clang
            '

      - name: Assert the --add-invariants contract
        run: |
          docker run --rm \
            -u root \
            -v "${{ github.workspace }}:/workspace" \
            -w /workspace \
            -e MAP2CHECK_PATH=/workspace/install_clam_ci \
            ghcr.io/hbgit/map2check-dev:latest bash -c '
              bash tests/integration/test_add_invariants.sh
            '
```

- [ ] **Step 2: Add the build directories to .gitignore**

Append to the evaluation-artifacts block in `.gitignore`:

```
build_clam_ci/
install_clam_ci/
build_clam/
install_clam/
```

- [ ] **Step 3: Verify the workflow parses and commit**

```bash
python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml')); print('ok')"
git add .github/workflows/ci.yml .gitignore
git commit -m "ci: assert the --add-invariants contract in both capability states"
```

---

### Task 7: Differential run before promoting the default

**Files:**
- Create: `tests/integration/test_invariants_no_regression.sh`
- Create: `docs/reports/2026-XX-XX-invariants-differential.md` (date it on the day it
  runs)

**Interfaces:**
- Consumes: a Clam-enabled install from Task 5.
- Produces: the evidence that decides whether `--add-invariants` may ever become a
  default. Nothing consumes it in code.

An unsound invariant does not raise an error; it produces a wrong TRUE. Under KLEE
`klee_assume` prunes a reachable state; under LibFuzzer `nondet_assume` calls
`pthread_exit` and the execution vanishes. With 35% of baseline cases taking input
through unresolved externals, how Clam models those decides soundness — and the failure
is invisible in a single run. Only a comparison can see it.

- [ ] **Step 1: Write the differential runner**

```bash
#!/bin/bash
# test_invariants_no_regression.sh -- every case detected WITHOUT invariants
# must still be detected WITH them.
#
# The asymmetry is deliberate. Gaining detections is the hoped-for outcome and
# is reported, not asserted. LOSING one is a hard failure, because a lost
# detection is the signature of an unsound invariant: klee_assume pruned a
# reachable state, or nondet_assume killed a fuzzer execution. Both look like
# "the program is safe", which is the worst way for this to fail.

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CORPUS="${CORPUS:-$SCRIPT_DIR/../castle/CASTLE-Benchmark}"
MAP2CHECK="${MAP2CHECK_PATH:-/workspace/install_clam_ci}/bin/map2check"
BUDGET="${BUDGET:-60}"

. "$SCRIPT_DIR/../lib/verdict_classifier.sh"
. "$SCRIPT_DIR/../lib/isolated_run.sh"

lost=0
gained=0
same=0

run_one() {  # <file> <extra flags...>
  local f="$1"; shift
  local raw rc=0
  raw=$(mktemp)
  run_isolated "$raw" "$((BUDGET * 2))" \
    "$MAP2CHECK" --memtrack --timeout "$BUDGET" "$@" "$f" || rc=$?
  classify_map2check_verdict "$(cat "$raw")" "$rc" 0 "$BUDGET"
  rm -f "$raw"
}

echo "============================================================"
echo "Invariants differential (budget ${BUDGET}s per run)"
echo "============================================================"

while IFS= read -r f; do
  without=$(run_one "$f")
  with=$(run_one "$f" --add-invariants)

  case "$without:$with" in
    FALSE*:FALSE*|TRUE:TRUE|UNKNOWN:UNKNOWN|TIMEOUT:TIMEOUT)
      same=$((same+1)) ;;
    FALSE*:*)
      echo "  LOST     $(basename "$f"): $without -> $with"
      lost=$((lost+1)) ;;
    *:FALSE*)
      echo "  GAINED   $(basename "$f"): $without -> $with"
      gained=$((gained+1)) ;;
    *)
      echo "  CHANGED  $(basename "$f"): $without -> $with"
      same=$((same+1)) ;;
  esac
done < <(find "$CORPUS" -name '*.c' | sort | head -"${SAMPLE:-40}")

echo "============================================================"
echo "unchanged=$same gained=$gained lost=$lost"
echo "============================================================"

if [ "$lost" -gt 0 ]; then
  echo "FAIL: $lost detection(s) lost. Invariants must stay off by default."
  exit 1
fi
echo "PASS: no detection lost; $gained gained."
exit 0
```

- [ ] **Step 2: Run it**

```bash
MAP2CHECK_PATH=$PWD/install_clam SAMPLE=40 BUDGET=60 \
  bash tests/integration/test_invariants_no_regression.sh
```

- [ ] **Step 3: Write up the result**

Record, in `docs/reports/`, the sample size, the budget, and the three counters. State
plainly whether the recommendation changes:

- `lost > 0` → invariants stay off, and the lost cases become the investigation. This
  is a finding, not a setback: it would be the first direct evidence about how Clam
  models unresolved externals.
- `lost == 0 && gained == 0` → the capability works and buys nothing measurable on this
  corpus. Keep it off by default and say so.
- `lost == 0 && gained > 0` → the case for promoting the default, to be made with the
  numbers attached.

- [ ] **Step 4: Commit**

```bash
git add tests/integration/test_invariants_no_regression.sh docs/reports/
git commit -m "test(invariants): differential run before any default change"
```

---

## Self-review

**Spec coverage.** The review's Phase A ("parar o silêncio", both input paths, testable
in CI) is Tasks 1 and 2. Its Phase B (Clam `dev16`, `-DENABLE_CLAM=ON`, off by default,
promotion gated on differential evidence) is Tasks 3 through 7. The review's rejected
options — removing the capability, enabling by default, IKOS, a bespoke pass over
`crab` — appear nowhere in the plan, which is the intent.

**The progress-detector requirement.** A single test file asserts two contracts and
prints which one it checked (Task 1 Step 1). It passes today in the `ABSENT` state and
will pass in the `PRESENT` state only if Clam actually injects `verifier.assume`.
Task 6 builds with `-DENABLE_CLAM=ON` so the `PRESENT` branch becomes reachable the
moment the published image carries Clam. This satisfies "falharem agora e depois
pararem de falhar" without ever parking a red check in CI — a red check that is
expected to be red gets ignored, and then the day it turns green nobody notices.

**The gap between the phases is deliberate.** After Task 1 the three call sites that
request invariants start failing, and they stay that way until Phase B. That was the
explicit decision: the flag is not withdrawn from any caller, because a request the
tool cannot yet satisfy is exactly the signal that should be visible, and withdrawing
it would recreate the silence issue #54 is about. The single carve-out is CASTLE's
pass 2, which overwrites pass 1's verdict — there the failure would land as a corrupted
CSV rather than as a visible error, so exit 3 is recognised and the earlier verdict
kept.

**Ordering constraint.** Task 2 should still land with or immediately after Task 1, not
because of the wrappers but because of that carve-out: between the two commits, any
CASTLE run writes `ERROR` over legitimate `UNKNOWN` rows.

**Placeholder scan.** Two steps are deliberately parameterised by observation and say
so: Task 3 Step 3 records the working Clam command line, and Task 5 Step 1 transcribes
it. That ordering exists so the flag list is verified before it is committed. No TBDs
elsewhere.

**Type consistency.** `invariantGeneratorAvailable()` returns `bool` and is defined in
Task 1, redefined in Task 5, and called from one place. `compileToCrabLlvm()` is
renamed to `compileWithClam()` in both `caller.hpp` and `caller.cpp` in Task 5, and its
only call site is updated in the same task. `ERROR_UNAVAILABLE_CAPABILITY` is `size_t`,
matching the surrounding constants and the function's return type.

**What this plan does not claim.** That invariants will reduce UNKNOWN. The review is
explicit that the two tools closest to Map2Check in architecture — Symbiotic and
FuSeBMC — reduce the same gap without injected invariants. Task 7 exists to find out
what this one actually buys, and its most likely honest outcome is
`lost == 0 && gained == 0`.
