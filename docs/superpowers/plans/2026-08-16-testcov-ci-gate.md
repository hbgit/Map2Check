# TestCov CI Gate Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make the official Test-Comp validator a blocking CI check, so that a test
suite Map2Check emits is proven to reproduce the bug on every push — not merely proven
to be well-formed XML.

**Architecture:** A corpus of small C programs with known outcomes lives in
`tests/testcomp/`. A driver generates a suite for each one with Map2Check, zips it,
hands it to TestCov, and compares the verdict against a checked-in manifest of expected
results. The manifest is what makes the gate meaningful in both directions: a program
that regresses turns the job red, and a program that starts passing unexpectedly also
turns it red, forcing the manifest to be updated deliberately. A new CI job runs the
driver inside the same `map2check-dev` container the CASTLE Regression job already uses.

**Tech Stack:** GitHub Actions, Docker (`ghcr.io/hbgit/map2check-dev`), TestCov 3.7
from PyPI, bash.

**Spec:** [docs/superpowers/plans/2026-08-16-testcomp-testsuite-emitter.md](2026-08-16-testcomp-testsuite-emitter.md)
— this plan validates what that one emits. It also closes gap **H1.5 / G8** of
`.opencode/Plano de desenvolvimento_Testcomp.md` ("Pipeline que roda TestCov sobre
suites geradas", criterion: "Job verde no CI").

## Global Constraints

- **Prerequisite:** Task 3 of the emitter plan must be merged. Without
  `--generate-test-suite` there is nothing to validate. Task 4 of that plan
  (`tests/testcomp/run_testcov.sh`, `cover-error-call.prp`,
  `programs/two_guards.c`) is the seed this plan grows.
- **TestCov 3.7**, `pip install testcov`, from
  `https://gitlab.com/sosy-lab/software/test-suite-validator`. It is the same
  validator the competition uses.
- **`--no-isolation` is mandatory in CI.** TestCov defaults to BenchExec's `runexec`,
  which needs cgroup access that GitHub Actions runners do not grant. `--no-isolation`
  drops resource limits; the outer `timeout` provides the bound instead.
- **`--test-suite` takes a zip**, while the competition consumes a directory. The
  driver zips; the tool must not.
- **`-64`.** TestCov defaults to `-m32`. Map2Check compiles without an explicit machine
  model, so it inherits the host's 64-bit model. A mismatch produces confusing
  reproduction failures rather than an error.
- **The job must be able to fail.** The corpus includes a program whose error is
  unreachable; if the driver reports it as covered, the harness is lying and the job
  goes red. A gate that cannot fail is not a gate.
- **Budget:** `timeout-minutes: 20` for the job, `BUDGET=120` seconds per program, to
  stay inside the CASTLE Regression job's established envelope.

---

### Task 1: Pin down TestCov's result contract

**Files:**
- Modify: `tests/testcomp/run_testcov.sh`
- Create: `tests/testcomp/NOTES-testcov-output.md`

**Interfaces:**
- Consumes: `tests/testcomp/run_testcov.sh` and `programs/two_guards.c` from the
  emitter plan's Task 4.
- Produces: a `testcov_verdict()` bash function in `run_testcov.sh` that echoes exactly
  one of `COVERED`, `NOT_COVERED`, or `ERROR`, given TestCov's stdout in a file. Task 2
  builds on it.

The emitter plan's runner greps for `Cover-Error.*true\|Error covered`. That string was
inferred, not observed. Everything downstream depends on reading TestCov's verdict
correctly, and a grep that silently never matches would make every program report
`FAIL` — or worse, a grep that always matches would make the gate vacuous. Observe the
real output before building on it.

- [ ] **Step 1: Produce one suite that covers and one that does not**

```bash
python3 -m venv /tmp/tcvenv && /tmp/tcvenv/bin/pip install testcov
export MAP2CHECK_PATH=$PWD/release TESTCOV=/tmp/tcvenv/bin/testcov

# Covered: the emitter plan's program, whose error is reachable.
bash tests/testcomp/run_testcov.sh \
  tests/testcomp/programs/two_guards.c tests/testcomp/cover-error-call.prp
```

Then hand-build a suite that is valid but wrong, to see the other verdict:

```bash
WORK=$(mktemp -d) && cp tests/testcomp/programs/two_guards.c "$WORK/"
mkdir -p "$WORK/test-suite"
cat > "$WORK/test-suite/metadata.xml" <<EOF
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE test-metadata PUBLIC "+//IDN sosy-lab.org//DTD test-format test-metadata 1.1//EN" "https://sosy-lab.org/test-format/test-metadata-1.1.dtd">
<test-metadata>
  <sourcecodelang>C</sourcecodelang>
  <producer>manual</producer>
  <specification>COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )</specification>
  <programfile>two_guards.c</programfile>
  <programhash>$(sha256sum "$WORK/two_guards.c" | cut -d' ' -f1)</programhash>
  <entryfunction>main</entryfunction>
  <architecture>64bit</architecture>
  <creationtime>2026-08-16T12:00:00Z</creationtime>
</test-metadata>
EOF
cat > "$WORK/test-suite/testcase-1.xml" <<'EOF'
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE testcase PUBLIC "+//IDN sosy-lab.org//DTD test-format testcase 1.1//EN" "https://sosy-lab.org/test-format/testcase-1.1.dtd">
<testcase>
  <input>1</input>
  <input>2</input>
</testcase>
EOF
cp tests/testcomp/cover-error-call.prp "$WORK/"
( cd "$WORK/test-suite" && zip -q -r ../suite.zip . )
( cd "$WORK" && /tmp/tcvenv/bin/testcov --test-suite suite.zip --no-isolation -64 \
    --goal cover-error-call.prp two_guards.c ) | tee /tmp/testcov-notcovered.log
echo "exit=$?"
```

- [ ] **Step 2: Record what was observed**

Write `tests/testcomp/NOTES-testcov-output.md` with the verbatim final lines of both
runs and both exit codes. This is a lab note, not prose — the point is that the next
person can see why the parser is written the way it is:

```markdown
# TestCov 3.7 output contract, as observed

Recorded on <date>, TestCov 3.7 from PyPI, `--no-isolation -64`.

## Suite that covers the error
Exit code: <observed>
Final lines:
```
<paste verbatim>
```

## Valid suite that does not cover the error
Exit code: <observed>
Final lines:
```
<paste verbatim>
```

## Consequence
`testcov_verdict()` in run_testcov.sh keys off <the line that actually
distinguishes the two>. Exit code alone <is / is not> sufficient, because
<reason>.
```

- [ ] **Step 3: Replace the inferred grep with the observed parser**

In `tests/testcomp/run_testcov.sh`, replace the `if grep -qi "Cover-Error.*true\|Error
covered"` block with a function whose matching is justified by the notes:

```bash
# Verdict extraction. The patterns below are not guesses: they are what
# TestCov 3.7 printed on a covering and a non-covering suite, recorded in
# NOTES-testcov-output.md. If a TestCov upgrade changes the wording, this
# function returns ERROR rather than silently reporting NOT_COVERED, so the
# gate fails loudly instead of turning vacuous.
testcov_verdict() {
  local log="$1"
  if grep -qiE '<PATTERN OBSERVED FOR COVERED>' "$log"; then
    echo COVERED
  elif grep -qiE '<PATTERN OBSERVED FOR NOT COVERED>' "$log"; then
    echo NOT_COVERED
  else
    echo ERROR
  fi
}
```

Substitute the two placeholders with the patterns from Step 2 before committing.
Leaving them as written is a plan failure, not an implementation detail.

- [ ] **Step 4: Verify both verdicts are distinguishable**

```bash
bash -c '. tests/testcomp/run_testcov.sh 2>/dev/null; true'  # syntax check
bash -n tests/testcomp/run_testcov.sh
```
Then re-run both scenarios from Step 1 and confirm the function returns `COVERED` for
the first and `NOT_COVERED` for the second. If both return the same value, the parser
is wrong and the gate would be worthless.

- [ ] **Step 5: Commit**

```bash
git add tests/testcomp/run_testcov.sh tests/testcomp/NOTES-testcov-output.md
git commit -m "test(testcomp): key the TestCov verdict off observed output"
```

---

### Task 2: The corpus and its expected-results manifest

**Files:**
- Create: `tests/testcomp/programs/single_int.c`
- Create: `tests/testcomp/programs/no_input.c`
- Create: `tests/testcomp/programs/char_input.c`
- Create: `tests/testcomp/programs/loop_reads.c`
- Create: `tests/testcomp/programs/unreachable.c`
- Create: `tests/testcomp/expected.txt`
- Create: `tests/testcomp/run_testcov_suite.sh`
- Modify: `tests/testcomp/README.md`

**Interfaces:**
- Consumes: `testcov_verdict()` and the single-program flow from Task 1.
- Produces: `tests/testcomp/run_testcov_suite.sh`, which reads `expected.txt`, runs
  every listed program, and exits non-zero on any mismatch. Task 3 calls it with no
  arguments.

The manifest format is `<program> <expected-verdict>`, one per line, `#` for comments.
Three verdicts are legal: `COVERED`, `NOT_COVERED`, `ERROR`.

- [ ] **Step 1: Write the corpus**

Each program isolates one thing the emitter could get wrong.

`tests/testcomp/programs/single_int.c` — the minimal case; a failure here means the
whole mechanism is broken, not an edge case:

```c
// One nondeterministic read, one guard. The simplest suite that can exist.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 99) {
    reach_error();
  }
  return 0;
}
```

`tests/testcomp/programs/no_input.c` — the empty test vector, which the writer handles
explicitly and which a naive implementation drops:

```c
// No nondeterministic input at all. The suite must still contain one test case
// with zero <input> elements; emitting no test case would make the suite
// unscoreable even though the error is trivially reachable.
extern void reach_error(void);

int main(void) {
  reach_error();
  return 0;
}
```

`tests/testcomp/programs/char_input.c` — a non-`int` nondet type, since `NonDetLog.c`
formats values per type and only the integer path is covered by the other programs:

```c
// A char-typed nondeterministic read. NonDetLog.c formats the logged value
// according to the nondet type, so a type other than int exercises a distinct
// branch of the serializer.
extern char __VERIFIER_nondet_char(void);
extern void reach_error(void);

int main(void) {
  char c = __VERIFIER_nondet_char();
  if (c == 'A') {
    reach_error();
  }
  return 0;
}
```

`tests/testcomp/programs/loop_reads.c` — many reads, so an off-by-one in ordering or
counting shows up:

```c
// Four reads inside a loop. A suite that recovers the values but loses one, or
// reorders them, cannot reach the error -- so this checks the ordering contract
// at a length where an off-by-one is visible.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int sum = 0;
  for (int i = 0; i < 4; i++) {
    sum += __VERIFIER_nondet_int();
  }
  if (sum == 10) {
    reach_error();
  }
  return 0;
}
```

`tests/testcomp/programs/unreachable.c` — the negative control:

```c
// reach_error() is unreachable: the guard is unsatisfiable. No suite can cover
// it. This is the negative control -- if the harness ever reports this one as
// COVERED, the harness is broken and every other PASS in this corpus is
// meaningless.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 0 && x < 0) {
    reach_error();
  }
  return 0;
}
```

- [ ] **Step 2: Write the manifest**

`tests/testcomp/expected.txt`:

```
# Expected TestCov verdict per program. Format: <program> <verdict>
#
# Legal verdicts: COVERED, NOT_COVERED, ERROR.
#
# This file makes the gate bidirectional. A program that stops being COVERED is
# a regression. A program that starts being COVERED when this file says
# otherwise is also a failure -- it means the tool improved and nobody updated
# the record, and an unrecorded improvement is indistinguishable from a broken
# harness. Change a line here deliberately, in the commit that causes it.

single_int.c     COVERED
two_guards.c     COVERED
no_input.c       COVERED
char_input.c     COVERED
loop_reads.c     COVERED
unreachable.c    NOT_COVERED
```

If a program turns out not to be COVERED on first run, do **not** relax the assertion
silently: change the line to the observed verdict and write, in the same commit
message, why the tool cannot cover it yet. An honest `NOT_COVERED` with a reason is
data; a deleted test is a blind spot.

- [ ] **Step 3: Write the driver**

`tests/testcomp/run_testcov_suite.sh`:

```bash
#!/bin/bash
# run_testcov_suite.sh -- runs every program in expected.txt through Map2Check
# and TestCov, and compares the verdict against the manifest.
#
# Exit 0 only if every program's observed verdict equals its expected verdict.
# Both directions are failures: a regression, and an unrecorded improvement.
#
# Per-program budget is deliberately small. This is a conformance gate, not a
# benchmark; a program that needs more than two minutes to have its error
# covered does not belong in the corpus.

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MANIFEST="$SCRIPT_DIR/expected.txt"
PROPERTY="$SCRIPT_DIR/cover-error-call.prp"
export BUDGET="${BUDGET:-120}"

PASSED=0
FAILED=0

echo "============================================================"
echo "Test-Comp validation via TestCov"
echo "============================================================"

if ! command -v "${TESTCOV:-testcov}" >/dev/null 2>&1; then
  echo "FATAL: testcov not found. Install it with: pip install testcov" >&2
  exit 2
fi

while read -r program expected; do
  case "$program" in ''|\#*) continue ;; esac

  path="$SCRIPT_DIR/programs/$program"
  if [ ! -f "$path" ]; then
    echo "  FAIL $program: listed in expected.txt but the file is missing"
    FAILED=$((FAILED+1))
    continue
  fi

  observed=$(bash "$SCRIPT_DIR/run_testcov.sh" "$path" "$PROPERTY" --verdict-only)

  if [ "$observed" = "$expected" ]; then
    echo "  PASS $program: $observed"
    PASSED=$((PASSED+1))
  else
    echo "  FAIL $program: expected $expected, observed $observed"
    FAILED=$((FAILED+1))
  fi
done < "$MANIFEST"

echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
```

- [ ] **Step 4: Teach run_testcov.sh the `--verdict-only` mode**

In `tests/testcomp/run_testcov.sh`, accept an optional third argument. When it is
`--verdict-only`, print just the verdict word and nothing else, so the driver can
capture it. Add near the top, after `PROPERTY="$2"`:

```bash
VERDICT_ONLY=""
[ "${3:-}" = "--verdict-only" ] && VERDICT_ONLY=1
```

And replace the final reporting block with:

```bash
verdict=$(testcov_verdict "$WORK/testcov.log")

if [ -n "$VERDICT_ONLY" ]; then
  echo "$verdict"
  [ "$verdict" = "ERROR" ] && exit 1
  exit 0
fi

if [ "$verdict" = "COVERED" ]; then
  echo "PASS $name: TestCov reproduced the error"
  exit 0
fi
echo "FAIL $name: TestCov verdict is $verdict"
tail -20 "$WORK/testcov.log"
exit 1
```

Also handle the "no suite produced" case in verdict-only mode: a program whose error is
unreachable produces no violation, so Map2Check writes no suite. That must map to
`NOT_COVERED`, not to a hard failure. Replace the existing `if [ ! -d
"$WORK/test-suite" ]` block with:

```bash
if [ ! -d "$WORK/test-suite" ]; then
  # No violation found means no suite, which for validation purposes is exactly
  # "the error was not covered". Distinguishing "found nothing" from "found the
  # wrong thing" is the tool's job, not the validator's.
  if [ -n "$VERDICT_ONLY" ]; then
    echo NOT_COVERED
    exit 0
  fi
  echo "FAIL $name: no test suite produced"
  sed -n '$p' "$WORK/map2check.log"
  exit 1
fi
```

- [ ] **Step 5: Run the whole corpus locally**

```bash
chmod +x tests/testcomp/run_testcov_suite.sh
MAP2CHECK_PATH=$PWD/release TESTCOV=/tmp/tcvenv/bin/testcov \
  bash tests/testcomp/run_testcov_suite.sh
```
Expected: `Results: 6 passed, 0 failed`. Any mismatch is resolved by fixing the tool or
by amending `expected.txt` with a written reason — never by deleting the line.

- [ ] **Step 6: Prove the gate can fail**

A gate is only worth adding if a real defect turns it red. Break the emitter on
purpose and confirm the driver notices:

```bash
# Reverse the input order, which produces a structurally perfect but useless suite.
sed -i 's|for (const std::string& value : inputs) {|for (auto it = inputs.rbegin(); it != inputs.rend(); ++it) { const std::string\& value = *it;|' \
  modules/frontend/test_suite/test_suite.cpp
cd build && ninja && ninja install && cd ..
MAP2CHECK_PATH=$PWD/release TESTCOV=/tmp/tcvenv/bin/testcov \
  bash tests/testcomp/run_testcov_suite.sh
```
Expected: `two_guards.c` and `loop_reads.c` FAIL with `expected COVERED, observed
NOT_COVERED`; `single_int.c` still passes (one input cannot be reordered). Then revert:

```bash
git checkout modules/frontend/test_suite/test_suite.cpp
cd build && ninja && ninja install && cd ..
```

Record the outcome of this step in the commit message. If the corpus does not go red
under a reversed input order, the corpus is too weak and needs a program that depends
on ordering.

- [ ] **Step 7: Update the README and commit**

Append to `tests/testcomp/README.md`:

```markdown
## Running the whole corpus

    pip install testcov
    MAP2CHECK_PATH=<install prefix> bash tests/testcomp/run_testcov_suite.sh

`expected.txt` records the expected verdict per program. The gate fails on any
mismatch in either direction: a regression, and an improvement nobody recorded.
`unreachable.c` is the negative control — if it ever reports COVERED, the harness
is broken and no other result in this directory can be trusted.
```

```bash
git add tests/testcomp
git commit -m "test(testcomp): validate a corpus of suites against TestCov"
```

---

### Task 3: The CI job

**Files:**
- Modify: `.github/workflows/ci.yml`
- Modify: `CLAUDE.md`

**Interfaces:**
- Consumes: `tests/testcomp/run_testcov_suite.sh` from Task 2.
- Produces: a required CI check named `Test-Comp Validation (TestCov)`.

The job mirrors `castle-regression` (`ci.yml:298-354`), which already establishes the
working pattern: pull `map2check-dev`, build into a job-specific prefix, symlink the
KLEE runtime and clang headers, then run inside the container as root. Deviating from
that pattern is how the earlier permission failures happened — the image's default user
cannot execute the installed clang.

- [ ] **Step 1: Add the job**

Insert into `.github/workflows/ci.yml` after the `castle-regression` job ends
(before the `valgrind-memcheck` comment block at line 362):

```yaml
  # ===========================================================
  # Job: Test-Comp conformance — the official validator
  # ===========================================================
  # Asserts that an emitted test suite REPRODUCES the bug, which is what the
  # competition scores. Structural validity of the XML is asserted separately
  # by tests/integration/test_test_suite_emission.sh in the CASTLE job.
  testcov-gate:
    name: Test-Comp Validation (TestCov)
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
              rm -rf build_testcov_ci install_testcov_ci
              mkdir -p build_testcov_ci && cd build_testcov_ci
              cmake .. -G Ninja \
                -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
                -DMAP2CHECK_DYNAMIC_LINK=ON \
                -DCMAKE_INSTALL_PREFIX=/workspace/install_testcov_ci
              ninja
              ninja install
              mkdir -p /workspace/install_testcov_ci/lib/klee
              ln -sf /opt/klee/lib/klee/runtime /workspace/install_testcov_ci/lib/klee/runtime
              ln -sf /usr/lib/llvm-16/lib/clang /workspace/install_testcov_ci/lib/clang
            '

      - name: Run TestCov over the conformance corpus
        run: |
          docker run --rm \
            -u root \
            -v "${{ github.workspace }}:/workspace" \
            -w /workspace \
            -e MAP2CHECK_PATH=/workspace/install_testcov_ci \
            ghcr.io/hbgit/map2check-dev:latest bash -c '
              # TestCov needs zip for the suite archive, and gcc/lcov to
              # measure coverage of the compiled program.
              apt-get update -qq
              apt-get install -y -qq python3-pip zip gcc lcov
              pip3 install --quiet testcov
              bash tests/testcomp/run_testcov_suite.sh
            '

      - name: Upload TestCov logs on failure
        if: failure()
        uses: actions/upload-artifact@v4
        with:
          name: testcov-logs
          path: |
            tests/testcomp/*.log
          if-no-files-found: ignore
```

- [ ] **Step 2: Keep the logs when the job fails**

The driver's temp directories are removed by their `trap`, which makes a red CI job
undebuggable. In `tests/testcomp/run_testcov.sh`, make retention conditional:

```bash
# Keep the working directory when the caller asks, so a red CI job leaves
# something to read. The trap is the default because local runs would otherwise
# litter /tmp with one directory per program per invocation.
if [ -n "${KEEP_WORK:-}" ]; then
  trap 'echo "  (work kept at $WORK)"' EXIT
else
  trap 'rm -rf "$WORK"' EXIT
fi
```

And in the CI step, set `-e KEEP_WORK=1` on the `docker run` so a failure leaves the
logs behind.

- [ ] **Step 3: Add the gitignore entries**

Append to `.gitignore`, in the existing evaluation-artifacts block:

```
build_testcov_ci/
install_testcov_ci/
tests/testcomp/*.log
```

- [ ] **Step 4: Verify the workflow parses**

```bash
python3 -c "import yaml,sys; yaml.safe_load(open('.github/workflows/ci.yml')); print('ok')"
```
Expected: `ok`.

- [ ] **Step 5: Document the job**

In `CLAUDE.md`, in the CI section, extend the sentence listing the jobs so the new one
is named alongside the others, and add one line under it:

```markdown
`Test-Comp Validation (TestCov)` generates a test suite for each program in
`tests/testcomp/` and asks the official validator whether it reproduces the bug.
Expected verdicts live in `tests/testcomp/expected.txt`; a mismatch in either
direction fails the job.
```

- [ ] **Step 6: Commit and confirm the job is green**

```bash
git add .github/workflows/ci.yml .gitignore CLAUDE.md tests/testcomp/run_testcov.sh
git commit -m "ci: gate on TestCov reproducing the emitted test suites"
git push -u origin feat/testcomp-conformance
gh pr checks --watch
```
Expected: `Test-Comp Validation (TestCov)` passes, and no previously green check turns
red.

---

## Self-review

**Spec coverage.** H1.5's completion criterion is "Job verde no CI" for a "pipeline que
roda TestCov sobre suites geradas". Task 3 delivers exactly that. G8's broader ask
("suite E2E incluindo validação por TestCov local") is covered by Task 2's local
driver, which runs identically outside CI.

**Where this plan deliberately stops.**
- *Cover-Branches is not validated.* The corpus is Cover-Error only, because the
  emitter produces one test case per run. Branch coverage needs a suite of many test
  cases and is gap G4, in a later plan. Adding a branch-coverage program now would
  encode a permanent expected failure with no path to green.
- *The corpus is small and synthetic.* Six programs, none from SV-Benchmarks. This is a
  conformance gate on a 20-minute budget, not a benchmark. Coverage rate on real tasks
  is the baseline harness's job.
- *LibFuzzer is not exercised.* The driver pins `--nondet-generator symex` for
  determinism. The emitter is generator-agnostic by construction, but a fuzzer-driven
  gate would be flaky, and a flaky required check gets ignored, which is worse than an
  absent one.

**Placeholder scan.** One intentional placeholder exists and is flagged as such: the
two grep patterns in Task 1 Step 3, which cannot be written before TestCov's output is
observed in Step 1. The step states explicitly that leaving them unsubstituted is a
plan failure. Everything else is concrete.

**Type consistency.** `testcov_verdict()` echoes one of three literals, and those same
three literals are the legal values in `expected.txt` and the ones compared in
`run_testcov_suite.sh`. The `--verdict-only` third argument is added in Task 2 Step 4
and used in Task 2 Step 3. `BUDGET` is exported by the driver and read by
`run_testcov.sh`, which already defaults it.

**The assumption most likely to be wrong.** That `no_input.c` yields `COVERED`. A
program with no nondeterministic input produces an empty test vector, and TestCov may
treat a zero-input test case as degenerate. If it does, the manifest changes to
`NOT_COVERED` with the reason recorded — and that fact matters well beyond this gate,
because Test-Comp tasks with no input exist and would score zero.
