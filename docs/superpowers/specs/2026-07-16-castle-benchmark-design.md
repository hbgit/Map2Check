# CASTLE-Benchmark Integration for Map2Check

**Design spec** | 2026-07-16 | Reference: TODO #8 (SBSeg 2026)

## Goals

- Run all 250 CASTLE C250 micro-benchmarks through Map2Check
- Report per-CWE detection results (TP/FP/TN/FN) for the paper
- Add 5 regression tests to CI to prevent future regressions

## Non-goals

- CASTLE Score calculation (done by CASTLE-Source separately)
- Tool wrappers for automated CASTLE scoring (out of scope)
- BenchExec XML (not needed for paper evaluation)

---

## File Layout

```
tests/castle/
  CASTLE-Benchmark/              ← git submodule (pinned main)
  run_castle_evaluation.sh       ← standalone: compile → run → score → report
  results/
    castle_results.csv           ← per-test verdicts
    castle_summary.txt           ← paper-ready table

tests/integration/
  test_castle_regression.sh      ← 5 must-pass tests (CI)

.github/workflows/ci.yml         ← new job: castle-regression
```

### Dependencies

- `clang-16` (LLVM bitcode compilation)
- `map2check` binary (in PATH or `MAP2CHECK_PATH`)
- KLEE runtime libraries (for symbolic execution)
- `python3` (JSON parsing, already available)
- Git submodule: `CASTLE-Benchmark/main`

---

## Evaluation Script: `run_castle_evaluation.sh`

### Phases

1. **Setup**: Verify dependencies, ensure submodule is checked out
2. **Compile**: Each `CASTLE_C250/*.c` → `.bc` via `clang-16 -c -emit-llvm -g -O0`
3. **Verify**: Two-pass strategy per test:
   - Pass 1: `map2check <mode_flags> --timeout 60 <.bc>` → verdict
   - Pass 2 (if UNKNOWN): retry with `--add-invariants`
4. **Score**: Compare verdict against `CASTLE-C250.min.json` ground truth
5. **Report**: Write `castle_results.csv` + `castle_summary.txt`

### Mode Selection

| CWE Group | CWEs | Map2Check Flags |
|-----------|------|----------------|
| Memory safety | 125, 415, 416, 476, 761, 787, 822, 843 | `--memtrack` |
| Overflow | 190 | `--check-overflow` |
| Memcleanup | 401 | `--memcleanup-property` |
| Assert | 134, 617 | `--check-asserts` |
| Reachability (other) | 253, 362, 369, 628, 674, 770, 835 | `--target-function --target-function-name main` |
| Out of scope | 22, 78, 89, 327, 522, 798 | `--target-function --target-function-name main` (marked N/A) |

All modes use `--add-invariants` as UNKNOWN fallback in pass 2.

### Classification

| Verdict Substring | Ground Truth `vulnerable` | Line Match | Result |
|-------------------|--------------------------|------------|--------|
| `FALSE-DEREF`, `FALSE-FREE`, `FALSE-MEMTRACK`, `FALSE-OVERFLOW`, `FALSE-MEMCLEANUP` | `true` | Yes | **TP** |
| `FALSE-*` | `true` | No | **FP** (wrong line) |
| `FALSE-*` | `false` | -- | **FP** |
| `SUCCESSFUL` | `false` | -- | **TN** |
| `SUCCESSFUL` | `true` | -- | **FN** |
| `UNKNOWN`, timeout | any | -- | **UNKNOWN** |
| Out-of-scope CWE | any | -- | **N/A** |

### Output

**castle_results.csv:**
```
id,cwe,name,vulnerable,mode,verdict,invariants,expected,result,time_sec
```

**castle_summary.txt:** Aggregate table per CWE + overall totals for paper.

---

## Regression Tests: `test_castle_regression.sh`

5 CASTLE tests compiled to LLVM bitcode, verified with `--memtrack`:

| Test | CWE | Expected |
|------|-----|----------|
| CASTLE-476-5 | NULL deref (always-null return) | `VERIFICATION FAILED` |
| CASTLE-415-2 | Double free | `VERIFICATION FAILED` |
| CASTLE-476-8 | Safe NULL check | `VERIFICATION SUCCEEDED` |
| CASTLE-416-1 | Use-after-free | `VERIFICATION FAILED` |
| CASTLE-787-6 | Out-of-bounds array write | `VERIFICATION FAILED` |

> Note: the original `CASTLE-476-1` (NULL deref via `malloc` failure) and
> `CASTLE-787-3` (`strcpy` stack overflow) were replaced because `--memtrack`
> cannot reach them: KLEE does not model `malloc` returning NULL, and the pass
> does not instrument libc `strcpy` internals. The replacements exercise the
> same CWEs (476 NULL deref, 787 out-of-bounds write) via code paths that the
> instrumentation actually covers.

### CI Job: `castle-regression`

- **Trigger**: push to `develop`, `feat-*`; PRs
- **Runtime**: ~5 min
- **Runs**: `tests/integration/test_castle_regression.sh`
- **Depends on**: `build-and-test` (needs compiled map2check binary)
- **Environment**: ubuntu-22.04, clang-16, no Docker needed

---

## Implementation Order

1. Add CASTLE-Benchmark git submodule
2. Write `tests/integration/test_castle_regression.sh` (5 tests)
3. Add `castle-regression` CI job
4. Verify CI passes
5. Write `tests/castle/run_castle_evaluation.sh` (full 250)
6. Run evaluation, collect results for paper
