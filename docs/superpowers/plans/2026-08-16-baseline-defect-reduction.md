# Baseline Defect Reduction Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Reduce the defects the v5 baseline measured, in descending order of how many cases each one actually costs.

**Architecture:** Three independent workstreams. Tasks 1–2 change how Map2Check hands unresolved library calls to KLEE, which is where the overwhelming majority of lost cases come from. Task 3 fixes how the Juliet harness scores use-after-free, which is where the overwhelming majority of false positives come from. Task 4 is an investigation, not a fix — the second-largest bucket of lost cases has no diagnosed cause yet, and planning a fix for it would be guessing. Task 5 is a cheap correctness fix in witness output.

**Tech Stack:** C++17 / LLVM 16 (`modules/frontend`), C (`modules/backend/library`), KLEE 3.1, Bash harnesses under `tests/` and `test-comp2026/simulation/`.

**Spec:** `docs/reports/2026-08-16-baseline-v5.html` — the v5 baseline report. Every number quoted below comes from it or from the CSVs it was computed from.

## Global Constraints

- LLVM 16 only; New Pass Manager; opaque pointers. Do not reintroduce `boost/filesystem` — `caller.hpp` uses C++17 `<filesystem>`.
- C++ follows Google style (`clang-format` with `.clang-format`); C follows LLVM style. Verify with `./check_code_style.py -p` and `-c`.
- The hybrid default (LibFuzzer for 0.2× the budget, then KLEE for 0.8×) is a product decision and must be preserved.
- Architectural and product decisions follow conventions established by SV-COMP, Test-Comp, CASTLE, Juliet and FuseBMC. Where this plan makes a modelling choice, it states which convention it follows.
- No change may reduce precision. v5 measured 100% on CASTLE and 99.2% on Juliet once benchmark-mapping artifacts are excluded; that is the tool's strongest property and is the thing to protect.
- Every task re-measures the slice it claims to fix and records the before/after in the task's commit message. A fix that is not measured is not done.

---

## Measured impact, and why the order is what it is

| # | Defect | Addressable cases | Share of the 2,525-case Juliet run |
|---|---|---|---|
| 1–2 | Unresolved externals: input never reaches the checker | **575 lost** (341 TIMEOUT + 183 FN + 51 ERROR) | 23% |
| 4 | Undecided cluster in CWE-121/122 — **cause unknown** | 252 UNKNOWN | 10% |
| 3 | CWE-416 scored against the leak property | 42 cases, 21 of the 36 FP, TN unreachable | 1.7% |
| 0 | `--add-invariants` silently ignored for bitcode input | 12 CASTLE cases, 481 s of wasted pass-2 | — |
| — | Floating-point guard concretized (see Task 2, step 6) | 3 FP | 0.1% |
| 5 | Witness points at the wrong line | 1 CASTLE case | — |

887 of 2,525 Juliet cases (35%) take their input through an external library call. Of those, 575 are lost. That single axis is larger than everything else in this plan combined, which is why it is Tasks 1 and 2.

**On the correlation that did *not* survive checking:** 252 of 252 UNKNOWN cases log `undefined reference to function`, which looks like the same root cause. It is not — 132 of 152 sampled *decided* cases log it too (87% base rate). The signal is confounded by KLEE's normal libc warnings. Task 4 is therefore an investigation, and no fix is planned for that bucket until it has a diagnosed cause.

---

### Task 0: Stop `--add-invariants` from being silently ignored

Extends **issue #54** (*crabllvm is incompatible with LLVM 16 — `--add-invariants` is a no-op*). That issue records the fallback landed in `map2check.cpp`: when `bin/crabllvm/bin/crabllvm.py` is missing, warn and compile plainly. The v5 baseline shows the fallback never fires in practice.

`map2check.cpp:226` guards the whole branch with `if (!is_llvmir_in)`. Both baseline runners pass pre-compiled `.bc`, so `is_llvmir_in` is true, the branch is skipped, and the flag is accepted and ignored **without the warning**. Reproduced directly:

```
$ map2check --memtrack --add-invariants --timeout 20 /tmp/x.bc
  (no mention of crabllvm or invariants)
$ map2check --memtrack --add-invariants --timeout 20 /tmp/x.c
  crabllvm is not built — ignoring --add-invariants
```

The cost is real: the CASTLE runner uses `--add-invariants` as its pass-2 fallback on UNKNOWN, so v5 ran 12 second passes that could not differ from the first, burning 481 s. It also makes the CSV's `invariants=yes` column false — it records a technique that was never applied.

Two things are wrong and both are cheap: the warning must reach bitcode callers, and the CASTLE harness must stop paying for a pass that cannot change anything.

**Files:**
- Modify: `modules/frontend/map2check.cpp:226-247`
- Modify: `tests/castle/run_castle_evaluation.sh` (the pass-2 condition)
- Test: `tests/integration/test_add_invariants_warns.sh` (create)

**Interfaces:**
- Consumes: nothing new
- Produces: no signature change; `--add-invariants` warns on every input kind

- [ ] **Step 1: Write the failing test**

Create `tests/integration/test_add_invariants_warns.sh`:

```bash
#!/bin/bash
# test_add_invariants_warns.sh — --add-invariants must never be silently
# ignored. See issue #54: crab-llvm is not built against LLVM 16, so the flag
# does nothing; the fallback warning existed but was unreachable for bitcode
# input, which is how every baseline run invokes the tool.
set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_dev}"
CLANG="${CLANG:-/usr/bin/clang-16}"
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
FAILED=0

printf 'int main(void){int *p = 0; return *p;}\n' > "$WORK/x.c"
"$CLANG" -c -emit-llvm -g -O0 "$WORK/x.c" -o "$WORK/x.bc" || { echo "FAIL compile"; exit 1; }

cd "$WORK"
for input in x.c x.bc; do
  out=$(MAP2CHECK_PATH="$MAP2CHECK_DIR" "$MAP2CHECK_DIR/map2check" \
          --memtrack --add-invariants --timeout 20 "$WORK/$input" 2>&1)
  if echo "$out" | grep -aq 'ignoring --add-invariants'; then
    echo "  PASS $input warns that --add-invariants is ignored"
  else
    echo "  FAIL $input accepted --add-invariants silently"
    FAILED=$((FAILED+1))
  fi
done

[ "$FAILED" -eq 0 ] && exit 0 || exit 1
```

- [ ] **Step 2: Run the test to verify it fails**

Run:
```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash tests/integration/test_add_invariants_warns.sh
```
Expected: `PASS x.c ...` then `FAIL x.bc accepted --add-invariants silently`.

- [ ] **Step 3: Move the check out from behind the bitcode guard**

Replace `modules/frontend/map2check.cpp:226-247` with:

```cpp
  // Crab-LLVM is a legacy SeaHorn dependency that was never migrated to
  // LLVM 16 (FindCrabLlvm.cmake is not wired into the modern build), so
  // ${MAP2CHECK_PATH}/bin/crabllvm may be absent -- see issue #54.
  //
  // The check sits outside the !is_llvmir_in branch on purpose. Invariant
  // generation only applies to a source compile, but the WARNING has to reach
  // everyone who passed the flag: the evaluation harnesses feed pre-compiled
  // bitcode, so guarding the warning on source input meant --add-invariants
  // was accepted and dropped in complete silence for every baseline run.
  bool crabAvailable = false;
  if (args.invCrabLlvm) {
    const char *m2cPath = getenv("MAP2CHECK_PATH");
    std::string crabPy = (m2cPath ? std::string(m2cPath) : std::string("")) +
                         "/bin/crabllvm/bin/crabllvm.py";
    crabAvailable = fs::exists(crabPy);
    if (!crabAvailable) {
      Map2Check::Log::Warning(
          "crabllvm is not built — ignoring --add-invariants");
    } else if (is_llvmir_in) {
      Map2Check::Log::Warning(
          "input is already LLVM IR — ignoring --add-invariants");
    }
  }

  if (!is_llvmir_in && args.invCrabLlvm && crabAvailable) {
    caller->compileToCrabLlvm();
  } else {
    caller->compileCFile(is_llvmir_in);
  }
```

- [ ] **Step 4: Rebuild and run the test to verify it passes**

```bash
docker run --rm -u 0 -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash -c 'cd build_dev && ninja && ninja install'
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash tests/integration/test_add_invariants_warns.sh
```
Expected: both `PASS` lines.

- [ ] **Step 5: Stop the CASTLE harness paying for a pass that cannot help**

In `tests/castle/run_castle_evaluation.sh`, replace the pass-2 condition:

```bash
  # --- Pass 2: --add-invariants fallback ---
  # Disabled while issue #54 is open: crab-llvm is not built against LLVM 16,
  # so --add-invariants is a no-op and the second pass is bit-for-bit identical
  # to the first. In v5 it cost 481s across 12 cases and changed no verdict.
  # Re-enable by setting CASTLE_PASS2=1 once crab-llvm builds again.
  if [ "${CASTLE_PASS2:-0}" = "1" ] && [ "$verdict" = "UNKNOWN" ] && [ "$mode_flags" != "--check-asserts" ]; then
```

- [ ] **Step 6: Confirm no case still reports using invariants**

```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace \
  -e MAP2CHECK_PATH=/workspace/install_dev \
  -e RESULTS_DIR=/workspace/tests/castle/results_probe_inv \
  map2check-dev bash tests/castle/run_castle_evaluation.sh

awk -F, 'NR>1 && $7=="yes"' tests/castle/results_probe_inv/castle_results.csv | wc -l
```
Expected: `0`. In v5 it was 12.

- [ ] **Step 7: Commit and update issue #54**

```bash
git add modules/frontend/map2check.cpp tests/castle/run_castle_evaluation.sh \
        tests/integration/test_add_invariants_warns.sh
git commit -m "fix(cli): warn when --add-invariants is ignored for bitcode input

The fallback added for issue #54 sat behind 'if (!is_llvmir_in)', so it never
fired for pre-compiled bitcode -- which is how both evaluation harnesses invoke
the tool. --add-invariants was accepted and dropped in silence for every run in
the v5 baseline.

The CASTLE pass-2 fallback is disabled behind CASTLE_PASS2 while #54 is open:
with crab-llvm unbuilt the second pass is identical to the first, and it cost
481s across 12 cases in v5 without changing a single verdict.

Refs #54"
```

Then comment on issue #54 with the reproduction above and the 481 s measurement, so the issue records that the landed workaround does not cover the project's own usage pattern.

---

### Task 1: Symbolic stdin through KLEE's POSIX runtime

The largest single group: 242 of 345 Juliet timeouts come from `fscanf` families. Juliet's own header for these files says *"BadSource: fscanf Read data from the console using fscanf()"* — the input is stdin. Map2Check currently invokes KLEE with `--external-calls=all --libc=uclibc` and no POSIX runtime, so `fscanf` is executed for real against the actual stdin and the analysis explores one concrete path.

KLEE ships `--posix-runtime`, which models file descriptors and stdin as symbolic objects. Modelling an unknown input as nondeterministic rather than concretizing it is the sound over-approximation and is standard SV-COMP treatment — the same reasoning already documented in `test-comp2026/simulation/juliet_stubs.c` for `rand()`.

**Files:**
- Modify: `modules/frontend/caller.cpp` (KLEE branch, the `kleeCommand` construction beginning at the `case (NonDetGenerator::Klee)` label)
- Modify: `modules/frontend/utils/tools.hpp` (add the symbolic-stdin size constant next to `killGracePeriod`)
- Test: `tests/integration/test_symbolic_stdin.sh` (create)

**Interfaces:**
- Consumes: `Map2Check::killGracePeriod` (already present in `tools.hpp`)
- Produces: `Map2Check::symbolicStdinBytes` (`constexpr int`), consumed by nothing else; the KLEE command string gains `--posix-runtime -sym-stdin <n>`

- [ ] **Step 1: Write the failing test**

Create `tests/integration/test_symbolic_stdin.sh`:

```bash
#!/bin/bash
# test_symbolic_stdin.sh — a program whose only bug is reachable through stdin
# must be detected. Before the POSIX runtime was linked, KLEE executed fscanf
# for real, saw one concrete value, and reported the program safe.
set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAP2CHECK="${MAP2CHECK_PATH:-/workspace/install_dev}/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

cat > "$WORK/stdin_overflow.c" <<'EOF'
#include <stdio.h>
#include <limits.h>
int main(void) {
  int data = 0;
  /* Only INT_MAX overflows; a single concrete draw will never hit it. */
  if (fscanf(stdin, "%d", &data) != 1) { return 0; }
  int result = data + 1;
  printf("%d\n", result);
  return 0;
}
EOF

"$CLANG" -c -emit-llvm -g -O0 "$WORK/stdin_overflow.c" -o "$WORK/t.bc" || {
  echo "FAIL: compile"; exit 1; }

cd "$WORK"
out=$(timeout -k 30 300 "$MAP2CHECK" --check-overflow --timeout 60 "$WORK/t.bc" 2>&1)

if echo "$out" | grep -q "FALSE-OVERFLOW"; then
  echo "PASS stdin-sourced overflow is detected"
  exit 0
fi
echo "FAIL expected FALSE-OVERFLOW, got: $(echo "$out" | grep -aoE 'VERIFICATION [A-Z]+|FALSE-[A-Z]+' | tail -1)"
exit 1
```

- [ ] **Step 2: Run the test to verify it fails**

Run:
```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash tests/integration/test_symbolic_stdin.sh
```
Expected: `FAIL expected FALSE-OVERFLOW, got: VERIFICATION SUCCEEDED` — the current binary proves the program safe because `fscanf` returned one concrete value.

- [ ] **Step 3: Add the constant**

In `modules/frontend/utils/tools.hpp`, directly below `killGracePeriod`:

```cpp
/** Bytes of symbolic stdin handed to KLEE's POSIX runtime.
 * Juliet's fscanf sources read a single number from the console, so a small
 * buffer covers them; making it large costs solver time on every run. */
constexpr int symbolicStdinBytes = 32;
```

- [ ] **Step 4: Link the POSIX runtime**

In `modules/frontend/caller.cpp`, in the KLEE branch, immediately after the existing `--external-calls=all` append:

```cpp
      // Model stdin as symbolic instead of reading the real one. Without this
      // KLEE executes fscanf/fgets for real, gets a single concrete value, and
      // reports VERIFICATION SUCCEEDED for programs whose bug is only
      // reachable through input -- an unsound TRUE, not an UNKNOWN. 242 of the
      // 345 timeouts and a large share of the false negatives in the v5
      // baseline came from this. Treating an unknown input as nondeterministic
      // is the sound over-approximation and is what SV-COMP expects.
      kleeCommand << " --posix-runtime";
      kleeCommand << " -sym-stdin " << Map2Check::symbolicStdinBytes;
```

- [ ] **Step 5: Rebuild**

```bash
docker run --rm -u 0 -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash -c 'cd build_dev && ninja && ninja install'
```

- [ ] **Step 6: Run the test to verify it passes**

Run:
```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash tests/integration/test_symbolic_stdin.sh
```
Expected: `PASS stdin-sourced overflow is detected`

- [ ] **Step 7: Verify precision did not regress**

The whole existing integration suite must still pass — `--posix-runtime` changes the environment every analysis runs in, so a regression here would show up as previously-safe programs now reporting bugs.

```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev bash -c '
  bash tests/integration/test_verdict_classifier.sh &&
  bash tests/integration/test_juliet_sampling.sh &&
  bash tests/integration/test_modes.sh'
```
Expected: all pass; `test_modes.sh` reports `16 passed, 0 failed`.

- [ ] **Step 8: Measure the fscanf slice**

```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace \
  -e MAP2CHECK_PATH=/workspace/install_dev -e JULIET_CWES="190" -e PER_FAMILY=3 \
  -e RESULTS_DIR=/workspace/test-comp2026/simulation/resultados_de_testes/probe_stdin \
  map2check-dev bash test-comp2026/simulation/run_juliet_evaluation.sh

awk -F, '$2 ~ /fscanf/ {print $8}' \
  test-comp2026/simulation/resultados_de_testes/probe_stdin/juliet_scope_c_results.csv \
  | sort | uniq -c
```
v5 baseline for this slice, for comparison: CWE-190 had 144 TIMEOUT and 37 ERROR, both dominated by `fscanf` families. Success is TIMEOUT falling sharply with TP rising and **FP staying at zero**.

- [ ] **Step 9: Commit**

```bash
git add modules/frontend/caller.cpp modules/frontend/utils/tools.hpp tests/integration/test_symbolic_stdin.sh
git commit -m "fix(klee): model stdin symbolically instead of reading the real one

Juliet's fscanf sources read from the console. Without --posix-runtime KLEE
executed fscanf for real, took one concrete value, and reported programs safe
whose bug was only reachable through input. That is an unsound TRUE, not an
UNKNOWN, and it accounted for 242 of the 345 timeouts in the v5 baseline.

Before/after on the CWE-190 fscanf slice: <paste the two uniq -c outputs>"
```

---

### Task 2: Nondeterministic socket sources

96 of the remaining timeouts come from `listen_socket` and `connect_socket` families. These call real `socket`/`bind`/`listen`/`accept`/`recv`, which KLEE cannot resolve; the analysis stalls until the budget expires. The POSIX runtime from Task 1 models files, not sockets, so this needs its own treatment.

The convention to follow is the one already established in this repository for `rand()`: intercept at the lowest unresolved boundary and return a nondeterministic value constrained to what the real function could produce. Here that boundary is `recv`, which returns a byte count in `[-1, requested_len]`.

**Files:**
- Modify: `test-comp2026/simulation/juliet_stubs.c`
- Test: `tests/integration/test_socket_source.sh` (create)

**Interfaces:**
- Consumes: `__VERIFIER_nondet_int` (already used in this file for `globalReturnsTrueOrFalse` and `rand`)
- Produces: definitions of `socket`, `bind`, `listen`, `accept`, `recv`, `connect`, `closesocket` in the linked bitcode

- [ ] **Step 1: Write the failing test**

Create `tests/integration/test_socket_source.sh`:

```bash
#!/bin/bash
# test_socket_source.sh — a Juliet socket-sourced overflow must be decided
# rather than run out its budget. In the v5 baseline every listen_socket and
# connect_socket family timed out.
set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"
MAP2CHECK="${MAP2CHECK_PATH:-/workspace/install_dev}/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"
LLVM_LINK="${LLVM_LINK:-/usr/bin/llvm-link-16}"
SUP="$REPO/test-comp2026/juliet/testcasesupport"
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

SRC=$(find "$REPO/test-comp2026/juliet/testcases/CWE190_Integer_Overflow" \
        -name '*int_listen_socket_add_01.c' | head -1)
[ -n "$SRC" ] || { echo "SKIP juliet submodule not checked out"; exit 0; }

"$CLANG" -c -emit-llvm -g -O0 -I "$SUP" \
  "$REPO/test-comp2026/simulation/juliet_stubs.c" -o "$WORK/stubs.bc" || {
    echo "FAIL: stubs compile"; exit 1; }
"$CLANG" -c -emit-llvm -g -O0 -DINCLUDEMAIN -DOMITGOOD -I "$SUP" \
  "$SRC" -o "$WORK/t.bc" || { echo "FAIL: testcase compile"; exit 1; }
"$LLVM_LINK" "$WORK/t.bc" "$WORK/stubs.bc" -o "$WORK/c.bc" || {
  echo "FAIL: link"; exit 1; }

cd "$WORK"
start=$(date +%s)
out=$(timeout -k 30 300 "$MAP2CHECK" --check-overflow --timeout 60 "$WORK/c.bc" 2>&1)
elapsed=$(( $(date +%s) - start ))

verdict=$(echo "$out" | grep -aoE 'FALSE-[A-Z]+|VERIFICATION [A-Z]+' | tail -1)
if [ "$elapsed" -ge 54 ]; then
  echo "FAIL still exhausts the budget (${elapsed}s), verdict=$verdict"
  exit 1
fi
echo "PASS socket source decided in ${elapsed}s: $verdict"
exit 0
```

- [ ] **Step 2: Run the test to verify it fails**

Run:
```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash tests/integration/test_socket_source.sh
```
Expected: `FAIL still exhausts the budget (5Xs)` — the case burns its whole 60 s slice.

- [ ] **Step 3: Add the socket stubs**

In `test-comp2026/simulation/juliet_stubs.c`, directly after the `srand` definition:

```c
/*
 * Socket sources — same reasoning as rand() above, applied at the boundary
 * that actually delivers data.
 *
 * Juliet's listen_socket/connect_socket families call the real
 * socket/bind/listen/accept/recv. KLEE cannot resolve them, so the analysis
 * stalls until its budget expires: 96 of the 345 timeouts in the v5 baseline.
 *
 * Only recv carries input, so only recv is nondeterministic; the rest return
 * a fixed success so the setup sequence stays feasible. recv's contract is a
 * byte count in [-1, len] (-1 being error), and the constraint matters for
 * the same reason RAND_MAX did: an unconstrained return would let the solver
 * invent a length the real call cannot produce, and manufacture a false
 * positive in the caller's buffer arithmetic.
 */
#define M2C_FAKE_FD 3

int socket(int domain, int type, int protocol) {
  NODISCARD(domain); NODISCARD(type); NODISCARD(protocol);
  return M2C_FAKE_FD;
}
int bind(int fd, const void *addr, unsigned len) {
  NODISCARD(fd); NODISCARD(addr); NODISCARD(len);
  return 0;
}
int listen(int fd, int backlog) { NODISCARD(fd); NODISCARD(backlog); return 0; }
int accept(int fd, void *addr, void *len) {
  NODISCARD(fd); NODISCARD(addr); NODISCARD(len);
  return M2C_FAKE_FD;
}
int connect(int fd, const void *addr, unsigned len) {
  NODISCARD(fd); NODISCARD(addr); NODISCARD(len);
  return 0;
}
int closesocket(int fd) { NODISCARD(fd); return 0; }

long recv(int fd, void *buf, unsigned long len, int flags) {
  extern int __VERIFIER_nondet_int(void);
  extern void __VERIFIER_assume(int);
  unsigned char *b = (unsigned char *)buf;
  unsigned long i;
  int n = __VERIFIER_nondet_int();
  NODISCARD(fd); NODISCARD(flags);
  /* recv returns -1 on error, otherwise at most the requested length. */
  __VERIFIER_assume(n >= -1 && (unsigned long)(n > 0 ? n : 0) <= len);
  if (n > 0) {
    for (i = 0; i < (unsigned long)n; i++) {
      b[i] = (unsigned char)__VERIFIER_nondet_int();
    }
  }
  return (long)n;
}
```

- [ ] **Step 4: Verify the stubs compile and define the symbols**

```bash
docker run --rm -u 0 -v "$(pwd)":/workspace map2check-dev sh -c '
/usr/bin/clang-16 -c -emit-llvm -g -O0 \
  -I /workspace/test-comp2026/juliet/testcasesupport \
  /workspace/test-comp2026/simulation/juliet_stubs.c -o /tmp/stub.bc &&
/usr/bin/llvm-nm-16 /tmp/stub.bc | grep -E " T (socket|recv|accept|listen|bind|connect)$"'
```
Expected: all six symbols listed as `T` (defined).

- [ ] **Step 5: Run the test to verify it passes**

Run:
```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash tests/integration/test_socket_source.sh
```
Expected: `PASS socket source decided in <54s`

- [ ] **Step 6: Check whether the floating-point false positives moved**

v5 produced exactly 3 genuine false positives, all `int64_t_rand_square` good variants, caused by KLEE silently concretizing the `imaxabs(data) <= sqrtl(LLONG_MAX)` guard. Nothing in Tasks 1–2 addresses floating point, so the expectation is that they persist — this step exists to confirm they neither spread nor multiply.

```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace \
  -e MAP2CHECK_PATH=/workspace/install_dev -e JULIET_CWES="190" -e PER_FAMILY=3 \
  -e RESULTS_DIR=/workspace/test-comp2026/simulation/resultados_de_testes/probe_socket \
  map2check-dev bash test-comp2026/simulation/run_juliet_evaluation.sh

awk -F, '$8=="FP"{print $2}' \
  test-comp2026/simulation/resultados_de_testes/probe_socket/juliet_scope_c_results.csv
```
Expected: at most the three `int64_t_rand_square` good variants. **Any new family appearing here is a regression and blocks the commit** — it would mean a stub is admitting behaviour the real function cannot produce.

- [ ] **Step 7: Commit**

```bash
git add test-comp2026/simulation/juliet_stubs.c tests/integration/test_socket_source.sh
git commit -m "feat(juliet): model socket sources nondeterministically

listen_socket/connect_socket families called the real socket API, which KLEE
cannot resolve; the analysis stalled until its budget expired -- 96 of the 345
timeouts in the v5 baseline. recv now returns a nondeterministic byte count
constrained to [-1, len] and fills the buffer with nondeterministic bytes; the
rest of the setup sequence returns fixed success.

The range constraint is deliberate: an unconstrained return would let the
solver invent a length the real call cannot produce and manufacture a false
positive downstream, the same trap RAND_MAX guards against for rand().

Before/after on the socket slice: <paste the two uniq -c outputs>"
```

---

### Task 3: Score use-after-free by the use-after-free property

21 of the 36 Juliet false positives are CWE-416 good variants, and all of them report `FALSE-MEMTRACK` — a *leak* verdict. Map2Check is right: `goodG2B()` mallocs 100 ints and never frees them, and the Juliet source says so itself (`/* POTENTIAL INCIDENTAL - Possible memory leak here if data was not freed */`). `good()` calls `goodG2B()` then `goodB2G()`, so every CWE-416 good program leaks by construction.

The harness maps CWE-416 to `--memtrack`, which checks leaks as well as use-after-free, so it asks the wrong question and then scores the right answer as a false positive. The consequence is worse than 21 bad rows: **TN is unreachable for CWE-416**, because no safe program can pass a leak check it was built to fail.

The fix belongs in the harness, not the tool. Map2Check's verdicts are already fine-grained (`FALSE-FREE`, `FALSE-DEREF`, `FALSE-MEMTRACK`); the harness is what collapses them.

**Files:**
- Modify: `test-comp2026/simulation/run_juliet_evaluation.sh` (the `classify_verdict` function)
- Test: `tests/integration/test_juliet_sampling.sh` (extend — it already owns harness-level assertions)

**Interfaces:**
- Consumes: `classify_map2check_verdict` from `tests/lib/verdict_classifier.sh` (unchanged)
- Produces: `classify_verdict <output> <rc> <elapsed> <budget> <cwe>` — gains a fifth positional argument

- [ ] **Step 1: Write the failing test**

Append to `tests/integration/test_juliet_sampling.sh`, before the results summary:

```bash
# --- CWE-416 must be scored on use-after-free, not on leaks ----------------
# Juliet's CWE-416 good variants leak by construction (goodG2B mallocs and
# never frees, and says so in a comment). Scoring them under --memtrack made
# every one a false positive and made TN unreachable for the whole CWE.
. "$SCRIPT_DIR/../../test-comp2026/simulation/juliet_classify_lib.sh"

got=$(classify_verdict "Violated property:
	FALSE-MEMTRACK
VERIFICATION FAILED" 0 2 60 416)
[ "$got" = "TRUE" ] && ok "CWE-416: an incidental leak is not a use-after-free" \
                    || fail "CWE-416 leak" "expected TRUE, got $got"

got=$(classify_verdict "Violated property:
	FALSE-FREE
VERIFICATION FAILED" 0 2 60 416)
[ "$got" = "FALSE" ] && ok "CWE-416: FALSE-FREE is still a detection" \
                     || fail "CWE-416 use-after-free" "expected FALSE, got $got"

got=$(classify_verdict "Violated property:
	FALSE-MEMTRACK
VERIFICATION FAILED" 0 2 60 401)
[ "$got" = "FALSE" ] && ok "CWE-401: FALSE-MEMTRACK is still a detection" \
                     || fail "CWE-401 leak" "expected FALSE, got $got"
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `bash tests/integration/test_juliet_sampling.sh`
Expected: FAIL on the source line — `juliet_classify_lib.sh` does not exist yet.

- [ ] **Step 3: Extract the classifier into a sourceable library**

Create `test-comp2026/simulation/juliet_classify_lib.sh`:

```bash
# shellcheck shell=bash
# Juliet-specific verdict collapsing, split out of run_juliet_evaluation.sh so
# it can be tested without executing the runner (the runner is a script, not a
# library: sourcing it starts compiling and analysing).

SCRIPT_LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=../../tests/lib/verdict_classifier.sh
. "$SCRIPT_LIB_DIR/../../tests/lib/verdict_classifier.sh"

# classify_verdict <output> <exit_code> <elapsed_s> <budget_s> <cwe>
#
# Juliet only needs the coarse FALSE/TRUE distinction, so the fine-grained
# FALSE-* verdicts collapse here -- with one exception.
#
# CWE-416 is use-after-free, but the runner analyses it under --memtrack, which
# also reports leaks. Juliet's CWE-416 good variants leak on purpose:
# goodG2B() mallocs and never frees, and the source comment says so. Collapsing
# FALSE-MEMTRACK into FALSE there scores a correct leak report as a false
# positive on the use-after-free property, and makes TN unreachable for the
# whole CWE -- 21 of the 36 false positives in the v5 baseline. Only the
# verdicts that ARE use-after-free count as a detection for 416.
classify_verdict() {
  local verdict cwe="${5:-}"
  verdict=$(classify_map2check_verdict "$1" "${2:-0}" "${3:-0}" "${4:-0}")
  if [ "$cwe" = "416" ] && [ "$verdict" = "FALSE-MEMTRACK" ]; then
    echo "TRUE"; return
  fi
  case "$verdict" in
    FALSE-*) echo "FALSE" ;;
    *)       echo "$verdict" ;;
  esac
}
```

- [ ] **Step 4: Point the runner at the library**

In `test-comp2026/simulation/run_juliet_evaluation.sh`, delete the inline `classify_verdict` definition and its preceding comment block, and replace them with:

```bash
# shellcheck source=juliet_classify_lib.sh
. "$SCRIPT_DIR/juliet_classify_lib.sh"
```

Then update the single call site to pass the CWE:

```bash
        verdict=$(classify_verdict "$output" "$rc" "$elapsed" "$INNER_TIMEOUT" "$cwe")
```

- [ ] **Step 5: Run the test to verify it passes**

Run: `bash tests/integration/test_juliet_sampling.sh`
Expected: `Results: 21 passed, 0 failed`

- [ ] **Step 6: Re-score CWE-416 from retained raw output**

The raw output of every v5 run was kept precisely so a scoring change does not require re-running. Confirm the 21 false positives become true negatives:

```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace \
  -e MAP2CHECK_PATH=/workspace/install_dev -e JULIET_CWES="416" -e PER_FAMILY=3 \
  -e RESULTS_DIR=/workspace/test-comp2026/simulation/resultados_de_testes/probe_416 \
  map2check-dev bash test-comp2026/simulation/run_juliet_evaluation.sh

cut -d, -f8 test-comp2026/simulation/resultados_de_testes/probe_416/juliet_scope_c_results.csv \
  | tail -n +2 | sort | uniq -c
```
v5 for this slice was `FP=21 FN=12 TP=9 TN=0`. Success is `FP=0` and `TN=21`; TP must stay at 9.

- [ ] **Step 7: Commit**

```bash
git add test-comp2026/simulation/juliet_classify_lib.sh \
        test-comp2026/simulation/run_juliet_evaluation.sh \
        tests/integration/test_juliet_sampling.sh
git commit -m "fix(juliet): score CWE-416 on use-after-free, not on leaks

CWE-416 is analysed under --memtrack, which reports leaks too. Juliet's 416
good variants leak by construction -- goodG2B() mallocs and never frees, and
the source says so -- so a correct FALSE-MEMTRACK was scored as a false
positive and TN was unreachable for the entire CWE. 21 of the 36 false
positives in the v5 baseline were this.

Before/after on CWE-416: FP=21 TN=0 -> FP=0 TN=21, TP unchanged at 9."
```

---

### Task 4: Diagnose the undecided cluster in CWE-121/122

252 cases — 10% of the suite, the second-largest bucket of lost work — end in UNKNOWN, and **the cause is not known**. This task produces a diagnosis, not a fix; the fix gets planned once there is something to fix.

Do not assume the cause is unresolved externals. That hypothesis was tested and did not survive: 252 of 252 UNKNOWN cases log `undefined reference to function`, but so do 132 of 152 sampled decided cases (87% base rate). The correlation is confounded by KLEE's ordinary libc warnings.

**Files:**
- Create: `docs/reports/2026-08-16-unknown-cluster-diagnosis.md`

**Interfaces:**
- Consumes: retained raw output under `test-comp2026/simulation/resultados_de_testes/juliet_v5_{a,d}/raw/`
- Produces: a diagnosis document; if it identifies a fixable cause, a follow-up plan

- [ ] **Step 1: Establish what distinguishes UNKNOWN from decided**

For each candidate signal, compute its rate in UNKNOWN cases and in decided cases from the same CWE. A signal only matters if the two rates differ.

```bash
cd test-comp2026/simulation/resultados_de_testes
for sig in 'halting execution' 'max-memory' 'silently concretizing' \
           'unable to solve' 'query timed out' 'Exited klee with [1-9]' \
           'partially completed paths' 'ASSERTION FAIL'; do
  for cls in UNKNOWN TP TN; do
    n=0; hit=0
    while IFS=, read -r cwe name variant vuln mode verdict el c; do
      [ "$c" = "$cls" ] || continue
      b=$(echo "$name" | sed 's#.*/##; s#\.c$##')
      r=$(ls juliet_v5_a/raw/*"$b"*."$variant".txt 2>/dev/null | head -1)
      [ -n "$r" ] || continue
      n=$((n+1)); [ "$n" -gt 120 ] && break
      grep -aqE "$sig" "$r" && hit=$((hit+1))
    done < <(tail -n +2 juliet_v5_a/juliet_scope_c_results.csv)
    printf '%-28s %-8s %3d/%3d\n' "$sig" "$cls" "$hit" "$n"
  done
done
```
Record every signal whose UNKNOWN rate exceeds its TP/TN rate by more than 20 points. Signals that do not clear that bar are confounds and must be written down as such — the base-rate trap above is exactly what this step exists to prevent repeating.

- [ ] **Step 2: Read three UNKNOWN cases end to end**

```bash
cd test-comp2026/simulation/resultados_de_testes
for b in CWE131_memcpy_01 CWE131_loop_01 CWE193_char_alloca_cpy_01; do
  f=$(ls juliet_v5_a/raw/*"$b"*bad* 2>/dev/null | head -1)
  [ -n "$f" ] && { echo "=================== $b"; cat "$f"; }
done
```
Read the whole file, not a grep of it. The `--memtrack` mode instruments allocation and deallocation; the question to answer is whether KLEE finished and could not decide, or stopped early.

- [ ] **Step 3: Determine whether the analysis completed**

For the same three cases, extract KLEE's own completion statistics:

```bash
cd test-comp2026/simulation/resultados_de_testes
for b in CWE131_memcpy_01 CWE131_loop_01 CWE193_char_alloca_cpy_01; do
  f=$(ls juliet_v5_a/raw/*"$b"*bad* 2>/dev/null | head -1)
  echo "--- $b"
  grep -aE 'KLEE: done|completed paths|generated tests|halting' "$f"
done
```
`completed paths` far below `generated tests`, or a `halting execution` line, means the search was cut short and the property was never exercised — a different problem from the analysis genuinely not being able to decide.

- [ ] **Step 4: Write the diagnosis**

Create `docs/reports/2026-08-16-unknown-cluster-diagnosis.md` containing: the signal table from Step 1 with both rates side by side, which signals cleared the 20-point bar and which were confounds, the three raw outputs' interpretation, and one of two conclusions — either a named root cause with the evidence for it, or an explicit statement that the cause is still unknown and what would be needed to find it. Do not write a conclusion the evidence does not support.

- [ ] **Step 5: Commit**

```bash
git add docs/reports/2026-08-16-unknown-cluster-diagnosis.md
git commit -m "docs: diagnose the CWE-121/122 undecided cluster

252 of 2,525 v5 cases end UNKNOWN with no diagnosed cause. Records what
distinguishes them from decided cases in the same CWEs, and separates real
signals from base-rate confounds -- the unresolved-externals hypothesis was
tested here and rejected (100% in UNKNOWN vs 87% in decided)."
```

---

### Task 5: Point the witness at the line that violated the property

`CASTLE-822-3.c` is a vulnerable program, Map2Check reports `FALSE-DEREF` on it, and the harness scores it a false positive solely because the reported line does not match the expected one. It is the only thing standing between the tool and 100% reported precision on CASTLE.

**Files:**
- Modify: `modules/frontend/witness/` (the generator that emits `map2check_property line N`)
- Test: `tests/integration/test_castle_regression.sh` (extend)

**Interfaces:**
- Consumes: nothing new
- Produces: no signature change; the emitted line number changes

- [ ] **Step 1: Reproduce and capture the wrong line**

```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /tmp map2check-dev bash -c '
/usr/bin/clang-16 -c -emit-llvm -g -O0 -Wno-everything \
  /workspace/tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250/CASTLE-822-3.c -o /tmp/t.bc
cd /tmp && /workspace/install_dev/map2check --memtrack --timeout 300 /tmp/t.bc 2>&1 \
  | grep -aE "map2check_property line|FALSE-"'

python3 -c "
import json
d=json.load(open('tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250.min.json'))
print([t['lines'] for t in d['tests'] if t['name']=='CASTLE-822-3.c'])"
```
Record both numbers. The gap between them is the defect.

- [ ] **Step 2: Write the failing test**

Append to `tests/integration/test_castle_regression.sh`:

```bash
# --- The witness must name the line that violated the property -------------
# CASTLE-822-3 is detected correctly but the witness points elsewhere, which
# scores a true positive as a false positive. Precision on CASTLE is 100% with
# this fixed and 98.2% without.
/usr/bin/clang-16 -c -emit-llvm -g -O0 -Wno-everything \
  "$REPO/tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250/CASTLE-822-3.c" \
  -o "$WORK/822-3.bc" 2>/dev/null
cd "$WORK"
out=$("$MAP2CHECK" --memtrack --timeout 300 "$WORK/822-3.bc" 2>&1)
line=$(echo "$out" | grep -aoP 'map2check_property line \K\d+' | head -1)
want=$(python3 -c "
import json
d=json.load(open('$REPO/tests/castle/CASTLE-Benchmark/datasets/CASTLE-C250.min.json'))
print(','.join(str(x) for t in d['tests'] if t['name']=='CASTLE-822-3.c' for x in t['lines']))")
if echo ",$want," | grep -q ",$line,"; then
  echo "  PASS witness line $line is one of the expected [$want]"
else
  echo "  FAIL witness reported line $line, expected one of [$want]"
  FAILED=$((FAILED+1))
fi
```

- [ ] **Step 3: Run the test to verify it fails**

Run:
```bash
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash tests/integration/test_castle_regression.sh
```
Expected: `FAIL witness reported line <wrong>, expected one of [<right>]`

- [ ] **Step 4: Fix the line attribution**

Locate where the violating instruction's debug location is read in `modules/frontend/witness/`, and confirm it takes the location of the instruction that failed the check rather than of the instrumentation call or the enclosing block. Correct it to use the former.

- [ ] **Step 5: Rebuild and run the test to verify it passes**

```bash
docker run --rm -u 0 -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash -c 'cd build_dev && ninja && ninja install'
docker run --rm -u 0 --memory=4g -v "$(pwd)":/workspace -w /workspace map2check-dev \
  bash tests/integration/test_castle_regression.sh
```
Expected: `PASS witness line <n> is one of the expected [...]` and every pre-existing assertion in the file still passing.

- [ ] **Step 6: Commit**

```bash
git add modules/frontend/witness tests/integration/test_castle_regression.sh
git commit -m "fix(witness): report the line that violated the property

CASTLE-822-3 was detected correctly but the witness named the wrong line,
scoring a true positive as a false positive. It was the only false positive on
CASTLE; reported precision goes from 98.2% to 100%."
```

---

### Task 6: Re-run the full baseline as v6 and compare

Nothing in Tasks 1–5 counts until it is measured on the same scope that measured the problem.

**Files:**
- Create: `docs/reports/2026-XX-XX-baseline-v6.html` (date it on the day it is run)

- [ ] **Step 1: Launch, with the guards that made v5 survive**

The memory cap and the restart policy are not optional. Without `--memory=4g`, KLEE ballooned to 12.7 GB, crashed ten times in eight minutes, and each crash wrote a full memory dump that together filled 74 GB of the Windows disk. Without `--restart=on-failure`, a host restart leaves the containers dead instead of resuming.

```bash
OPTS="-u 0 --restart=on-failure:100 --memory=4g --memory-swap=6g \
      --log-opt max-size=20m --log-opt max-file=2"
docker run -d --name m2c-castle-v6 $OPTS -v "$(pwd)":/workspace -w /workspace \
  -e MAP2CHECK_PATH=/workspace/install_dev \
  -e RESULTS_DIR=/workspace/tests/castle/results_v6 \
  map2check-dev bash tests/castle/run_castle_evaluation.sh

for spec in "a:121" "b:190 476 415" "c:191 401 416" "d:122 369 761"; do
  sh=${spec%%:*}; cwes=${spec#*:}
  docker run -d --name "m2c-juliet-v6$sh" $OPTS -v "$(pwd)":/workspace -w /workspace \
    -e MAP2CHECK_PATH=/workspace/install_dev \
    -e JULIET_CWES="$cwes" -e PER_FAMILY=3 \
    -e RESULTS_DIR="/workspace/test-comp2026/simulation/resultados_de_testes/juliet_v6_$sh" \
    map2check-dev bash test-comp2026/simulation/run_juliet_evaluation.sh
done

BASELINE_TAG=v6 nohup bash tests/lib/baseline_watchdog.sh \
  "$(pwd)/tests/baseline-logs/watchdog_v6.log" 40 300 \
  m2c-castle-v6 m2c-juliet-v6a m2c-juliet-v6b m2c-juliet-v6c m2c-juliet-v6d &
```

- [ ] **Step 2: Compute the metrics from the CSVs**

Do not use the summary the runner prints — it counts only rows produced in its own session and under-reports every resumed run. It reported 28.6% recall for a CASTLE run whose real recall was 74.0%.

```bash
python3 - <<'EOF'
import glob, collections
rows=[]
for f in sorted(glob.glob('test-comp2026/simulation/resultados_de_testes/juliet_v6_*/juliet_scope_c_results.csv')):
    for line in open(f, encoding='utf-8', errors='replace'):
        line=line.strip()
        if not line or line.startswith('cwe,'): continue
        p=line.split(',')
        if len(p)==8 and '\x00' not in line: rows.append(p)
c=collections.Counter(r[7] for r in rows)
TP,FP,FN=c['TP'],c['FP'],c['FN']
bad=sum(1 for r in rows if r[3]=='True')
print(f"n={len(rows)}  " + "  ".join(f"{k}={v}" for k,v in c.most_common()))
print(f"precisao={TP/(TP+FP)*100:.1f}%  recall(decididos)={TP/(TP+FN)*100:.1f}%  recall(todos os bugs)={TP/bad*100:.1f}%")
EOF
```

- [ ] **Step 3: Report both recall figures**

v5 reported only `TP/(TP+FN)` = 46.8%, which counts únicamente the cases where the tool committed to an answer. Over every vulnerable program it was 28.2%. Both belong in the report: the gap between them *is* the size of the undecided problem, and quoting only the generous one hides exactly what this plan set out to fix.

- [ ] **Step 4: Write the v6 report and commit**

Mirror `docs/reports/2026-08-16-baseline-v5.html`, adding a v5→v6 delta column per CWE. State plainly any metric that moved the wrong way.

---

## Self-review

**Spec coverage.** Every defect quantified in the v5 report has a task: the silently-ignored `--add-invariants` (0, extending issue #54), externals (1, 2), CWE-416 mis-scoring (3), the undecided cluster (4), witness lines (5), re-measurement (6). Two are deliberately excluded, with reasons: the 3 floating-point false positives are a KLEE limitation with no cheap fix, so Task 2 Step 6 only guards against them spreading; `decodeHexChars` is dropped entirely because the scope sample contains **zero** such cases, making the earlier backlog item moot.

**Not covered, and deliberately so.** The CASTLE reachability failures — type confusion (5 FN), wrong-argument calls (3 FN), unbounded allocation (8 TIMEOUT), infinite loops (6 TIMEOUT) — need new analysis capability rather than a fix, and belong in their own plan after brainstorming. They are approximated today with `--target-function main`, which is not a check of those properties.

**Type consistency.** `classify_verdict` gains a fifth positional argument in Task 3 and is called with it at the single call site in the same task. `Map2Check::symbolicStdinBytes` is defined in Task 1 and used only there. `run_isolated` and `classify_map2check_verdict` are untouched.

**Risk to watch.** Tasks 1 and 2 both widen the input space, and widening input is what produced the 3 existing false positives. Task 1 Step 7 and Task 2 Step 6 are the gates; a new false-positive family blocks the commit.
