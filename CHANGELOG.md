# Changelog

All notable changes to Map2Check are documented in this file.
The format loosely follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

## [9.0.0] - Unreleased

A major version: the fuzzing engine changed (LibFuzzer → AFL++ 4.40c), the
`--nondet-generator` values changed (`fuzzer` → `afl`), and several verdicts
change meaning -- a KLEE run that did not explore every path is no longer
TRUE, and the program's own `abort()` prunes a path instead of ending the
search. Measured against the v15 baseline in `docs/reports/tacas-experiment-log.md`.

### Changed (breaking)

- **The default hybrid is the alternation** (`--alternate-engines`, with seed
  exchange) whenever `--timeout` is given. `--fixed-hybrid` keeps the 8.x
  schedule. Measured on the 213-task Cover-Error sample: 157 covered, against
  128 for the 8.x hybrid. On Cover-Branches: 53.5% against 46.5% (R26).
- The fuzzer's corpus is part of Cover-Branches suites by default
  (`MAP2CHECK_FUZZER_SUITE=0` turns it off).

- Fuzzing engine: LibFuzzer replaced by AFL++ 4.40c (persistent mode, PCGUARD,
  CmpLog). `--nondet-generator fuzzer` is now `--nondet-generator afl`.
- Verdicts: TRUE only from an exhaustive KLEE exploration. KLEE exiting 0 after
  its timer, after concretizing a symbolic input (floats), after killing
  states (`*.err`, `*.early`), near its memory cap, or after crashing is now
  UNKNOWN, never TRUE.
- The program's own `abort()` (inline or through `assume_abort_if_not`) prunes
  the path (`map2check_assume(0)`, `klee_silent_exit` under KLEE) instead of
  stopping KLEE's search.

### Added

- `--slice` for every property (reachability, assert, memtrack, memcleanup,
  overflow) through sbt-slicer, preserving the nondet read order; the slice is
  computed once per run (`<hash>.slice/`), a slicer failure is remembered, and
  the criteria are collected without regex. Experiment knobs:
  `MAP2CHECK_SLICE_CLEANUP=light|o2`, `MAP2CHECK_SLICER_FLAGS`.
- `--seed-exchange`: the engines hand each other input vectors through a
  persistent store (`<hash>.seeds/`) -- the fuzzer queue, replayed through the
  witness binary into typed `.ktest` seeds for KLEE (ranked: new-edge entries
  first), and KLEE's vectors back to the fuzzer.
- `--alternate-engines`: AFL++ and KLEE take turns, each ending when its engine
  stagnates (`AFL_EXIT_ON_TIME`; KLEE's covered instructions from `run.stats`,
  SQLite optional), with windows and patience doubling every round.
- KLEE's vectors, completed with zeros past their end, are run natively
  through the witness after every KLEE phase that found nothing.
- AFL++ binaries built once per run (`<hash>.build/`), within one build budget.
- The fuzzer's corpus contributes Cover-Branches test cases (up to half the
  suite, deduplicated).
- `--add-invariants` profiles (`MAP2CHECK_CLAM_PROFILE=default|memory|none`),
  the number of invariants inserted in the log, and a fallback when Clam
  fails. `MAP2CHECK_PREOPT=ssa` (reachability and assert): the module in SSA
  form before instrumentation.
- `MAP2CHECK_CHECK_CSTRINGS=1`: the strings a `%s` or `puts` reads are checked.

### Fixed

- The AFL++ generator replayed its input from the start past its end: a
  `while (__VERIFIER_nondet_int())` loop never ended and afl-fuzz aborted in
  its dry run. Reads past the end are now zero.
- MemoryTrackPass matched memory intrinsics by their LLVM 6 names:
  `memset/memcpy/memmove` were never checked under LLVM 16.
- A fuzzer binary that fails to link is reported with its cause instead of as
  a build timeout; an unreadable input program is reported as such.
- The Cover-Branches suite: the 50-case cap and duplicates count across
  phases, and a run starts from an empty suite.
- Evaluation harnesses: children no longer inherit the manifest's descriptor
  (the program under test could move the loop's offset); a crash replayed from
  the fuzzer is not a tool failure.


### Changed

- Replaced LibFuzzer with AFL++ 4.40c (persistent, PCGUARD) as the fuzzing engine.
- tacasv2a: `--slice` no longer crashes KLEE and its test suites stay valid
  on the original program. The slicer runs with `-cutoff-diverging=false`
  (the cutoff's `exit(0)` had no debug location and KLEE rejected the
  module), and every `__VERIFIER_nondet_*` function is a slicing criterion,
  so the read order is preserved. `--slice` now also works with
  `--check-asserts`. The slice is logged in functions/blocks/instructions.
- Migrated the toolchain from LLVM 6.0 to LLVM 16, moving all instrumentation passes (`modules/backend/pass/`) to the New Pass Manager and opaque pointers.
- Migrated the codebase to C++17 (CMake `CMAKE_CXX_STANDARD` 11 → 17, required by LLVM 16 headers).
- Upgraded KLEE to 3.1.
- Bumped project version to 8.0.0 (`CMakeLists.txt`).

### Added

- `Dockerfile.dev` development image (Ubuntu 22.04 + LLVM 16 + KLEE 3.1).
- GitHub Actions CI pipeline (`ci.yml`): build + unit tests, static analysis (clang-tidy, cppcheck), and ASan/UBSan sanitizer jobs.
- Docker image publishing workflow (`docker-publish.yml`).
- Static analysis configuration (`.clang-tidy`, `.cppcheck-suppressions.txt`).
- Migration documentation set under `docs/migration/`.

## [7.3.1] - 2019-11-19 (140ba2d2)

- Adopted LibFuzzer to feed C programs with random input, quickly exposing "shallow" bugs that don't require complex data.
- Implemented a new runtime library and instrumentation approach to monitor crashes, failing built-in assertions, and pointer safety.
- Adopted Crab-LLVM to infer invariants.
- Combined LibFuzzer and KLEE sequentially to check safety properties in a novel way.
- Adopted MetaSMT as a wrapper around additional SMT solvers (Boolector, Yices) previously unsupported by the tool.
- Fixed several bugs.

## [7.1] - 2017-11-17

- Added witness generation for `true` verdicts.
- Improved handling of nondeterministic ("nondet") functions.
- Added support files for SV-COMP'18 — see [further details](https://link.springer.com/chapter/10.1007/978-3-319-89963-3_28).
- General improvements to memory address tracking.
- Fixed several bugs.

## [7.0] - 2017-09-17

- Adopted Clang for parsing C code.
- Adopted LLVM IR as the intermediate representation for code instrumentation.
- Adopted KLEE for symbolic execution.
- Added a Dockerfile to build Map2Check.
- Fixed bugs related to counterexample generation.
- General improvements to memory address tracking.

## [6.0] - 2016-01-06

- Minor tool improvements — see [further details](https://link.springer.com/chapter/10.1007/978-3-662-49674-9_64).

## [5.0] - 2014-11-14

- First public release of Map2Check.
