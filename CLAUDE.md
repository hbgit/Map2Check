# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What is Map2Check

Map2Check is a bug-hunting tool that automatically generates and checks safety properties in C programs. It tracks memory pointers and variable assignments to check user-specified assertions, overflow, and pointer safety. It uses LLVM 6.0, LibFuzzer, and KLEE for test case generation, and Crab-LLVM for computing inductive invariants.

## Build System

The project uses **CMake + Ninja** inside Docker. The intended build environment is the Docker image defined in [Dockerfile](Dockerfile).

### Build the release (inside Docker)

```sh
docker build -t hrocha/mapdevel --no-cache -f Dockerfile .
docker run --rm -v $(pwd):/home/map2check/devel_tool/mygitclone:Z --user $(id -u):$(id -g) hrocha/mapdevel /bin/bash -c "cd /home/map2check/devel_tool/mygitclone; ./make-release.sh; ./make-unit-test.sh"
```

### Manual CMake build (if LLVM 6.0 is locally available)

```sh
export LLVM_DIR=/llvm/release/llvm600/lib/cmake/llvm
export CXX=/llvm/release/llvm600/bin/clang++
export CC=/llvm/release/llvm600/bin/clang
mkdir build && cd build
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DSKIP_LIB_FUZZER=ON -DSKIP_KLEE=ON
ninja && ninja install
```

### Key CMake flags

| Flag | Default | Purpose |
|------|---------|---------|
| `SKIP_LIB_FUZZER` | OFF | Skip building LibFuzzer |
| `SKIP_KLEE` | OFF | Skip building KLEE/Z3/STP/MiniSat |
| `ENABLE_TEST` | OFF | Build GTest unit tests |
| `REGRESSION` | OFF | Download regression test benchmarks |

### Run unit tests

```sh
cd build
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DSKIP_LIB_FUZZER=ON -DSKIP_KLEE=ON -DENABLE_TEST=ON
ninja && ninja install && ctest
```

### Run regression tests

Requires the `hrocha/benchexecrun` Docker image (from [utils/benchexecrun](utils/benchexecrun/)):

```sh
./make-regression-test.sh t   # small Travis set
./make-regression-test.sh m   # full map2check set
./make-regression-test.sh s <xml_path>  # SV-COMP set
```

## Code Style

- **C++ code**: Google C++ style — run `clang-format` with the [.clang-format](.clang-format) file.
- **C code**: LLVM style.
- Style check script: `./check_code_style.py -p` (C++ Google style) or `-c` (C LLVM style).

## Architecture

Map2Check has three main modules under [modules/](modules/):

### 1. Frontend ([modules/frontend/](modules/frontend/))

Entry point: `map2check.cpp` → `main()`. Parses CLI options (via Boost.ProgramOptions) and drives the whole pipeline.

`Caller` class ([caller.hpp](modules/frontend/caller.hpp)) orchestrates the verification pipeline:
1. `compileCFile()` — compile the input C file to LLVM IR via clang
2. `callPass()` — apply the appropriate LLVM pass (instrumentation)
3. `linkLLVM()` — link instrumented IR with the library backend
4. `applyNonDetGenerator()` — invoke LibFuzzer or KLEE to generate inputs
5. `executeAnalysis()` — run the instrumented binary; collect results
6. Witness/counterexample generation in [counter_example/](modules/frontend/counter_example/) and [witness/](modules/frontend/witness/)

**Verification modes** (enum `Map2CheckMode`): `MEMTRACK_MODE`, `REACHABILITY_MODE`, `OVERFLOW_MODE`, `ASSERT_MODE`, `MEMCLEANUP_MODE`.

To add a new analysis: extend `callPass()` in [caller.cpp](modules/frontend/caller.cpp) and add a CLI option in [map2check.cpp](modules/frontend/map2check.cpp).

### 2. Pass-Backend ([modules/backend/pass/](modules/backend/pass/))

Contains all LLVM passes that instrument the program under analysis:

- `MemoryTrackPass` — instruments memory allocation/deallocation
- `AssertPass` — instruments `__VERIFIER_assert` calls
- `OverflowPass` — instruments signed integer arithmetic
- `NonDetPass` — instruments nondeterministic input functions
- `TrackBasicBlockPass` — tracks execution of basic blocks
- `TargetPass` — marks reachability targets
- `LoopPredAssumePass`, `GenerateAutomataTruePass` — automata/loop support

### 3. Library-Backend ([modules/backend/library/](modules/backend/library/))

C library (not C++) that is linked into the instrumented program at runtime. It implements the functions called by the LLVM passes.

Key API: [Map2CheckFunctions.h](modules/backend/library/header/Map2CheckFunctions.h).

To add a new analysis mode: implement the interface in [AnalysisMode.h](modules/backend/library/header/AnalysisMode.h) and add a new `AnalysisMode<Name>.c` file alongside the existing ones.

**NonDet generators** are selected at link time: `NonDetGeneratorNone.c`, `NonDetGeneratorKlee.c`, `NonDetGeneratorLibFuzzy.c`.

## Submodule Note

After cloning, initialize submodules before building:

```sh
git submodule update --init --recursive
```
