# tacasv2b — Slicing para memtrack e memcleanup

**Data:** 2026-09-27
**Branch:** `feat/tacas-slicing-mem` (a partir de `feat/tacas-slicing`, PR #68)
**Baseline:** tacasv1 (AFL++ 4.40c + CmpLog), mesmo build com e sem `--slice`
**Status:** rascunho — aguardando revisão
**Registro de rodadas:** `docs/reports/tacas-experiment-log.md` (R6 = sondagem deste design)

---

## 1. Objetivo

Estender o `--slice` para `--memtrack` e `--memcleanup-property` e medir o efeito em três
corpora: **MemSafety do SV-COMP** (programas grandes, onde o slicing tem o que cortar),
**CASTLE** e **Juliet** (os corpora já medidos na v15, programas pequenos). O critério de
sucesso é o slicing **não introduzir veredito errado** (FALSE espúrio ou TRUE errado) e,
onde houver o que cortar, reduzir o tempo ou aumentar os acertos.

---

## 2. Abordagem escolhida: fatiar depois da instrumentação

Na tacasv2a, o slicing roda antes da instrumentação porque o critério (`reach_error`,
`__VERIFIER_assert`) existe no programa do usuário. Para memória não há chamada no
programa que sirva de critério: o critério são os próprios acessos à memória, que só
ganham forma de chamada depois do `MemoryTrackPass`.

Então, em `MEMTRACK_MODE` e `MEMCLEANUP_MODE`:
1. `callPass` instrumenta normalmente, gerando `<hash>-output.bc`.
2. **Novo:** o slicer roda sobre `<hash>-output.bc` com
   - critério = **toda função `map2check_*`** chamada no módulo (as que o runtime do
     memtrack recebe: `map2check_malloc`, `map2check_free`, `map2check_check_deref`,
     `map2check_alloca`, `map2check_load`, `map2check_add_store_pointer`, …) **mais** as
     `__VERIFIER_nondet_*` (lista fixa + IR, como na 2a);
   - `--entry=__map2check_main__` (a instrumentação renomeia o `main` do usuário; o `main`
     real vem do gerador nondet no link);
   - `-cutoff-diverging=false --statistics` (os mesmos motivos da 2a).
3. `linkLLVM` segue igual, sobre o `-output.bc` fatiado.

**Por que é seguro:** o problema registrado no código (fatiar depois da instrumentação
remove a instrumentação) acontecia com o critério `reach_error`: as chamadas que
**registram** a violação não influenciam o alcance do alvo e eram cortadas. Aqui toda
chamada do runtime é critério, então nenhuma é removida; o que sai é computação que não
alimenta nenhum acesso à memória, alocação ou leitura de entrada.

**Sondagem (R6):** busybox `basename-2.i` 2985 → 2373 instruções (−20%);
memsafety-cve `admesh.i` −1,5%; o slicer falha em `llvm.stacksave` (VLA) e aí o Caller
já volta ao programa inteiro.

**Rejeitadas:** marcadores no estilo Symbiotic antes da instrumentação (mais código, ganho
não medido) e A/B das duas (dobra o custo). Ficam como alternativa se a medição mostrar
fatias grandes demais.

---

## 3. Mudanças

### 3.1 `slicer.hpp`
- `runtimeNamesInIR(ir)`: todo símbolo `@map2check_[A-Za-z0-9_]+` no IR textual, na ordem
  da primeira ocorrência, sem repetição (mesma forma de `nondetNamesInIR`).

### 3.2 `Caller`
- `sliceInstrumented()`: fatia `<hash>-output.bc` no lugar. Desmonta com `opt -S`,
  junta `runtimeNamesInIR` + `nondetNamesInIR` como critério, roda o slicer com
  `--entry=__map2check_main__`, e em sucesso renomeia a fatia por cima do `-output.bc`.
  Mesmo orçamento, mesmo fallback (programa inteiro com aviso) e mesma linha de log
  (`describeSlice`, rótulo `map2check runtime`) da 2a.
- Sem stub `weak`: nenhuma função do programa é critério.
- Refatoração: o trecho comum com `sliceWithRespectToTarget` (desmontar, ler estatísticas,
  orçamento, fallback) vira um helper privado, para as duas usarem o mesmo caminho.

### 3.3 `map2check.cpp`
- `--slice` em `MEMTRACK_MODE`/`MEMCLEANUP_MODE` chama `sliceInstrumented()` **depois** de
  `callPass` e **antes** de `linkLLVM`. Overflow continua recusado (2c); a mensagem passa a
  "reachability, assert and memory properties".

### 3.4 Harness de MemSafety do SV-COMP (novo)
- `build_corpus.py` ganha as propriedades `memsafety` (`valid-memsafety.prp`) e
  `memcleanup` (`valid-memcleanup.prp`). Para essas, o manifesto carrega o **veredito
  esperado e a subpropriedade** (`valid-deref`/`valid-free`/`valid-memtrack`), lidos do
  `.yml`.
- Categorias por diretório, espelhando os `.set` do SV-COMP (os `.set` não vêm no
  benchmark local):

| categoria | diretórios |
|---|---|
| Arrays | `array-memsafety`, `array-memsafety-realloc` |
| Heap | `memsafety`, `memsafety-ext*`, `memsafety-broom`, `ldv-memsafety*`, `forester-heap`, `heap-manipulation` |
| LinkedLists | `list-simple`, `list-ext-properties`, `list-properties`, `ddv-machzwd` |
| Other | `busybox-1.22.0`, `coreutils-v8.31`, `coreutils-v9.5-units`, `memsafety-cve`, `uthash-2.0.2`, `goblint-regression`, `goblint-coreutils` |
| Juliet | `Juliet_Test` |
| MemCleanup | tarefas com `valid-memcleanup.prp` |

  Excluídos: `pthread*`, `weaver`, `termination-*` (o Map2Check não suporta threads nem
  terminação; mediria zeros sem significado).
- `run_memsafety_evaluation.sh` (novo, no molde do `run_testcomp_evaluation.sh`: retomável,
  por shard, `EXTRA_FLAGS`, `BUDGET`): roda `--memtrack` (ou `--memcleanup-property`) e
  classifica cada tarefa em `correct-true`, `correct-false` (FALSE **com a subpropriedade
  certa**), `wrong-true`, `wrong-false`, `unknown`, `error`. FALSE com subpropriedade
  diferente conta como `wrong-false`.

### 3.5 CASTLE e Juliet
- `run_castle_evaluation.sh` e `tests/juliet/run_juliet_evaluation.sh` passam a aceitar
  `EXTRA_FLAGS` (acrescentado a `mode_flags`) e `RESULTS_DIR` já existente, para o braço
  com `--slice`.

---

## 4. Testes

Integração (`test_testcomp_regressions.sh`):
1. **Memtrack com slice mantém a detecção:** programa com uso depois de `free` e código
   irrelevante; `--memtrack --slice --nondet-generator symex` → `Sliced with respect to
   map2check runtime` e FALSE-FREE/FALSE-DEREF como sem slice.
2. **Memcleanup com slice mantém o leak:** leak alcançado por leitura nondet →
   FALSE-MEMCLEANUP com slice.
3. **Programa seguro não vira FALSE com slice** (mesma forma do 1, sem o bug).
4. **VLA:** programa com array de tamanho variável → aviso de fallback, veredito igual ao
   sem slice.
5. A recusa em overflow continua (mensagem nova).

Unitário: `runtimeNamesInIR` acha todo símbolo `@map2check_*` e deduplica. Não precisa
distinguir função de variável global: um nome sem call site é critério inofensivo (a 2a
verificou isso no código do dg).

Harness: teste do classificador de veredito (tabela esperado × obtido × subpropriedade →
classe), no estilo de `tests/lib/verdict_classifier.sh`.

---

## 5. Avaliação (mini-rodadas, cada uma registrada no log)

- **R7:** CASTLE completo (250), tacasv1 com e sem `--slice`, 300 s.
- **R8:** amostra de MemSafety do SV-COMP (por categoria, cota pequena — p.ex. 10 por
  categoria), com e sem `--slice`.
- **R9:** Juliet escopo C, amostra por CWE, com e sem `--slice`.
- Em cada uma: acertos, **erros (wrong-true/wrong-false)**, unknown, tempo mediano e
  redução de instruções. Comparação explícita com a rodada anterior.
- A avaliação completa fica para a execução sequencial depois do merge.

---

## 6. Riscos

| Risco | Mitigação |
|---|---|
| Fatia remove algo que o runtime precisava → FALSE espúrio ou violação perdida | Todo `map2check_*` é critério; testes 1–3; wrong-* medido em R7–R9 |
| Slicer não suporta construções (VLA/`stacksave`, threads, `longjmp`) | Fallback já existente para o programa inteiro, com aviso; teste 4 |
| Redução pequena (R6: 1,5%–20%) → efeito nulo | Aceitável: o critério é não piorar; o ganho é medido, não presumido |
| Categorias por diretório divergem dos `.set` oficiais | Mapeamento documentado na §3.4; é amostra de diagnóstico |
