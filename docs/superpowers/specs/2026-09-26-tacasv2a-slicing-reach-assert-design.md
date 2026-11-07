# tacasv2a — Slicing para reachability e assert que preserva a suíte de testes

**Data:** 2026-09-26
**Branch:** `tacas/slicing` (a partir de `tacas/aflpp`)
**Baseline:** tacasv1 (AFL++ 4.40c + CmpLog), mesmo motor com e sem `--slice`
**Status:** rascunho — aguardando revisão

---

## 1. Objetivo

Fazer o `--slice` deixar de **prejudicar** o Cover-Error e, se possível, ajudar. Na v15, o
braço com slice cobriu 412 tarefas contra 470 do controle (McNemar χ² = 36,10), com 57
das 58 perdas em ECA. Esta etapa corrige a causa dessas perdas e estende o slicing ao
modo assert. É o primeiro sub-projeto da tacasv2 ("slicing por propriedade"); memória e
overflow ficam para 2b e 2c.

> Linha de desenvolvimento: `decisions/tacas-afl-slicing-roadmap.md` (ai-memory).
> A tacasv2 parte da tacasv1 porque o AFL++ é a evolução decidida: nada é calibrado
> contra o LibFuzzer.

---

## 2. Diagnóstico (medido em 2026-09-26)

Amostra: 12 tarefas que a v15 cobria sem slice e perdia com slice (9 ECA, 3 de outras
famílias). Build tacasv1, orçamento de 300 s, TestCov 300 s, uma execução por braço.

| braço | cobertas |
|---|---|
| controle (sem slice) | 11/12 |
| slice como está hoje | 6/12 |
| slice com `-cutoff-diverging=false` | 10/12 |
| slice sem cutoff + nondets como critério | 10/12 (e o único que cobre `floppy.i.cil-1`) |

As duas tarefas que a última variante não cobriu terminaram perto do orçamento (~258 s).
Com uma execução por braço, não dá para separar isso de variação.

**Depois da implementação** (2026-09-27, build final da `tacas/slicing`, mesma amostra e
orçamento, uma execução por braço): controle **12/12**, slice **12/12**. `floppy.i.cil-1`
dá FAILED + COVERED com slice. O slice terminou antes do controle em 5 das 9 tarefas ECA
(por exemplo, `Problem17_label55`: FAILED em 51 s, contra UNKNOWN em 137 s no controle).
São 12 tarefas: é validação de que os defeitos sumiram, não avaliação (§7).

### Defeito 1 — o KLEE aborta em toda fatia com caminho cortado

- O `--cutoff-diverging` (default `true` no dg/sbt-slicer) insere um bloco `diverge:`
  com `call exit(0)` + `unreachable`. O `--help` diz "abort()", mas o código
  (`dg/tools/llvm-slicer-preprocess.cpp`) chama `exit(0)`.
- Essas chamadas não têm localização de debug (`!dbg`), e o programa é compilado com `-g`.
- Quando o KLEE linka a uClibc, `exit` passa a ter corpo e o verificador rejeita o módulo:
  `inlinable function call in a function with debug info must have a !dbg location` →
  `LLVM ERROR: Broken module found`. O KLEE aborta (status 134) antes de executar.
- Em `eca-rers2012/Problem15_label09.c`, a fatia tem 60 blocos `diverge`.
- **Consequência:** no braço com slice da v15, o KLEE não rodou em nenhuma tarefa com
  caminho cortado. Com o LibFuzzer, o `exit(0)` também encerrava a sessão de fuzzing.
- Com `-cutoff-diverging=false`, o KLEE da mesma tarefa roda até o fim (saída 0).

### Defeito 2 — o vetor da fatia não vale no programa original

- O slicer remove chamadas `__VERIFIER_nondet_*` cujo valor não afeta o critério. Em
  `ntdrivers/floppy.i.cil-1.c`: 29 chamadas no programa, 10 na fatia.
- O vetor de entradas é gerado na fatia, mas o TestCov executa o **programa original**,
  que consome mais valores e em outra ordem. O veredito sai FAILED e a suíte não cobre.
- Passar as funções nondet como critério **primário**, junto com o alvo, preserva as
  chamadas alcançáveis: `floppy` passa a FAILED + COVERED.

---

## 3. Decisões

| Decisão | Valor |
|---|---|
| Cutoff | `-cutoff-diverging=false` |
| Critério em reachability | `<função alvo>` + todas as funções `__VERIFIER_nondet_*` conhecidas |
| Critério em assert | `__VERIFIER_assert` + as mesmas funções nondet |
| Onde fatiar | continua **antes** da instrumentação (`sliceWithRespectToTarget`, antes de `callPass`) |
| Stub da função alvo | mantém o stub `weak` atual |
| Visibilidade | `--statistics` no slicer; o Caller registra funções/blocos/instruções antes → depois |
| Outras flags (`--pta`, `--cda`, `--undefined-funs`) | defaults nesta etapa; calibrar só com medição própria |
| Cutoff "consertado" (manter o corte, corrigir `!dbg`, trocar `exit` por poda silenciosa) | fora; experimento condicional se a medição mostrar falta de eficiência de busca |

A lista de funções nondet é fixa: as 16 que o `NonDetPass` instrumenta (`bool`, `char`,
`uchar`, `short`, `ushort`, `int`, `uint`, `unsigned`, `long`, `ulong`, `size_t`,
`loff_t`, `sector_t`, `pointer`, `pchar`, `double`) mais as do SV-COMP que ele ainda não
cobre (`float`, `longlong`, `ulonglong`, `_Bool`, `u8`, `u16`, `u32`, `charp`). O slicer
aceita nomes que não existem no programa. Isso foi verificado: não dá erro e não muda a
saída. Uma lista fixa dispensa desmontar o bitcode.

---

## 4. Referência comparada: o que o Symbiotic faz

Levantado no código do Symbiotic (master `4474bb9`), do sbt-slicer (`e350116`, o mesmo
SHA que o nosso Dockerfile fixa) e do dg.

| Aspecto | Symbiotic | tacasv2a | Por quê |
|---|---|---|---|
| Fatia em Test-Comp (coverage-error/branches) | **Não** no pipeline principal | Sim, em Cover-Error | É onde o Map2Check usa o slicing; exige resolver o Defeito 2, que ele nunca enfrenta |
| `--cutoff-diverging` | Mantém (default) | Desliga | O KLEE dele reconhece violação por `-error-fn` e o módulo dele não quebra; o nosso quebra (Defeito 1) |
| Ponto do pipeline | Depois da instrumentação, com marcadores como critério | Antes da instrumentação | Para reach/assert o critério já existe no programa; os marcadores entram no 2b/2c |
| Contraexemplo | Reexecutado no programa sem slice (`-replay-nondets`) | Nondets preservados na fatia | Replay exigiria nomear cada nondet por call site; preservar é mais simples e resolve para Test-Comp |
| Flags extras | `-pta fi`, `-2c __VERIFIER_assume,klee_assume` (implícito) | Iguais por default | — |

**O que é contribuição nossa:** slicing que **preserva a ordem de consumo das entradas**, de
modo que a suíte gerada na fatia continua válida no programa original. O Symbiotic não
precisa disso porque não gera suíte a partir da fatia.

---

## 5. Mudanças

### 5.1 `Caller::sliceWithRespectToTarget` (`modules/frontend/caller.cpp`)
- Recebe a lista de critérios em vez de só o nome da função alvo, e monta
  `-c <alvo>,<nondets…>`.
- Acrescenta `-cutoff-diverging=false` e `--statistics`.
- Extrai do `slicer.output` as linhas `Statistics before/after` e registra
  `Sliced with respect to X: F/B/I functions/blocks/instructions → F'/B'/I'`, mantendo
  também os bytes.
- O restante (orçamento, fallback para o programa inteiro, stub `weak`) não muda.

### 5.2 Gating em `map2check.cpp`
- `--slice` passa a valer também em `ASSERT_MODE`, com o critério `__VERIFIER_assert`.
  Os demais modos continuam recusados com aviso, até o 2b e o 2c.

### 5.3 Funções puras do slicing
- `modules/frontend/utils/slicer.hpp` (header-only, testável sem build completo):
  `nondetFunctionNames()`, `slicingCriteria()`, `targetStubSource()` (o stub de
  `__VERIFIER_assert` recebe `int cond`), `parseSlicerStatistics()` e `describeSlice()`.
- Em assert o critério primário é `__VERIFIER_assert,__assert_fail`: o `AssertPass`
  instrumenta as duas.

---

## 6. Testes

Integração (`tests/integration/test_testcomp_regressions.sh`, seção de slicing):
1. **O KLEE sobrevive à fatia.** Um programa com ramos que não alcançam o alvo, rodado com
   `--slice --nondet-generator symex`: sem `Broken module`, com FAILED.
2. **O vetor vale no original.** Um programa com um nondet irrelevante antes do relevante
   (`int a = nondet(); int b = nondet(); if (b == 42) reach_error();`), rodado com
   `--slice --generate-test-suite`: a suíte tem 2 entradas, na ordem do original.
3. **Assert fatia.** `--check-asserts --slice` registra `Sliced with respect to
   __VERIFIER_assert` e mantém o veredito.
4. Os testes existentes de slicing ("degrade loudly", recusa em cover-branches) continuam.

Unitário: se a extração das estatísticas virar função própria, um teste de parser sobre um
`slicer.output` fixo.

---

## 7. Avaliação (tacasv2a)

- **Quando:** depois do merge do PR #66 e desta branch, na execução sequencial combinada
  com o usuário.
- **Como:** corpus de Cover-Error (1087 tarefas pareadas, `cover-error-q400.tsv`), dois
  braços com o **mesmo build**: controle (sem slice) e `--slice`. Orçamento de 300 s,
  3 shards, como na v15.
- **Critério de sucesso:** o braço com slice **não perde** para o controle (McNemar sem
  diferença significativa contra, ou a favor) e as perdas em ECA desaparecem.
- **Relato:** cobertas por família, perdas/ganhos pareados e estatísticas de redução
  (instruções antes/depois).

---

## 8. Riscos

| Risco | Mitigação |
|---|---|
| Sem o cutoff, a fatia fica maior e o ganho de busca diminui | É o preço da correção. O cutoff "consertado" fica como experimento condicional (§3) |
| Preservar nondets puxa código que a fatia removeria | Medido no `floppy`: 8933 → 9155 linhas de IR (+2,5%). Acompanhar a redução na avaliação |
| Alguma função nondet fora da lista | A lista inclui as do SV-COMP; uma função ausente só reproduz o Defeito 2 naquela tarefa, e o teste 2 detecta o caso comum |
| Amostra de 12 tarefas | É diagnóstico, não avaliação; a conclusão vem da §7 |
