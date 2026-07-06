# Changelog

All notable changes to Map2Check are documented in this file.
The format loosely follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

### Changed

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
