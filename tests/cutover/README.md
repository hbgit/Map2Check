# Cutover regression baseline (C runtime)

Reference data for the Map2Check C -> Rust runtime cutover. Produced by
`scripts/cutover/run-corpus.sh`, consumed by the Rust comparison (Task 4.1).

- Map2Check commit: `f01e8c3ee66a2d54d91805f87443eaa57b598414`
- Image: `map2check-dev:latest`, ID `sha256:adbb5ec2824d3c3387781c0b6d45ffe365313335bd702dc10a4ae56d097d2b9c`
- CPU: 12th Gen Intel(R) Core(TM) i7-12650H
- The image lacks `/usr/bin/time`; the container installs the `time` package
  (`apt-get install -y time`) before the build. The runner needs it.
- Configure flags (as in CI): `-G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm -DMAP2CHECK_DYNAMIC_LINK=ON -DCMAKE_INSTALL_PREFIX=/src/release`;
  the installed tree needs `lib/klee/runtime` and `lib/clang` symlinks (CI does the same).
- `MAP2CHECK_PATH=/src/release` is exported for the wrapper.

## Full baseline command

```sh
docker run --rm -u root -v "$PWD":/src -w /src map2check-dev:latest bash -c '
  apt-get update -qq && apt-get install -y -qq time >/dev/null &&
  cmake -S . -B build -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm -DMAP2CHECK_DYNAMIC_LINK=ON -DCMAKE_INSTALL_PREFIX=/src/release &&
  cmake --build build && cmake --install build &&
  mkdir -p release/lib/klee &&
  ln -sfn /opt/klee/lib/klee/runtime release/lib/klee/runtime &&
  ln -sfn /usr/lib/llvm-16/lib/clang release/lib/clang &&
  GENS="symex" REPS=5 TIMEOUT=45 MAP2CHECK_PATH=/src/release \
    scripts/cutover/run-corpus.sh release tests/cutover/baseline-c.tsv'
```

Runner self-test: `bash scripts/cutover/test-run-corpus.sh`.

## Known issues found during the smoke run (read before running the full baseline)

- The runner argument is `release` (the dir holding the `map2check` wrapper), not `release/bin`.
- The C binary has no `fuzzer` generator; valid values are `afl` and `symex`. `GENS=fuzzer`
  gives "Selected generator don't exist" (runner records TIMEOUT). `afl` needs
  `/usr/local/bin/afl-clang-fast`, absent from this image (runner records UNKNOWN).
  Only `symex` works in `map2check-dev:latest`, so the command above uses `GENS="symex"`.
- The C pipeline chdirs into a scratch dir and removes it, so `map2check_property` is never
  left in the runner's cwd: tag/line/function columns are `-` for C. They are printed on stdout
  ("Violated property: file ... line N function F" then "TAG: ...") if Task 4.1 needs them.
- Smoke (symex, REPS=1): 11 FAILED, 4 SUCCEEDED, 26 UNKNOWN (41 rows, ~1 s each; KLEE
  "Forcing timeout" yields UNKNOWN).
