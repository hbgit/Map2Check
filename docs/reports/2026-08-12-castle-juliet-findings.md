# Map2Check — Relatório de correções e achados (CASTLE + Juliet baseline)

**Branch:** `feat/castle-juliet-baseline` (a partir de `develop`)
**Data:** 2026-08-12

> Documento de apoio para a PR. Lista cada correção e cada achado da sessão de
> baseline empírico sobre CASTLE (250) e Juliet (escopo C).

---

## 1. Correções aplicadas (code fixes)

### 1.1 Suporte a `main(int argc, char *argv[])` — wrapper de nondet
- **Bug:** `NonDetGeneratorKlee.c` / `NonDetGeneratorLibFuzzy.c` chamavam
  `__map2check_main__()` **sem argumentos**. Programas com `main(argc, argv)`
  (Juliet inteiro, Test-Comp) caíam em `KLEE: calling function with too few
  arguments` → todo programa seguro virava `UNKNOWN`.
- **Fix:** repassar `(argc, argv)` (KLEE) e `(0, NULL)` (LibFuzzer).
- **Teste:** `tests/integration/test_main_argv.sh` (bad⇒FALSE, safe⇒TRUE), no CI.
- **Commit:** `ae65830c4`

### 1.2 `-m2c-entry-function` passado incondicionalmente
- **Bug:** `caller.cpp` passava `-m2c-entry-function` ao `opt` sempre, mas esse
  `cl::opt` só é registrado pelo `MemoryTrackPass`. Nos modos overflow/assert/
  reachability (que não carregam o pass), o `opt` abortava → `main` nunca era
  renomeado → `undefined reference to __map2check_main__` → `ERROR`.
- **Fix:** passar o flag só quando o MemoryTrackPass é carregado.
- **Teste:** `tests/integration/test_modes.sh` (overflow/assert/target), no CI.
- **Commit:** `ea9669c9d`

### 1.3 Truncamento 32-bit no `find_row_with_address` (memtrack)
- **Bug:** `AllocationLog.c` fazia cast do ponteiro para `unsigned` (32-bit),
  enquanto o resto usa 64-bit. Com o heap de 1024 GiB do KLEE, dois endereços
  distintos colidem nos 32 bits baixos → duas alocações vivas se confundem no
  allocation log → falsos positivos (e FNs latentes) em programas com múltiplas
  alocações. Os casos de "swap-trick" (415-7/8, 416-7/8/10) apenas *disparavam*
  o bug.
- **Fix:** `unsigned` → `unsigned long` (2 linhas).
- **Validação:** 415-7/8, 416-7/10 agora ⇒ `SUCCEEDED`; 415-2, 416-1 continuam ⇒ `FAILED`.

### 1.4 `assert()` da libc não instrumentado (AssertPass)
- **Bug:** `AssertPass` só interceptava `__VERIFIER_assert`. O `assert()` do
  `<assert.h>` (glibc) lowera para `__assert_fail(...)`, não tratado → `ERROR`.
- **Fix:** interceptar `__assert_fail` e emitir `map2check_assert(0, arg2, arg3)`
  (condição falsa, linha e função vêm dos operandos).
- **Validação:** `CASTLE-617-1` agora ⇒ `FAILED`; caso novo em `test_modes.sh`.

### 1.5 crabllvm (`--add-invariants`) — fallback gracioso
- **Causa:** `cmake/FindCrabLlvm.cmake` (seahorn/crab-llvm) nunca foi ligada ao
  build moderno e é incompatível com LLVM 16; `--add-invariants` referenciava um
  binário inexistente e quebrava o pipeline.
- **Fix:** `map2check.cpp` detecta a ausência e degrada graciosamente (warning).
- **Issue aberta:** ver §3. **Commit:** `914d7bab8`

### 1.6 Parser de veredito / linha dos scripts de avaliação
- **Bug A (veredito):** `grep -qi "TIMEOUT|timed out"` casava com o erro do
  utilitário `timeout` ("failed to run command ... No such file or directory"),
  classificando falha de infra como TIMEOUT.
- **Fix A:** `detect_verdict`/`classify_verdict` — ordem `FALSE → TRUE → "Forcing
  timeout" (TIMEOUT real) → falha de infra (ERROR) → UNKNOWN`, com bucket `ERROR`.
- **Bug B (linha):** `grep 'line \d+' | head -1` casava com `in**line 8**-bit
  counters` (LibFuzzer), reportando "line 8" para tudo.
- **Fix B:** extrair de `map2check_property line \K\d+`.
- **Commit:** `26d9a85c1` (A); B neste commit.

### 1.7 Mapeamento CWE→modo (CASTLE)
- `134` (format string), `253` (checagem de retorno), `362` (race) → **N/A**
  (nenhum modo detecta).
- `369` (div por zero) → `--check-overflow` (correto; ainda FN até
  `divisionByZeroError()` ser implementado).
- `617` → `--check-asserts` (agora funcional via §1.4).

---

## 2. Achados (bugs/limitações identificados)

| # | Achado | Local | Situação |
|---|---|---|---|
| A | Truncamento 32-bit no allocation log | `AllocationLog.c` | ✅ corrigido (§1.3) |
| B | `--target-function --target-function-name main` é **degenerado**: TargetPass instrumenta chamadas *para* `main` (que ninguém faz) e `AnalysisModeNone` sempre retorna "correct" | `TargetPass.cpp`, `AnalysisModeNone.c` | aberto (afeta 253/362/369/628/674/770/835) |
| C | `divisionByZeroError()` é TODO no-op | `AnalysisModeOverflow.c:86` | aberto (369) |
| D | Overhead "fuzzer-first": default roda LibFuzzer (60s) antes do KLEE | `map2check.cpp:498-508` | aberto (G5 — lean reachability via `--nondet-generator symex`) |
| E | 33/250 CASTLE não compilam por headers externos (`mysql.h`, `openssl/*.h`) | imagem Docker | aberto (maioria CWE-89/798, fora de escopo) |

---

## 3. Issue aberta (GitHub)

- **`crabllvm (seahorn/crab-llvm) is incompatible with LLVM 16 — --add-invariants is a no-op`**

---

## 4. Baseline (em andamento)

- **CASTLE 250:** rodando (parcial ~139/250 na última checagem), log
  `/tmp/opencode/castle_bg.log`, resultados `tests/castle/results/`.
- **Juliet escopo C:** watcher aguardando o CASTLE terminar.
- Métricas preliminares (parciais, pré-fixes) indicavam: memtrack com muitos
  "FP" que eram, na verdade, **artefato do parser de linha** (§1.6-B) + os
  swap-tricks reais (§1.3). Ambos corrigidos → re-rodada dará números limpos.

---

## 5. Backlog (próximos passos, sem trocar engine)

1. Implementar `divisionByZeroError()` (CWE-369).
2. Corrigir/desativar o modo reachability degenerado (TargetPass target=main).
3. Lean reachability (KLEE-only por default em reachability).
4. Instrumentar funções de string libc (`strcpy`/`memcpy`) no MemoryTrackPass.
5. Modelar `malloc`→NULL para CWE-476 via malloc.
6. Re-rodar CASTLE/Juliet com os fixes para o relatório final.
