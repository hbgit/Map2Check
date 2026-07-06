# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What is Map2Check

Map2Check is a bug-hunting tool that automatically generates and checks safety properties in C programs. It tracks memory pointers and variable assignments to check user-specified assertions, overflow, and pointer safety. It uses LLVM 16, LibFuzzer, and KLEE 3.1 for test case generation.

## Build System

The project uses **CMake 3.20+ + Ninja** and **C++17**. The intended build environment is the Docker image defined in [Dockerfile.dev](Dockerfile.dev) (Ubuntu 22.04 + LLVM 16 + KLEE 3.1).

> The root [Dockerfile](Dockerfile) and the `make-release.sh` / `make-unit-test.sh` scripts are leftovers from the LLVM 6.0 era (hardcoded to `/llvm/release/llvm600`) and do not work with the current toolchain — use the steps below instead.

### Build Map2Check (inside Docker)

```sh
git clone https://github.com/hbgit/Map2Check.git
cd Map2Check
docker build -t map2check-dev --no-cache -f Dockerfile.dev .
docker run -it --rm -v $(pwd):/workspace map2check-dev /bin/bash
```

Inside the container:

```sh
mkdir build && cd build
cmake .. -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm
ninja && ninja install
# binary at release/bin/map2check
```

`Dockerfile.dev` already builds and installs KLEE 3.1 (to `/opt/klee`) and provides LibFuzzer via LLVM 16's compiler-rt — do **not** pass `-DSKIP_KLEE=ON` or `-DSKIP_LIB_FUZZER=ON` unless you deliberately want a build without KLEE/LibFuzzer support.

### Manual CMake build (if LLVM 16 is locally available, e.g. via apt.llvm.org)

```sh
export LLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm
export CXX=/usr/bin/clang++-16
export CC=/usr/bin/clang-16
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
| `MAP2CHECK_ENABLE_SANITIZERS` | OFF | Enable ASan + UBSan (forces dynamic linking) |

Enabling sanitizers switches from static to shared linking and enables `-fsanitize=address,undefined -fno-omit-frame-pointer -g`.

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

### CI

GitHub Actions ([.github/workflows/ci.yml](.github/workflows/ci.yml)) runs on every push/PR against LLVM 16: build + unit tests, static analysis (`clang-tidy`, `cppcheck`), and ASan/UBSan sanitizer tests. [docker-publish.yml](.github/workflows/docker-publish.yml) publishes the `Dockerfile.dev` image on push to `develop`.

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

`caller.hpp` uses `<filesystem>` (C++17 std) — do not reintroduce `boost/filesystem`.

### 2. Pass-Backend ([modules/backend/pass/](modules/backend/pass/))

Contains all LLVM passes (New Pass Manager, opaque pointers) that instrument the program under analysis:

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
