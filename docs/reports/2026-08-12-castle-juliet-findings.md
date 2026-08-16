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

> **Revisto em 2026-08-16 — o fallback não cobria o caminho que usamos.** O aviso
> ficava dentro de `if (!is_llvmir_in)`. Com entrada `.c` ele aparecia; com `.bc`
> — como todo o baseline v5, o arreio do CASTLE e os wrappers do BenchExec
> invocam a ferramenta — o bloco inteiro era pulado e **nem o aviso saía**. A
> flag era aceita, ignorada e não deixava rastro. Ver §2.x abaixo e
> [a revisão da dependência](2026-08-16-crabllvm-review.md).

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
- `369` (div por zero) → `--check-overflow` (agora funcional via §1.8).
- `617` → `--check-asserts` (agora funcional via §1.4).

### 1.8 Divisão por zero — propriedade dedicada `FALSE-DIVBYZERO`
- **Bug:** `divisionByZeroError()` era TODO no-op; div-por-zero não era
  detectado.
- **Fix:** implementar a checagem + propriedade dedicada (segue a convenção de
  KLEE/CBMC/SV-COMP — CWE-369 usa `unreach-call`, não `no-overflow`).
- **Arquivos:** `AnalysisModeOverflow.c`, `Map2CheckTypes.h`, `PropertyGenerator.c`,
  `tools.hpp/.cpp`, `counter_example.cpp/.hpp`, parsers dos evals, `test_modes.sh`.
- **Validação:** `div_bug`⇒`FALSE-DIVBYZERO`, `CASTLE-369-1`⇒`FALSE`.
- **Commit:** `c09378d6b`

### 1.9 Assinatura i64/i32 dos binops do OverflowPass (bug latente)
- **Bug:** `OperationsFunctions.hpp` declarava `map2check_binop_{add,sub,mul,sdiv}`
  com parâmetros `i64`, mas a runtime (`AnalysisModeOverflow.c`) define `int`
  (i32). A checagem runtime de overflow estava **silenciosamente quebrada** — o
  overflow só era detectado via warning `-Winteger-overflow` do clang.
- **Bug 2:** o check era inserido **depois** da operação; para div-por-zero o
  `sdiv` dispara SIGFPE antes do check.
- **Fix:** declarar `i32` (sem cast i64) + inserir o check **antes** da operação.
- **Validação:** `ov_nondet` (overflow runtime) ⇒ `OVERFLOW`/`FAILED`.
- **Commit:** `2cd1042ac`

---

## 2. Achados (bugs/limitações identificados)

| # | Achado | Local | Situação |
|---|---|---|---|
| A | Truncamento 32-bit no allocation log | `AllocationLog.c` | ✅ corrigido (§1.3) |
| A2 | Assinatura i64/i32 + ponto de inserção dos binops (overflow runtime quebrado) | `OperationsFunctions.hpp`, `OverflowPass.cpp` | ✅ corrigido (§1.9) |
| B | `--target-function --target-function-name main` é **degenerado**: TargetPass instrumenta chamadas *para* `main` (que ninguém faz) e `AnalysisModeNone` sempre retorna "correct" | `TargetPass.cpp`, `AnalysisModeNone.c` | aberto (afeta 253/362/369/628/674/770/835) |
| C | `divisionByZeroError()` era TODO no-op | `AnalysisModeOverflow.c:86` | ✅ corrigido (§1.8) |
| D | Overhead "fuzzer-first": default roda LibFuzzer (60s) antes do KLEE | `map2check.cpp:498-508` | aberto (G5 — lean reachability via `--nondet-generator symex`) |
| E | 33/250 CASTLE não compilam por headers externos (`mysql.h`, `openssl/*.h`) | imagem Docker | aberto (maioria CWE-89/798, fora de escopo) |
| F | `--add-invariants` aceito e ignorado **sem rastro** na entrada `.bc`: o aviso do §1.5 estava dentro de `if (!is_llvmir_in)` | `map2check.cpp:227-245` | ✅ corrigido — agora sai com código 3 |
| G | `main()` descarta o retorno de `map2check_execution()` nas três chamadas, então o código de saída nunca reflete o resultado da análise — inclusive o `return 1` de `--expected-result` | `map2check.cpp:521,524,528` | aberto |
| H | **Use-after-scope no log de nondet:** `NONDET_CALL.value` guardava o endereço do parâmetro `value` do wrapper `map2check_nondet_*`. Esse quadro de pilha já morreu quando o log é gravado na saída | `Map2CheckTypes.h`, `NonDetLog.c` | ✅ corrigido — valor agora inline (união) |
| I | `map2check_nondet_double` registrava o tipo como `UNSIGNED`, então todo double logado saía como `%u` sobre 4 bytes da mantissa | `NonDetLog.c:130` | ✅ corrigido |
| J | Todo estado bifurcado do KLEE grava o mesmo `klee_log.csv` na saída; o último a terminar vence, e não é o violador (que aborta cedo) | `WitnessGeneration.c`, `Map2CheckFunctions.c` | ✅ corrigido — flush só na violação |

### O log de nondet estava vazio em toda execução simbólica

Os achados **H**, **I** e **J** foram descobertos juntos, ao tentar emitir a test
suite do Test-Comp, e explicam por que o `klee_log.csv` saía com **zero bytes**
em todo run com `--nondet-generator symex`:

1. O KLEE detectava o erro de ponteiro do achado **H** dentro do próprio
   `nondet_log_to_file()` e matava o estado — `completed paths = 0`,
   `test000001.ptr.err`. Nativamente o mesmo código costumava "funcionar",
   porque a pilha morta ainda continha o valor: é UB que passa despercebido
   fora de um executor simbólico.
2. Corrigido **H**, o vetor passou a sair com o primeiro valor certo e o
   segundo errado — porque um estado que tomou outro ramo sobrescrevia o
   arquivo depois (achado **J**).
3. Corrigidos os dois, o vetor recuperado é exatamente o da execução violadora.

Consequência para o baseline v5: nenhum veredito muda — a detecção nunca
dependeu do log —, mas **nenhum contraexemplo de execução simbólica jamais
carregou valores de entrada**, e a coluna de valores nondet dos witnesses
gerados sob KLEE não tinha conteúdo. Isso é pré-requisito para pontuar no
Test-Comp, onde o vetor de entrada *é* o artefato avaliado.

### Correção do registro: o pass 2 do CASTLE nunca aplicou invariantes

O arreio re-executava todo UNKNOWN com `--add-invariants`. Como o achado **F**
mostra, essa flag era ignorada sem rastro na entrada `.bc` — e a entrada é `.bc`.
O pass 2 era, portanto, uma re-execução **byte-idêntica** do pass 1. Duas
consequências para os dados do v5:

- A coluna `used_invariants=yes` não significa o que diz. **Nenhuma execução do
  baseline v5 usou invariantes.**
- Cada caso UNKNOWN custou dois orçamentos de timeout em vez de um, o que infla
  o tempo de parede das linhas afetadas sem mudar veredito algum.

Nenhum veredito da tabela publicada muda: uma segunda execução idêntica só
converteria um UNKNOWN por acaso, e o veredito registrado era o da última
execução de qualquer forma. O que muda é a interpretação da coluna e dos tempos.
A partir do v6 o arreio reconhece o código de saída 3 e registra
`used_invariants=unavailable`, preservando o veredito do pass 1 —
ver [a revisão da dependência](2026-08-16-crabllvm-review.md).

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

1. ~~Implementar `divisionByZeroError()` (CWE-369)~~ ✅ (§1.8).
2. Corrigir/desativar o modo reachability degenerado (TargetPass target=main).
3. Lean reachability (KLEE-only por default em reachability).
4. Instrumentar funções de string libc (`strcpy`/`memcpy`) no MemoryTrackPass.
5. Modelar `malloc`→NULL para CWE-476 via malloc.
6. Re-rodar CASTLE/Juliet com os fixes para o relatório final.
