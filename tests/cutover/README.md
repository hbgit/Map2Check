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
  GENS="afl symex" REPS=5 TIMEOUT=45 MAP2CHECK_PATH=/src/release \
    scripts/cutover/run-corpus.sh release tests/cutover/baseline-c.tsv'
```

Runner self-test: `bash scripts/cutover/test-run-corpus.sh`.

## Notes on the C runtime (found in the smoke run)

- The runner argument is `release` (the dir holding the `map2check` wrapper), not `release/bin`.
- Valid generators are `afl` and `symex` (no `fuzzer`). `afl` needs `/usr/local/bin/afl-clang-fast`,
  absent from `map2check-dev:latest`; a rebuilt image (`map2check-dev:cutover-f01e8c3e`) adds AFL++.
  With the old image only `GENS=symex` works (afl rows come out UNKNOWN in 0.08 s).
- The C pipeline deletes its scratch dir, so `map2check_property` is not left in cwd. The runner
  derives tag/line/function from stdout (fixtures in `scripts/cutover/fixtures/`); FALSE-MEMTRACK
  and FALSE-MEMCLEANUP carry no line/function (`-`). Witness is 1 only if `--generate-witness`
  output is seen (the runner does not pass it).
- The runner passes `--timeout $TIMEOUT` to map2check (default is 0 = unlimited-ish and caused
  premature "Forcing timeout" UNKNOWNs); the outer `timeout` is TIMEOUT+15.
- Smoke (symex, REPS=1, 41 rows): 13 FAILED, 8 SUCCEEDED, 20 UNKNOWN.

## Baseline completa do runtime C (`baseline-c.tsv`)

- Gerada em 2026-10-15 sobre `develop` `f01e8c3e` com `GENS="afl symex" REPS=5 TIMEOUT=45`.
- Imagem: `map2check-dev:cutover-f01e8c3e` (`sha256:0e5ba2669491ccf6e762d8c25af9218ffc71e53e769700fc435d21b8bf92280d`), reconstruída do `Dockerfile.dev` de `develop` (contém AFL++ 4.40c). Pacote `time` instalado em tempo de execução. Build em `build-afl/`, instalação em `release-afl/` (`MAP2CHECK_PATH=/src/release-afl`).
- 410 linhas (41 tarefas×propriedade × 2 geradores × 5 repetições), todas com 11 colunas.
- Vereditos: `afl` 55 FAILED / 150 UNKNOWN; `symex` 65 FAILED / 40 SUCCEEDED / 100 UNKNOWN. O AFL++ não prova TRUE: nenhum SUCCEEDED com `afl` é esperado.
- Tags: 40 FALSE-DEREF, 40 FALSE-FREE, 20 FALSE-MEMTRACK, 20 TARGET-REACHED; nenhuma UNMAPPED.
- Esta baseline é a referência de estado conhecido do C. O gate da Task 4.2 a regenera no merge-base da branch de migração.
