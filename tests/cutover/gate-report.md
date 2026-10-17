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
