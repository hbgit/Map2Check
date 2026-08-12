# Baseline CASTLE + Juliet — especificação de execução e análise

**Data:** 2026-08-12
**Branch:** `feat/castle-juliet-baseline` (a partir de `develop`)
**Status:** ⏳ Em progresso (harness validado, runs ainda não iniciados)

---

## 1. Objetivo

Estabelecer um **baseline empírico** do Map2Check sobre dois benchmarks completos —
**CASTLE C250** (250 micro-benchmarks rotulados, 25 CWEs) e **Juliet** (escopo C) —
para **identificar padrões de implementação** que possam ser melhorados de forma
incremental, **sem** os swaps estruturais previstos no plano de migração
(`docs/map2check_migration_plan.md`): AFL++, slicing (DG), KLEE 3.2.

O entregável é um relatório `docs/baseline-castle-juliet-2026.md` com métricas
(TP/FP/TN/FN/UNKNOWN/TIMEOUT por CWE/modo) + um **backlog priorizado de fixes
incrementais** nos passes/runtime existentes.

> Referência de contexto: `.opencode/Plano de desenvolvimento_Testcomp.md` (gaps
> G5–G7 de eficácia) e `docs/map2check_migration_plan.md` (Fases 2–3, adiadas).

---

## 2. Decisões fechadas

| Decisão | Valor |
|---|---|
| Branch | `feat/castle-juliet-baseline` (from `develop`) |
| Juliet escopo | **C**: CWEs 121, 122, 415, 416, 476, 761 (`--memtrack`), 190 (`--check-overflow`), 401 (`--memcleanup-property`) — **completo** (~17k arquivos `.c`) |
| Execução | **single-container sequencial** (map2check não é reentrante: escreve arquivos no CWD) |
| Paralelização | experimental (sharding + CWD separados), com fallback sequencial |
| `--add-invariants` | **não usar** (depende de `crabllvm`, não buildado) |
| Ground truth CASTLE | `CASTLE-C250.json` (`vulnerable`/`cwe`/`lines`) |
| Ground truth Juliet | variantes `bad` (espera FALSE) / `good` (espera TRUE) via `-DOMITGOOD`/`-DOMITBAD` |
| Timeouts | Juliet: outer 300s, inner `--timeout 60`; CASTLE: 360s/300s (script existente) |
| Resumibilidade | obrigatória (runs multi-dia, interrompíveis) |

---

## 3. Achados técnicos (causas-raiz descobertas nesta sessão)

Estes determinam o formato do harness. São também **candidatos de melhoria** por si só.

### 3.1 Map2Check escreve artefatos no CWD (não é paralelizável sem isolamento)
`caller.cpp` + backend library escrevem, no diretório de trabalho:
`allocation_log.csv`, `heap_log.csv`, `list_log.csv`, `map2check_property`,
`map2check_checked_error`, `ExecutionOutput.log`, `clang.out`,
`{hash}-result.bc`, `{hash}-output.bc`, `{hash}-witness-result.bc`,
`{hash}-preprocessed.c`, `klee-out-*`/`klee-last`, e `../witness.graphml`
(só com `--generate-witness`). → Todo run em CWD isolado (`mktemp -d`).

### 3.2 `main(int argc, char *argv[])` quebra o wrapper KLEE  ← BUG REAL
`NonDetGeneratorKlee.c` declara `int main() { return __map2check_main__(); }`
— chama o `main` renomeado **sem argumentos**. Programas com `main(argc, argv)`
(Juliet inteiro, TestComp) disparam `KLEE: ERROR: calling function with too few
arguments` → `completed paths = 0` → `VERIFICATION UNKNOWN` para todo programa
**seguro**. Workaround no harness: normalizar `int main(int argc, char * argv[])`
→ `int main(void)` via `sed`. **Fix definitivo (backlog):** wrapper chamar
`__map2check_main__(argc, argv)` com assinatura correta.

### 3.3 I/O do `testcasesupport/io.c` impede exaustão do KLEE  ← LIMITAÇÃO
`printLine`/`printIntLine` (via `printf`), `srand(time(NULL))` e `rand() % 2`
viram chamadas externas → `partially completed paths = 1` → safe ⇒ `UNKNOWN`
em vez de `TRUE`. Workaround no harness: `juliet_stubs.c` (no-op nos `print*Line`,
`globalReturnsTrueOrFalse()` via `__VERIFIER_nondet_int()` para explorar ambos os
ramos). Validado: good ⇒ `VERIFICATION SUCCEEDED`, bad ⇒ `VERIFICATION FAILED`.

### 3.4 Juliet é NIST cru, não sv-benchmarks
`test-comp2026/juliet/` é um clone local (gitignored, **não** versionado; o
submodule `sv-benchmarks` NÃO traz as tasks `Juliet_Test/`). O harness precisa
tratar `-DINCLUDEMAIN -DOMITBAD/-DOMITGOOD` e os variantes de arquivo.

### 3.5 Modos têm motores distintos
`--memtrack` usa KLEE; `--memcleanup-property` usa **LibFuzzer**; `--check-overflow`
e `--check-asserts`/`--target-function` precisam de validação por CWE. Isso afeta a
interpretação por modo.

### 3.6 (já conhecido, de sessões anteriores) FNs/FPs de memtrack
- FN: NULL-deref via falha de `malloc` (KLEE não modela `malloc`→NULL).
- FN: `strcpy`/`memcpy` da libc não instrumentados (só `llvm.memcpy`).
- FP: `415-7`/`416-7` (swap-trick seguro → falso positivo de double-free/UAF).
- Bug latente: off-by-one em `is_valid_allocation_address`; TODO "static array" no `map2check_load`.
- FN: `scanf`/`getenv`/`argv` não simbólicos.

---

## 4. Harness (arquivos versionados nesta branch)

| Arquivo | Status | Papel |
|---|---|---|
| `tests/castle/run_castle_evaluation.sh` | ✅ existente (committed) | 250 CASTLE, 2-pass, score vs JSON |
| `test-comp2026/simulation/run_juliet_evaluation.sh` | ✅ **novo** (esta sessão) | Juliet escopo C, sequencial, resumível, CWD-isolado, normaliza `main`, linka stubs |
| `test-comp2026/simulation/juliet_stubs.c` | ✅ **novo** (esta sessão) | stubs de I/O + `globalReturnsTrueOrFalse` simbólico |
| `docs/baseline-castle-juliet-2026.md` | ⏳ a criar | relatório final + backlog |
| `analyze_results.py` | ⏳ a criar | consome CSVs → tabelas por CWE/modo + TP/FP/TN/FN |

Build/install (gitignored): `build_castle_ci/`, `install_castle_ci/`
(`install_castle_ci/map2check` + symlinks KLEE runtime/clang).

---

## 5. Validação (smoke test, confirmado)

- CASTLE memtrack: `476-5` (NULL deref direto) ⇒ FALSE; `476-8` (safe) ⇒ TRUE.
- CASTLE memcleanup `401-1` ⇒ `FALSE-MEMCLEANUP` / `VERIFICATION FAILED`.
- Juliet `CWE476...int_01`: bad ⇒ `VERIFICATION FAILED`; good ⇒ `VERIFICATION SUCCEEDED`
  (com stubs + normalização de `main`).
- Runner Juliet com `LIMIT=2`: CSV correto (good ⇒ `TRUE`/TN; bad CWE121-memcpy ⇒
  `UNKNOWN`, esperado — é a descoberta).

---

## 6. Plano de execução

- [x] 1. Branch `feat/castle-juliet-baseline` + submodule init.
- [x] 2. Build Map2Check + KLEE no Docker (`install_castle_ci`).
- [x] 3. Runner Juliet + stubs (validado).
- [ ] 4. Kick-off **CASTLE full** (250) — `run_castle_evaluation.sh`, background, resumível.
- [ ] 5. Kick-off **Juliet escopo C** (~17k → ~34k variantes) — background, resumível.
- [ ] 6. Experimento de paralelização (sharding + CWDs separados; fallback sequencial).
- [ ] 7. `analyze_results.py` + `docs/baseline-castle-juliet-2026.md` (relatório + backlog).

---

## 7. Backlog preliminar de melhorias (incremental, sem trocar engine)

1. **Wrapper KLEE `main(argc,argv)`** (§3.2) — `NonDetGeneratorKlee.c` chamar com args corretos.
2. **Instrumentar funções de string libc** (`strcpy`/`memcpy`/`strcat`/`sprintf`) no `MemoryTrackPass`.
3. **Modelar `malloc`→NULL** (branch simbólico de falha) para CWE-476 via malloc.
4. **Corrigir FP swap-trick** (`415-7`/`416-7`) no tracking de aliasing de ponteiros.
5. **Fix off-by-one** em `is_valid_allocation_address`/`is_valid_heap_address`.
6. **Resolver TODO "static array"** no `map2check_load` (alloca de tamanho estático).
7. **Entradas não-simbólicas**: `scanf`/`getenv`/`argv` como fontes simbólicas.
8. (após medir) **modo reachability enxuto** (G5) — desligar memtrack/overflow em Cover-Error.

Ordem real a definir **após** os dados (§6) — o relatório deve quantificar cada
padrão (quantos FN/FP/UNK cada causa responde) antes de priorizar.

---

## 8. Estado multi-sessão

- [x] Branch criada; build persistente em `install_castle_ci/` (gitignored).
- [x] Runner + stubs escritos e validados (ainda não commitados nesta sessão).
- [ ] Commits pendentes: `.gitignore`, runner, stubs, esta spec.
- [ ] Runs ainda NÃO iniciados (CASTLE e Juliet).

**Para retomar:** `git checkout feat/castle-juliet-baseline`; confirmar
`install_castle_ci/map2check`; disparar runners em background com `nohup` +
`RESULTS_DIR` apontando para os diretórios gitignored; monitorar `run.log`.
