# Gate diferencial (Task 4.2) e memória D3/D4 (Task 4.3), execução 1

**Result: `gate` FAIL, both without and with the proposed accepted deltas.** The wall-time median passes, the TIMEOUT counts pass, and no Rust defect was found. Two kinds of rows still block:

- C defects that need a new SD;
- the median RSS ratio (1.36 against the 1.25 limit).

## Setup

### Rust side

- Map2Check-rust `7cc8ac7a` (the runner files are from `20e0572b`, which does not change the binary).
- map2check-library `44585e8` (cutover/runtime-rust), built from a `git archive` with a fresh `CARGO_TARGET_DIR`:
  - `cargo build --profile runtime --features libfuzzer,wasm`;
  - `scripts/build-klee-runtime.sh` (rustc 1.72.1, LLVM 16.0.5).
- Installed into fresh `build-gate2/` and `release-gate2/` with `-G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm -DMAP2CHECK_DYNAMIC_LINK=ON -DMAP2CHECK_RUST_LIB_DIR=… -DMAP2CHECK_RUST_KLEE_BC=…`, plus the klee/clang symlinks. `cmp` confirms that the installed `.a` and `.bc` are the ones built.
- `release-gate2/map2check` mtime: 2026-10-16 19:05:06 -0400.
- sha256 prefixes: `map2check-klee.bc` 1f5edca0756922dc, `libmap2check.a` fda95ed02de3954a.
- Image: `map2check-dev:cutover-klee-rust-time` (`sha256:ef46e184…`), which is `cutover-klee-rust` plus GNU `time`.

### C side

- `Map2Check-cutover/release-afl`, built from `b3c10cb7` (develop `f01e8c3e` plus the runner commits, which is the base of feat/rust-runtime). Binary mtime: 2026-10-15 19:33:45 -0400.
- Image: `map2check-dev:cutover-f01e8c3e-time` (`sha256:48a8ed09…`), which is `cutover-f01e8c3e` plus GNU `time`.

### Corpus and settings

- Corpus: `tests/regression_test/test_cases/sv-benchmarks`, with 41 task×property pairs.
- Settings: `GENS="afl symex" TIMEOUT=45`, REPS=5 for C and Rust, and REPS=3 for C `--btree`.
- The runs were sequential: Rust 19:08–20:12, C 20:12–21:17, C-btree 21:17–21:33.
- Before each run, `free -g` showed 4 GB or more available.

### Resource caps (the same on both sides)

- Every container ran with `--memory 3g --memory-swap 3g --cpus 4`.
- **KLEE `--max-memory=2000`:**
  - The frontend runs `${MAP2CHECK_PATH}/bin/klee …` and has no option for extra KLEE flags. It also resets `MAP2CHECK_PATH` from `/proc/self/exe`.
  - The cap is therefore applied through an overlay install tree, built per container. It contains a copy of `map2check`, symlinks to the rest of the install, and a `bin/klee` wrapper that runs `exec <real>/bin/klee --max-memory=2000 "$@"`.
  - The overlay script is `$S/t42b/overlay.sh`.
  - The wrapper was confirmed on both sides through a logged argv.
- The caps change the absolute numbers. They do not change the fairness of the comparison.
- **OOM detection:** `run-corpus.sh` (`20e0572b`) compares `oom_kill` in the container's `memory.events` before and after each task. A new kill without a decided verdict is recorded as verdict `OOM`.

### Runner commits (signed, not pushed)

- `8cf38ce9`: `EXTRA_FLAGS`.
- `20e0572b`: OOM verdict.

## compare.py output

```
$ python3 scripts/cutover/compare.py tests/cutover/baseline-c-gate.tsv tests/cutover/baseline-rust.tsv --accepted tests/cutover/deltas-aceitos.tsv
median wall ratio: 1.015  median rss ratio: 1.360

| key | field | c | rust |
|---|---|---|---|
| array-examples/standard_sentinel-2.yml unreach-call afl | verdict | OOM | UNKNOWN |
| array-examples/standard_two_index_04.yml unreach-call afl | verdict | OOM | UNKNOWN |
| array-examples/standard_two_index_05.yml unreach-call afl | verdict | OOM | UNKNOWN |
| ldv-memsafety/ArraysOfVariableLength.yml valid-memsafety symex | verdict | UNKNOWN | SUCCEEDED |
| list-ext-properties/960521-1_1-1.yml valid-memsafety symex | line | 0 | 533 |
| list-ext-properties/960521-1_1-1.yml valid-memsafety symex | function | - | main |
| list-ext-properties/list-ext.yml unreach-call afl | verdict | UNKNOWN | FAILED |
| list-ext-properties/list-ext.yml unreach-call afl | tag | - | TARGET-REACHED |
| list-ext-properties/list-ext.yml unreach-call afl | line | - | 556 |
| list-ext-properties/list-ext.yml unreach-call afl | function | - | main |
| list-ext-properties/simple-ext.yml unreach-call symex | verdict | UNKNOWN | FAILED |
| list-ext-properties/simple-ext.yml unreach-call symex | tag | - | TARGET-REACHED |
| list-ext-properties/simple-ext.yml unreach-call symex | line | - | 540 |
| list-ext-properties/simple-ext.yml unreach-call symex | function | - | main |
| map2check-assert/ex01_false.yml unreach-call symex | verdict | UNKNOWN | FAILED |
| map2check-assert/ex01_false.yml unreach-call symex | tag | - | TARGET-REACHED |
| map2check-assert/ex01_false.yml unreach-call symex | line | - | 5 |
| map2check-assert/ex01_false.yml unreach-call symex | function | - | __VERIFIER_assert |
| memsafety/960521-1-2.yml valid-memsafety symex | verdict | UNKNOWN | SUCCEEDED |
| all | median rss ratio | <= 1.25 | 1.360 |
```

With `--accepted tests/cutover/deltas-aceitos-PROPOSTA.tsv`, the four verdict-gain rows and the four list-ext afl rows drop out. The following still fail:

- the 3 OOM rows;
- the 2 rows for 960521-1_1-1;
- the 8 rows for simple-ext and ex01_false;
- the RSS median.

The result is still **FAIL**.

## Verdict counts (mode of the reps per task)

| gen | side | FAILED | SUCCEEDED | UNKNOWN | OOM | TIMEOUT | correct vs `expected_verdict` |
|---|---|---|---|---|---|---|---|
| afl | C | 11 | 0 | 27 | 3 | 0 | 11 |
| afl | Rust | 12 | 0 | 29 | 0 | 0 | 12 |
| symex | C | 13 | 8 | 20 | 0 | 0 | 20 |
| symex | Rust | 15 | 10 | 16 | 0 | 0 | 24 |

- No row went from SUCCEEDED to FAILED or back, and no tag changed between rows that are FAILED on both sides.
- The C reps never disagree on any field (they are fully stable), and the Rust reps don't either. So there are no (d) rows.
- Criterion 4 holds: Rust has 36 correct verdicts against 31 for C.

## Classification of every failing row

| # | task / prp / gen | C → Rust | class | diagnosis | proposed |
|---|---|---|---|---|---|
| 1 | ArraysOfVariableLength / memsafety / symex | UNKNOWN → SUCCEEDED (expected true) | **(a)** | C stops on KLEE's 31 s `--max-time` (31.35 s, 5/5). Rust finishes the exploration in 22.6 s, thanks to the 2.7b throughput gain. | GANHO-ORCAMENTO |
| 2 | 960521-1-2 / memsafety / symex | UNKNOWN → SUCCEEDED (expected true) | **(a)** | Same: C at 31.35 s, the budget; Rust at 31.28 s. | GANHO-ORCAMENTO |
| 3 | list-ext / unreach / afl (4 fields) | UNKNOWN → FAILED TARGET-REACHED line 556 main (expected false) | **(b)** | Known UB in the benchmark (task-3.3 "Cause A"). The tail node's `n` is never initialised. In C, glibc reuses a chunk the runtime freed, so the seed crashes on a garbage dereference, the replay misses the target, and the result is UNKNOWN. In Rust the word is 0, and CmpLog finds the target. | UB-BENCHMARK-list-ext |
| 4 | simple-ext / unreach / symex (4 fields) | UNKNOWN → FAILED TARGET-REACHED line 540 main (expected false) | **C defect** (not a/b/c/d) | Reproduced by hand, see "C property-file race" below. C's KLEE does reach `__VERIFIER_error` (line 539, `abort.err`), but `map2check_property` ends up **0 bytes**, so the frontend says UNKNOWN in 0.45 s. This is not a budget effect. | no: needs an SD ("bug do C corrigido") |
| 5 | ex01_false / unreach / symex (4 fields) | UNKNOWN → FAILED TARGET-REACHED line 5 `__VERIFIER_assert` (expected false) | **C defect** | Same race. KLEE `abort.err` at line 4 or 5, 199 ktests, `map2check_property` 0 bytes, UNKNOWN in 2.6 s. | no: needs an SD |
| 6 | 960521-1_1-1 / memsafety / symex (line, function) | `0`/`-` → `533`/`main` | **C defect** | Same race. Both `map2check_property` and `map2check_property_deref` are 0 bytes. The frontend still prints FALSE-DEREF, because the `_deref` file exists, but it parses line 0 and an empty function. C afl on the same task reports `533 main`, the same as Rust. | no: needs an SD |
| 7 | standard_sentinel-2 / unreach / afl | OOM → UNKNOWN (expected true) | **C defect (memory)** | C hits the 3 GB cap in 15 of 15 reps (`oom_kill` seen; maxrss about 3.1 GB). Without the cap, the committed `baseline-c.tsv` shows **12.8 GB**, with UNKNOWN. Rust uses 341 MB. The program reads 100 000 `__VERIFIER_nondet_int`. Hypothesis, not profiled: per-call growth of the C NonDetLog under AFL. | no: needs an SD, or a ruling that a C OOM is equivalent to UNKNOWN |
| 8 | standard_two_index_04 / unreach / afl | OOM → UNKNOWN | C defect (memory) | Same as #7 (`a[100000]`, `b[100000]`). | no |
| 9 | standard_two_index_05 / unreach / afl | OOM → UNKNOWN | C defect (memory) | Same as #7. | no |
| 10 | all / median RSS ratio | 1.360 > 1.25 | D4 (criterion 8) | See "Performance". The excess is a fixed overhead under symex, about +80 MB. | not a delta row |

- **(c) Rust or pass defects: 0.**
- **(d) C nondeterminism: 0.**

### C property-file race (rows 4–6)

How the cases were reproduced:
- `map2check --debug --timeout 45 --nondet-generator symex …` with `release-afl`, under the same caps.
- The kept dirs are in `$S/t42b/repro/{c-simple-ext,c-ex01,c-960}`.

What the C runtime does:
- `write_property` (`PropertyGenerator.c:50`) does `fopen("map2check_property","w")`, then `fprintf`, then `fclose`.
- Under KLEE with `--external-calls=all`, every state shares the host file. A state that later opens the file with `"w"` (O_TRUNC) truncates the violating state's content.
- `--exit-on-error-type=Abort` then halts KLEE before that state writes anything.

Evidence:
- `map2check_checked_error` (12 bytes) and `abort.err` are present;
- the property files are 0 bytes.

This is the same last-writer race the Rust runtime had (task-3.3 "Cause B"). Rust fixed it in `6b49b82` (temp file + `rename`). In this window the C race is deterministic: 5 of 5 reps on each task.

## Performance (criteria 6–9)

| gen | n | median wall ratio Rust/C | median RSS ratio | Σ median wall C | Σ median wall Rust | TIMEOUT C | TIMEOUT Rust |
|---|---|---|---|---|---|---|---|
| afl | 41 | 1.003 | 0.987 | 165.1 s | 167.5 s | 0 | 0 |
| symex | 41 | 1.091 | 1.582 | 617.0 s | 599.7 s | 0 | 0 |
| all | 82 | **1.015** | **1.360** | 782.2 s | 767.2 s | 0 | 0 |

- **Criterion 6 PASSES:** 1.015 ≤ 1.10. symex alone is 1.091.
- **Criterion 7 PASSES:** TIMEOUT is 0 on both sides for each generator. C additionally has 3 afl OOM rows.
- **Criterion 8 FAILS: 1.360 > 1.25.**
  - Under symex, Rust uses about 80 MB more than C at the median (about 200–230 MB against 127–170 MB), and the excess is nearly constant across tasks.
  - Hypothesis, not profiled: KLEE loads and `--optimize`s a 2.2 MB `map2check-klee.bc`, while the C runtime's `.bc` files total 0.13 MB.
  - afl is neutral (0.987). Some afl rows sit at a plateau of about 341 MB on both sides (for example C 960521-1_1-2, test-0158); those are AFL++ process peaks, not runtime state.
- **Criterion 9:** 18 rows have a wall ratio above 1.5. Each needs a profile in `docs/cutover/perf.md`.
  - 16 are symex runs where C finishes in under 2 s and Rust takes about 3.1–3.4 s. The median extra is +2.4 s on short symex tasks, which looks like a fixed start-up cost with the same probable cause as the RSS excess.
  - The other two are `check_if_bug` and `ex02_false` (afl, 0.33 s → 1.45 s).
  - Largest RSS outlier: split_list_test05-1 symex, 243 → 472 MB (1.94). The largest Rust peak in the whole run is 476 MB, and Rust had no OOM.

## D3 (`--btree`) and D4 (RSS)

valid-memsafety: 30 tasks × 2 generators. C-array and Rust have 5 reps, C-btree has 3. Peaks are medians.

**Local corpus note.** No "5 largest MemSafety-Arrays.set tasks" set is available locally. `MemSafety-Arrays.set` (`array-memsafety/*.yml`, `array-examples/*.yml`) matches exactly 5 memsafety tasks in the corpus: relax-1 (621 lines), insertionsort-alloca-1, array01-alloca-2, lis_unsafe and add_last_unsafe. They are included in the 30.

| gen | median C-array | median C-btree | median Rust | tasks where Rust > C-btree | max Rust/C-btree | median Rust/C-array |
|---|---|---|---|---|---|---|
| afl | 93.5 MB | 94.3 MB | 92.3 MB | 8 / 30 (the 341 MB AFL plateau) | 3.64 | 0.987 |
| symex | 139.6 MB | 136.3 MB | 221.2 MB | 30 / 30 | 3.17 (split_list_test05-1: 149 vs 472 MB) | 1.597 |

The 5 MemSafety-Arrays tasks, symex (C-array / C-btree / Rust):

| task | C-array | C-btree | Rust |
|---|---|---|---|
| relax-1 | 178 MB | 151 MB | 227 MB |
| add_last_unsafe | 169 MB | 136 MB | 217 MB |
| array01-alloca-2 | 169 MB | 137 MB | 239 MB |
| insertionsort-alloca-1 | 144 MB | 137 MB | 227 MB |
| lis_unsafe | 170 MB | 140 MB | 227 MB |

**C `--btree` changes verdicts and is unsound.** Under symex it gives **3 wrong SUCCEEDED results on tasks whose expected verdict is false**: lis_unsafe, memleaks_test1-1 and memleaks_test17_2-1. It also loses list-ext (FAILED → UNKNOWN, on both generators). Correct results: afl 8 vs 9 for C-array; symex 22 correct plus 3 wrong vs 18 correct plus 0 wrong for C-array. Rust: 20 correct, 0 wrong.

**Recommendation**

- **D3: do not port `--btree`. Make it a no-op that prints a warning in the Rust frontend.**
  - The literal Task 2.6 condition fails: Rust's peak is above C-btree on all 30 symex tasks. But that excess is a fixed overhead of about 80 MB, and the largest peak is 472 MB, far from any limit.
  - The C-btree baseline is not a valid target: it is unsound on 3 tasks.
  - The maintainer should approve this deviation from the literal condition explicitly.
- **D4: do not approve 1.25 as a median ratio as it stands.** The symex median of 1.58 comes from a constant overhead, not from growth with the program's state.
  - Option 1: reduce the overhead first. A smaller or stripped `map2check-klee.bc` would act on start-up and RSS together, and then the gate is re-measured.
  - Option 2: write D4 per generator: afl ratio ≤ 1.25, plus an absolute symex bound, for example a median excess of 100 MB or less.

## Proposal

- `tests/cutover/deltas-aceitos-PROPOSTA.tsv` covers only classes (a) and (b).
- It must not be merged into `deltas-aceitos.tsv` without the maintainer's approval.
- The C defects in rows 4–9 need new `SD-xx` entries ("bug do C corrigido") in `docs/semantic-deltas.md` before they can be accepted. The reproductions are given above.

`$S` = `/tmp/claude-1000/-home-rock-Documents-Dev-map2check-library/561b38ab-d0f9-4bb7-a72d-b85b8d5834c3/scratchpad`. It holds the logs, the overlay script, the analysis script (`t42/analyse.py`) and the reproduction dirs.

## Decisões do mantenedor (2026-10-16)

1. **Deltas: "Aceitar tudo".**
   - As 6 linhas da PROPOSTA (GANHO-ORCAMENTO e UB-BENCHMARK-list-ext) foram aprovadas.
   - Foram acrescentadas 13 linhas por bugs do runtime C:
     - `C-BUG-property-race` (10 linhas): o arquivo `map2check_property` compartilhado é truncado por outros estados do KLEE, e o frontend do C dá UNKNOWN (simple-ext, ex01_false) ou lê "Line 0"/função vazia (960521-1_1-1). Os campos verdict/tag/line/function de simple-ext e ex01_false e line/function de 960521-1_1-1.
     - `C-BUG-oom-afl-nondet` (3 linhas): o C estoura os 3 GB nas três tarefas `standard_*` em afl (Rust, cerca de 341 MB). A causa (crescimento do NonDetLog por chamada) é hipótese, não perfilada.
   - Todas as divergências de campo listadas neste relatório ficaram cobertas. Nenhuma ficou sem causa. As entradas correspondentes em `docs/semantic-deltas.md` (map2check-library) ainda precisam ser escritas.
   - `deltas-aceitos-PROPOSTA.tsv` foi removido; `deltas-aceitos.tsv` tem 19 linhas de dados.
2. **D3:** `--btree` não é portado; continua um no-op com aviso. O `--btree` do C dá SUCCEEDED errado em lis_unsafe, memleaks_test1-1 e memleaks_test17_2-1, então não é referência válida. O mantenedor aprovou se afastar da condição literal da Task 2.6.
3. **D4:** aceita a razão mediana medida. O limite do critério 8 passa a ser **1,36** (medido 1,36; afl 0,99, symex 1,58). `compare.py` usa `--max-rss` 1.36 por padrão. Causa (KLEE com `.bc` do runtime de 2,2 MB contra 0,13 MB): hipótese, não perfilada.
4. **Critério 9 continua ABERTO:** as 18 linhas com razão de tempo de parede acima de 1,5 ainda precisam de perfil em `docs/cutover/perf.md`. Não foi feito.

### Novo veredito

```
$ python3 scripts/cutover/compare.py tests/cutover/baseline-c-gate.tsv tests/cutover/baseline-rust.tsv --accepted tests/cutover/deltas-aceitos.tsv
median wall ratio: 1.015  median rss ratio: 1.360

gate: PASS
```

O `gate` (critérios 1–8) passa. O critério 9 segue pendente. `test_compare.py`: 6 de 6 testes passam.

## Critério 9 — perfis

Gate run 1 has 18 rows with a wall ratio (Rust/C, medians of 5 reps) above 1.5: **15 symex and 3 afl**. The earlier count of "16 symex + 2 afl" in this report was wrong, because list-ext afl is at 1.56. Every row falls into one of three cause groups.

### How the profiles were taken

- **Same images, installs and caps as the gate:**
  - `release-gate2` in `cutover-klee-rust-time`;
  - `release-afl` in `cutover-f01e8c3e-time`;
  - `--memory 3g --memory-swap 3g --cpus 4`, KLEE `--max-memory=2000`;
  - `free -g` showed 4 GB or more available before each container.
- **Phase times:** the overlay install wraps every `bin/*` tool (clang, opt, llvm-link, klee) with a timer, and the `--debug` output is timestamped line by line. The AFL++ phases are read from those timestamps.
- **KLEE breakdown:**
  - `klee-stats --to-csv` on `klee-out-0`. The `UserTime` of the first `run.stats` row is the CPU spent before the first instruction runs; the final row gives the exploration `WallTime` and `SolverTime`.
  - KLEE re-run alone on the kept `-witness-result.bc` with flags removed one at a time.
  - Host `perf record -g -p` on KLEE inside the container (run as the host uid), with symbols from the image's `libLLVM-16.so.1` via `--symfs`.
- **Scripts:** `$S/c9/` (`prof.sh`, `kvar2.sh`, `dropbt.sh`, `rssbt.sh`, `perfk.sh`, `ksum.sh`).
- **Kept run directories:** `$S/c9/runs/` and `$S/c9/runs2/`.

### Group A: KLEE start-up on the 2.3 MB runtime module (15 symex rows)

**Measured.**
- **What it is:** a fixed cost of about 2.6–2.7 s per KLEE run, before any instruction executes.
- **What it is not:** path count, the solver, or per-path cost in the runtime.
- **The C rows that take longer** (1–2.6 s) show a smaller delta, because Rust's exploration is faster there (the 2.7b gain) and absorbs part of the fixed cost.

KLEE profiles (`klee` = wall time of the KLEE process; `pre_exec` = `UserTime` before the first instruction):

| run | side | klee s | pre_exec s | exploration wall s | solver s | instructions | paths |
|---|---|---|---|---|---|---|---|
| byte_add_bug | Rust | 2.93 | 2.63 | 0.11 | 0.00 | 46 186 | 1 |
| byte_add_bug | C | 0.29 | 0.22 | 0.03 | 0.00 | 19 658 | 1 |
| test-bitfields-3.1-2 | Rust | 3.17 | 2.88 | 0.12 | 0.00 | 34 356 | 1 |
| test-bitfields-3.1-2 | C | 0.31 | 0.23 | 0.03 | 0.00 | 30 180 | 1 |
| 960521-1_1-3 | Rust | 3.35 | 2.85 | 0.35 | 0.07 | 471 187 | 15 |
| 960521-1_1-3 | C | 1.67 | 0.23 | 1.40 | 0.09 | 3 457 247 | 19 |
| insertionsort-alloca-1 | Rust | 3.35 | 2.90 | 0.29 | 0.05 | 406 655 | 4 |
| insertionsort-alloca-1 | C | 1.49 | 0.24 | 1.20 | 0.04 | 3 629 574 | 4 |
| ex01_false | Rust | 4.86 | 2.88 | 1.82 | 1.55 | 233 479 | 165 |
| ex01_false | C | 2.55 | 0.22 | 2.28 | 2.00 | 317 741 | 199 |

**Other pipeline phases:**
- clang, opt and the frontend take under 0.1 s on both sides.
- Rust's single `llvm-link` with `map2check-klee.bc` takes 0.18–0.19 s, against 2 × 0.015 s for C's links of the C `.bc` files.

**Where the start-up goes** (byte_add_bug, KLEE alone on the kept module, 2 runs each):

| variant | Rust s | C s |
|---|---|---|
| as in the gate (`--optimize`) | 3.14 | 0.29 |
| without `--optimize` | 1.22 | 0.27 |
| without `--optimize`, `--max-instructions=1` | 1.12 | 0.25 |
| `--optimize`, without `assembly.ll` and `run.istats` | 3.02 | 0.28 |
| `--optimize`, debug info stripped from the whole module | 3.07–3.14 | – |
| `--optimize`, internalize + globaldce (2.05 MB) | 2.95–3.07 | – |
| `--optimize`, **std backtrace symbolizer bodies deleted** (1.44 MB) | **1.47–1.53** | – |
| same, without `--optimize` | 0.75–0.77 | – |

- **`--optimize` is about 1.9 s.**
  - The `perf` profile is 81.5% in `libLLVM-16`.
  - The top symbols are LLVM optimization passes: `ScalarEvolution::getRangeRef`, `InstructionCombiningPass::runOnFunction`, `ValueHandleBase::AddToUseList`, `decomposeBitTestICmp` and `FoldingSet`.
  - KLEE re-optimizes a runtime that rustc already built at O3 with fat LTO.
- **Loading, linking with uclibc and module preparation are about 0.9 s.**
  - Without `--optimize`, 64% of the samples are in `libLLVM-16` (bitcode reader, `TypeFinder`, `IRMover`, `formatted_raw_ostream`) and 29% are in `ld-linux`.
- **What the module holds:**
  - The kept `-witness-result.bc` has 583 defined functions, 7 371 branches and an 11 MB `assembly.ll`. C's has 80 functions, 478 branches and 2.2 MB.
  - Of the runtime's 125 829 IR instructions, about 60 000 are std's backtrace symbolizer, pulled in by the default panic hook: gimli, addr2line and miniz_oxide (52 806) plus rustc_demangle (7 299).
  - The library itself is 30 896 instructions.
  - The symbolizer code never runs on a non-panicking path. With its bodies deleted, KLEE's instruction count is unchanged (45 184 against 45 176) and the result is the same.
- **RSS:** the same deletion lowers KLEE's peak RSS from 221 MB to 161 MB (C: 127 MB). That is about 60 of the 94 MB symex excess behind D4.

**Library defect: reported, not fixed.** See the task-4.3 criterion 9 report in map2check-library for the suggested fix.

### Group B: C skips AFL++ because its link fails; Rust runs it (2 afl rows)

**Measured.** Rows: check_if_bug and ex02_false (unreach-call, afl). Both sides say UNKNOWN.

**C** (0.36 s):
- All three `afl-clang-fast` links fail with `undefined reference to '__VERIFIER_assert'`. The C runtime does not define it.
- The frontend prints "the AFL++ binary is unavailable -- skipping the fuzzer phase" at 0.356 s and reports UNKNOWN.

**Rust** (1.48 s):
- The three AFL++ binaries build: 0.11 s → 0.45 s.
- `afl-fuzz` then runs for 1.03 s and exits with 256, on "The program took more than 1000 ms to process one of the initial test cases".
- The seed `AAAAAAAA` makes the loop bound `0x41414141`, so the dry run reaches AFL++'s 1 s timeout.

**Conclusion:** the extra 1.1 s is a fuzzing phase that C never reaches. It is not runtime cost. The C link failure is a C-side defect outside this gate's field comparison, because the verdicts are equal.

### Group C: a different verdict, from extra work that pays off (1 afl row)

**Measured.** Row: list-ext (unreach-call, afl): C UNKNOWN 0.53 s, Rust FAILED 0.79 s. This is the accepted `UB-BENCHMARK-list-ext` delta.

| phase | C | Rust |
|---|---|---|
| AFL++ builds | 0.28 s | 0.34 s |
| `afl-fuzz` | 0.08 s, exit 256 | 0.26 s, exit 0 (finds the target) |
| report | – | witness and replay |

- The +0.28 s is the work of reaching FAILED TARGET-REACHED.
- The RSS of 340 MB is the AFL++ process plateau already noted above.

### Per-row table

R−C is the difference of the median wall times. "profiled" marks a row with its own profile above; the other rows are assigned by the same signature.
- **Group A signature:** Rust at 3.1–3.4 s whatever C's time, and R−C between +1.6 and +2.9 s, falling as C's exploration time grows.
- **Group B signature:** both UNKNOWN, C at about 0.33 s.

| task | gen | ratio | cause group | evidence |
|---|---|---|---|---|
| ldv-memsafety-bitfields/test-bitfields-3.1-2 (memsafety) | symex | 9.17 | A: KLEE start-up | profiled: pre_exec 2.88 s vs 0.23 s; exploration 0.12 s vs 0.03 s; R−C +2.86 s |
| map2check-assert/byte_add_bug (unreach) | symex | 8.81 | A | profiled: pre_exec 2.63 s vs 0.22 s; 0 queries, 1 path; `perf`, flag ablation and symbolizer deletion done on this row; R−C +2.81 s |
| ldv-memsafety/memsetNonZero (memsafety) | symex | 8.38 | A | R−C +2.73 s; Rust 3.10 s |
| ldv-memsafety-bitfields/test-bitfields-1-1 (memsafety) | symex | 7.36 | A | R−C +2.67 s; Rust 3.09 s |
| list-ext-properties/simple-ext (unreach) | symex | 7.24 | A (verdict differs: C-BUG-property-race) | R−C +2.81 s; Rust 3.26 s; C stops at UNKNOWN in 0.45 s because of the race (SD-20) |
| ldv-memsafety-bitfields/test-bitfields-2.1-2 (memsafety) | symex | 6.62 | A | R−C +2.64 s; Rust 3.11 s |
| list-ext-properties/test-0158_1-1 (memsafety) | symex | 6.20 | A | R−C +2.65 s; Rust 3.16 s |
| list-ext-properties/test-0019_1-1 (memsafety) | symex | 5.46 | A | R−C +2.63 s; Rust 3.22 s |
| check_if_bug (unreach) | afl | 4.39 | B: C skips AFL++ | profiled: C link fails on `__VERIFIER_assert` at 0.36 s; Rust `afl-fuzz` dry-run timeout of 1.03 s; R−C +1.12 s |
| ex02_false (unreach) | afl | 4.24 | B | profiled: same as check_if_bug; R−C +1.10 s |
| list-ext-properties/simple-ext (memcleanup) | symex | 3.46 | A | R−C +2.36 s; Rust 3.32 s |
| memsafety/960521-1-3 (memsafety) | symex | 3.43 | A | R−C +2.38 s; Rust 3.36 s |
| memsafety/960521-1-1 (memsafety) | symex | 3.29 | A | R−C +2.29 s; Rust 3.29 s |
| list-ext-properties/960521-1_1-1 (memsafety) | symex | 2.12 | A | R−C +1.78 s; Rust 3.37 s; same program family as 960521-1_1-3 |
| list-ext-properties/960521-1_1-3 (memsafety) | symex | 2.01 | A | profiled: pre_exec 2.85 s vs 0.23 s; exploration 0.35 s vs 1.40 s (Rust 7× fewer instructions); R−C +1.73 s |
| array-memsafety/insertionsort-alloca-1 (memsafety) | symex | 1.97 | A | profiled: pre_exec 2.90 s vs 0.24 s; exploration 0.29 s vs 1.20 s; R−C +1.60 s |
| map2check-assert/ex01_false (unreach) | symex | 1.88 | A (verdict differs: C-BUG-property-race) | profiled: pre_exec 2.88 s vs 0.22 s; exploration 1.82 s vs 2.28 s, solver 1.55 s vs 2.00 s; R−C +2.27 s |
| list-ext-properties/list-ext (unreach) | afl | 1.56 | C: extra work for a better verdict | profiled: `afl-fuzz` 0.26 s (finds the target) vs 0.08 s; FAILED vs UNKNOWN (UB-BENCHMARK-list-ext); R−C +0.28 s |

**Totals:** A = 15 rows, B = 2, C = 1.
- No row shows a per-path or per-call cost in the runtime.
- No row shows solver time above C's.
- Every profiled exploration phase is as fast as C's or faster.

**Criterion 9:** criteria 6 and 7 pass, so none of the 18 rows blocks. All 18 now have a recorded cause. The plan names `docs/cutover/perf.md` as the place for these profiles. That file does not exist yet in either repository, so the profiles are recorded here.

## Re-execução symex com runtime KLEE reduzido (2026-10-16)

Only the Rust symex side was re-run. The runtime bitcode is now slimmer (std's backtrace symbolizer stubbed out). The afl rows and the C tables are unchanged.

### Setup

- map2check-library `94a17ec` (`ef166d1` + `94a17ec`), built from a `git archive` with a fresh `CARGO_TARGET_DIR`: `cargo build --profile runtime --features libfuzzer,wasm`, and `scripts/build-klee-runtime.sh` (rustc 1.72.1, LLVM 16.0.5) inside `map2check-dev:cutover-klee-rust`.
- Map2Check-rust `33e3b005`, installed into fresh `build-gate3/` and `release-gate3/` with the same CMake flags as `release-gate2`, pointing `MAP2CHECK_RUST_LIB_DIR` and `MAP2CHECK_RUST_KLEE_BC` at the new artifacts. `cmp` confirms the installed `.a` and `.bc` are the ones built.
- Installed `map2check-klee.bc`: 1 241 728 bytes (1.24 MB), against 2 296 020 bytes in `release-gate2`.
- Same image (`cutover-klee-rust-time`), same caps (`--memory 3g --memory-swap 3g --cpus 4`, KLEE `--max-memory=2000` through the overlay `bin/klee` wrapper, confirmed in the process list), `GENS=symex REPS=5 TIMEOUT=45`, and `free -g` showed 4 GB available before the container started.
- Output: `baseline-rust-symex-slim.tsv` (205 rows). `baseline-rust-v2.tsv` is the afl rows of `baseline-rust.tsv` plus these symex rows (410 rows).

### compare.py (C gate baseline vs v2, with `deltas-aceitos.tsv`)

```
median wall ratio: 1.011  median rss ratio: 1.081

gate: PASS
```

No field divergence outside the accepted deltas. Against the first run, no symex row changed verdict, tag, line, function or witness.

### Before and after (Rust/C)

| gen | median wall before → after | median RSS before → after | Σ median wall Rust (C = 617.0 s symex) | rows >1.5 wall | TIMEOUT / OOM |
|---|---|---|---|---|---|
| afl | 1.003 → 1.003 | 0.987 → 0.987 | 167.5 s (unchanged) | 3 → 3 | 0 / 0 |
| symex | 1.091 → 1.035 | 1.582 → 1.224 | 599.7 s → 523.0 s | 15 → 10 | 0 / 0 |
| all | 1.015 → 1.011 | 1.360 → 1.081 | 767.2 s → 690.6 s (C 782.2 s) | 18 → 13 | 0 / 0 |

- Criterion 6 passes (1.011 ≤ 1.10), criterion 7 passes (0 TIMEOUT, 0 Rust OOM), criterion 8 passes (1.081 ≤ 1.36, and also ≤ 1.25).
- Symex median peak RSS: C 143.7 MB, Rust 225.5 MB → 168.2 MB. Largest Rust peak: 443.6 MB (split_list_test05-1).
- Criterion 9: 13 rows above 1.5 wall (10 symex, 3 afl), all with C under 1 s and Rust 0.8–1.55 s. The symex ones are the short tasks (C 0.35–0.96 s, Rust 1.35–1.55 s), so the fixed start-up cost of about +1 s remains.

## Gate 2 (Task 5.1, 2026-10-17)

The second consecutive gate, on the branch as it stands for the merge. Only the Rust side was re-run: the C baseline is still `baseline-c-gate.tsv`, because the merge-base (develop `f01e8c3e`) has not changed (`git merge-base` = `origin/develop` = `f01e8c3e`, so the rebase is a no-op).

### Setup

- Map2Check-rust `f7f15e3b` (feat/rust-runtime HEAD, with the M-1 CMake fix and the README/CHANGELOG commits), built from the working tree with no tracked change, into fresh `build-gate5/` and `release-gate5/`, same CMake flags as `release-gate2`.
- Runtime: the `v1.1.0-rc1` release archive of map2check-library (sha256 `dd90e888…209666`, verified), `MAP2CHECK_RUST_LIB_DIR=<archive>/lib`. `cmp` confirms the installed `lib/libmap2check.a` is the archive's `libmap2check_runtime.a` (sha256 `872caf5f…`).
- KLEE bitcode: built by the branch's `scripts/runtime/map2check-klee-bc.sh` (tag `v1.1.0-rc1`, commit `49a25e4` checked), 1.2 MB, sha256 `9da42b2c…`; `cmp` confirms it is the installed `map2check-klee.bc`.
- Image `map2check-dev:cutover-klee-rust-time`, `--memory 3g --memory-swap 3g --cpus 4`, KLEE `--max-memory=2000` through the overlay `bin/klee` wrapper (seen in the container's process list), `GENS="afl symex" REPS=5 TIMEOUT=45`, `free -g` ≥ 4 GB before the container started. Run 13:02–14:07.
- Output: `baseline-rust-gate2.tsv` (410 rows).

### compare.py

```
$ python3 scripts/cutover/compare.py tests/cutover/baseline-c-gate.tsv tests/cutover/baseline-rust-gate2.tsv --accepted tests/cutover/deltas-aceitos.tsv
median wall ratio: 1.026  median rss ratio: 1.017

gate: PASS
```

No new divergence, and `deltas-aceitos.tsv` is unchanged. Against `baseline-rust-v2.tsv` (gate run 1 + slim symex), no row changed the mode of verdict, tag, line, function or witness.

| gen | median wall Rust/C | median RSS Rust/C | Σ median wall C / Rust | rows >1.5 wall | Rust verdicts (mode) | correct vs `expected_verdict` |
|---|---|---|---|---|---|---|
| afl | 1.009 | 0.983 | 165.1 s / 168.6 s | 3 | 12 FAILED, 29 UNKNOWN | 12 |
| symex | 1.033 | 1.214 | 617.0 s / 534.7 s | 10 | 15 FAILED, 10 SUCCEEDED, 16 UNKNOWN | 24 |
| all | 1.026 | 1.017 | 782.2 s / 703.4 s | 13 | | 36 (C: 31) |

Criteria 6 (1.026 ≤ 1.10), 7 (no TIMEOUT or OOM verdict in any row's mode) and 8 (1.017 ≤ 1.36) pass.

### Rep variance (not a field divergence)

One row has reps that disagree, which gate run 1 never had: `memsafety-ext/tree_stack` valid-memsafety symex gave UNKNOWN ×3 (33–40 s), **SUCCEEDED** (60.44 s) and TIMEOUT (60.12 s, the runner's outer `timeout 60`). The mode is UNKNOWN, as in C and in every earlier Rust run, so `compare.py` passes. One other rep was slow with the same verdict: `memsafety-ext2/length_test03-1` afl rep 1, 53.6 s against a 1.5 s median.

- A diagnostic re-run of tree_stack symex (5 reps, same install, image and caps) gave UNKNOWN 5/5 at 32.9–33.1 s, so this looks like host contention during the gate (the host was near its memory floor), not the runtime.
- The SUCCEEDED rep is still a concern, and it is in the frontend, not in the runtime. `Caller` counts only exit code 124 of `timeout -k 10 36 klee …` (`result == 31744`) as a timeout. A KLEE that ignores SIGTERM and is SIGKILLed after the grace period exits 137, which is not caught, and `kleeHaltedOnTimer` cannot fire without a final stats row. The verdict then comes from the property file, and no violation means TRUE. Hypothesis, not reproduced. The same code is in develop (`f01e8c3e`, `caller.cpp:854`), so it applies to the C runtime too.
