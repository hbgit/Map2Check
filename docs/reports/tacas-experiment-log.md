# Registro de mini-rodadas — linha TACAS

Uma entrada por mini-rodada de testes, na ordem em que foram feitas. Cada entrada tem
o que mudou, a configuração, os dados brutos, a comparação com a rodada anterior
relevante e a leitura. Os números aqui são **diagnóstico** (amostras pequenas, uma ou
poucas execuções); a avaliação de cada versão é a execução sequencial completa, feita
depois do merge.

Ambiente comum: imagem `map2check-dev:aflpp` (Dockerfile.dev com AFL++ 4.40c), host
WSL2. Build da v15 = `develop` em `415032766`, no mesmo container.

---

## R1 — tacasv1: smoke do AFL++ (2026-09-26)

- **Mudança:** LibFuzzer → AFL++ 4.40c (persistente, PCGUARD), 1 instância.
- **Config:** 4 programas mínimos, `--timeout 30`.
- **Dados:**

| programa | esperado | resultado |
|---|---|---|
| reach `x != 0` (a semente já causa crash) | FAILED | FAILED 3/3 (1 timeout no dry-run por core dump do WSL em outra execução) |
| reach `1000<x<1100` | FAILED | FAILED |
| reach seguro | ≠FAILED | UNKNOWN |
| KLEE-only `x == 123456` | FAILED | FAILED |

- **Leitura:** o pipeline AFL++ funciona de ponta a ponta. Veja R2 para a força de busca.

## R2 — tacasv1: v15 × AFL++ (PCGUARD puro), 11 programas (2026-09-26)

- **Config:** 11 programas mínimos (reach, assert, memtrack, memcleanup, overflow;
  bugs e variantes seguras), `--timeout 30`, 1 execução.
- **Só-fuzzer:** v15 LibFuzzer **7/9** bugs × AFL++ **3/9**.
- **Híbrido:** vereditos idênticos à v15 nos 11 programas, todos corretos.
- **Leitura:** sem CmpLog, o AFL++ não resolve comparações (`x == 123456`) que o
  `-use_value_profile` do LibFuzzer resolve → motivou R3.

## R3 — tacasv1: CmpLog e reset do índice (2026-09-26)

- **Mudança:** binário CmpLog (`-c`) + índice de leitura zerado a cada `__AFL_LOOP` +
  remoção do `-V` (relógio do WSL2 volta até 1,1 s em 20 s e encerrava o AFL++).
- **Config:** 8 programas com bug, 3 execuções cada, só-fuzzer, `--timeout 30`.

| configuração | bugs achados |
|---|---|
| v15 LibFuzzer (value profile, 8 jobs) | 20/24 |
| AFL++ + CmpLog, índice preservado | 8/24 |
| AFL++ + CmpLog, índice zerado | **17/24** |

- **Comparação com R2:** o AFL++ foi de 3/9 (um terço) para 17/24 (~71%); a v15 fez 20/24.
- **Híbrido (11 programas):** idêntico à v15, todos corretos.
- **Leitura:** o reset do índice é o que faz o CmpLog funcionar. O AFL++ ainda perde bugs
  guardados por faixa estreita (`5000<x<5100`): o CmpLog propõe os limites da faixa.

## R4 — tacasv2a: diagnóstico do slicing (2026-09-26)

- **Amostra:** 12 tarefas de Cover-Error que a v15 cobria sem slice e perdia com slice
  (9 ECA, 3 outras). Build tacasv1, 300 s, TestCov 300 s, 1 execução por braço.

| braço | cobertas |
|---|---|
| controle (sem slice) | 11/12 |
| slice como estava | 6/12 |
| slice `-cutoff-diverging=false` | 10/12 |
| slice sem cutoff + nondets como critério | 10/12 (único a cobrir `floppy`) |

- **Achados:** o cutoff insere `exit(0)` sem `!dbg` → o KLEE aborta com "Broken module";
  o slicer remove leituras nondet → a suíte desalinha no programa original.

## R5 — tacasv2a: implementação final (2026-09-27)

- **Mudança:** cutoff desligado + nondets (lista fixa + extraídas do IR) como critério +
  `--slice` em assert.
- **Mesma amostra de R4:** controle **12/12**, slice **12/12**; `floppy` FAILED + COVERED.
- **Comparação com R4:** slice 6/12 → 12/12. O slice terminou antes do controle em 5 das
  9 ECA (ex.: `Problem17_label55` FAILED em 51 s × UNKNOWN em 137 s no controle).
- **Testes:** integração 23/23, ctest 10/10, etapa TestCov da CI 16/16, 5/5, 6/6.

## R6 — tacasv2b: sondagem do slicing após a instrumentação (2026-09-27)

- **Config:** programas de MemSafety do SV-COMP, `--memtrack`, `*-output.bc`
  (instrumentado), critério = todas as `map2check_*` + nondets, entrada
  `__map2check_main__`, `-cutoff-diverging=false --statistics`.

| programa | linhas | instruções antes → depois | observação |
|---|---|---|---|
| busybox `basename-2.i` | 1602 | 2985 → 2373 (−20%), blocos 144 → 90 | < 1 s |
| memsafety-cve `admesh.i` | 159 | 598 → 589 (−1,5%) | < 1 s |
| ldv-memsafety `ArraysOfVariableLength2.c` | 43 | — | slicer falha em `llvm.stacksave` (VLA) → volta ao programa inteiro |

- **Leitura:** a abordagem é viável e barata; a redução é modesta. Decisão: abordagem 1 do
  2b (fatiar após a instrumentação).

## R7 — tacasv2c: sondagem do slicing para overflow (2026-09-27)

- **Achado de código:** o `OverflowPass` também instrumenta com chamadas de runtime
  `map2check_binop_{add,sub,mul,sdiv,srem,…}` (`OperationsFunctions.hpp`). O mecanismo do
  2b (fatiar o `-output.bc` com todo `map2check_*` como critério) cobre overflow sem lógica
  nova — só o gating.
- **Config:** tarefas NoOverflows do SV-COMP, `--check-overflow`, mesmo comando de R6.

| programa | linhas | chamadas `binop` | instruções antes → depois | funções |
|---|---|---|---|---|
| busybox `chgrp-incomplete-2.i` | 2509 | 23 | 927 → 487 (**−47%**) | 18 → 10 |
| bitvector `byte_add-2.i` | 134 | 16 | 452 → 442 (−2%) | 4 → 4 |
| nla-digbench `bresenham-ll_unwindbound20.c` | 52 | 30 | 175 → 150 (−14%) | 4 → 1 |
| goblint `04-mutex_07-ps_nr.c` | 42 | 2 | 46 → 40 (−13%) | 2 → 2 |

- **Comparação com R6 (memória):** a redução em overflow é maior no programa grande
  (−47% × −20% no busybox de memória). Faz sentido: o critério de overflow são só as
  operações aritméticas instrumentadas, enquanto o de memória são todos os acessos.
- **Leitura:** o 2c deve ser uma extensão pequena do 2b (gating + testes + avaliação em
  CASTLE CWE-190, Juliet CWE-190 e NoOverflows do SV-COMP).

## R8 — tacasv2b: CASTLE completo, controle × slice (2026-09-27)

- **Config:** build 2b (`feat/tacas-slicing-mem`, AFL++ + CmpLog), CASTLE-C250 completo
  (119 casos no escopo), 300 s, `EXTRA_FLAGS=""` × `--slice`. O slice só atua em
  memtrack/memcleanup nesta build (overflow e assert-mode seguem as regras anteriores).

| braço | TP | TN | FN | FP | UNKNOWN | TIMEOUT |
|---|---|---|---|---|---|---|
| v15 (referência) | 54 | 44 | 14 | 1 | 2 | 4 |
| tacasv1 controle | 53 | 44 | 14 | 1 | 7 | 0 |
| tacasv2b slice | 54 | 44 | **15** | 1 | 5 | 0 |

- **Mudanças controle → slice:**
  - `125-3` (memtrack): UNKNOWN em 203 s → **TP em 1,5 s**.
  - `787-2` (memtrack): UNKNOWN em 2,4 s → **FN (TRUE errado)** em 58 s. **Defeito:** o
    estouro acontece dentro de `strcpy` (libc). No controle, o AFL++ viu o crash e o KLEE
    acusou "out of bound pointer" (UNKNOWN honesto); no slice, a chamada a `strcpy` foi
    removida — ela não alimenta nenhuma chamada `map2check_*` e o slicer trata funções
    externas como só-leitura dos argumentos — e o programa virou "seguro".
- **Comparação com a v15:** o controle da tacasv1 perde o TP `125-3` (vira UNKNOWN) e
  troca 4 TIMEOUT por UNKNOWN (o `-V` removido na R3 mudou a forma de terminar).
- **Tempo (memtrack+memcleanup):** mediana 58,1 s × 58,3 s; soma 3432 s × 3377 s.
- **Leitura:** o slicing de memória como implementado **introduz TRUE errado** quando o
  bug está dentro de uma função externa. Correção necessária antes de qualquer conclusão
  (ver R8b).

## R9 — tacasv2b: SV-COMP MemSafety + MemCleanup, controle × slice (2026-09-27)

- **Config:** `build_corpus.py --property memsafety --per-category 10` (50 tarefas:
  Arrays, Heap, LinkedLists, Other, Juliet) e `memcleanup` (10), 120 s, mesmo build de R8.

| memsafety | correct-true | correct-false | wrong-true | wrong-false | unknown | error |
|---|---|---|---|---|---|---|
| controle | 18 | 15 | 2 | 5 | 8 | 2 |
| slice | 20 | 16 | **4** | **6** | 3 | 1 |

- **Mudanças controle → slice:**
  - ganhos: `coreutils od_…_1229` TIMEOUT → FALSE-DEREF correto; `memsafety-ext2/
    optional_data_creation_test04-2` e `forester-heap/sll-buckets-1` UNKNOWN → TRUE correto;
  - **erros novos:** `memsafety-cve/frr.i` e `memsafety-cve/pacparser.i` UNKNOWN → **TRUE
    errado**; `busybox sleep-3.i` UNKNOWN → **FALSE-MEMTRACK errado**.
- **MemCleanup:** idêntico nos dois braços (5 correct-false, 2 correct-true, 2 wrong-true,
  1 error); tempo mediano 13 s → 8,5 s.
- **Redução de instruções (memsafety):** mediana 3%, máximo 90% (45 fatias).
- **Erros já presentes no controle:** 2 wrong-true e 5 wrong-false vêm da própria
  tacasv1, não do slicing (a analisar à parte).
- **Leitura:** mesmo padrão de R8 — ganhos reais, mas TRUE errado novo. Hipótese: as
  mesmas chamadas externas removidas. Correção: toda função **externa** (declarada, sem
  corpo) passa a ser critério nos modos pós-instrumentação.

## Dois defeitos de corretude achados por R8/R9 (2026-09-27)

1. **Chamadas externas removidas pela fatia** (slicing, 2b): um erro de memória dentro de
   `strcpy`/`memcpy`/… não alimenta nenhuma chamada `map2check_*`, e o slicer trata funções
   externas como só-leitura → a chamada saía da fatia e o programa virava "seguro".
   **Correção:** toda função declarada sem corpo é critério nos modos pós-instrumentação.
2. **KLEE parado pelo próprio timer virava TRUE** (verdict, pré-existente na tacasv1 e na
   v15): `--max-time` sai com 0, como uma exploração completa; se um caminho curto gravou
   NONE, a resposta era TRUE. Reproduzido **sem slicing** (null deref alcançável → TRUE).
   O slicing só o expôs (fatias menores deixam o KLEE chegar ao próprio timer).
   **Correção:** KLEE com "HaltTimer invoked" conta como timeout; violação registrada é
   mantida, o resto é UNKNOWN.

As rodadas R8–R13 foram medidas **antes** de uma ou das duas correções: valem como
diagnóstico, não como comparação final (ver R14).

## R9b — MemSafety com a correção 1 (2026-09-27)

| memsafety | correct-true | correct-false | wrong-true | wrong-false | unknown | error |
|---|---|---|---|---|---|---|
| controle (R9) | 18 | 15 | 2 | 5 | 8 | 2 |
| slice R9 | 20 | 16 | 4 | 6 | 3 | 1 |
| slice R9b | 21 | 13 | **4** | 5 | 7 | 0 |

- `frr.i` e `pacparser.i` continuaram TRUE errado → investigação levou ao defeito 2
  (KLEE: "HaltTimer invoked", 0 caminhos completos, 2453 parciais, e mesmo assim TRUE).
- `busybox sleep-3.i` segue FALSE-MEMTRACK errado com slice (UNKNOWN no controle) — em aberto.
- MemCleanup: idêntico ao controle.

## R10 — Juliet escopo C, 1 arquivo/família, controle × slice (build 2b sem as correções)

| braço | TP | TN | FN | FP | UNKNOWN | TIMEOUT | ERROR |
|---|---|---|---|---|---|---|---|
| controle | 143 | 341 | 117 | 12 | 98 | 91 | 40 |
| slice | 133 | 343 | **169** | 12 | 72 | 88 | 25 |

- **Regressão:** FN (vulnerável → TRUE) +52, concentrada em CWE-121 (10 → 44) e CWE-122
  (9 → 26); 74 casos mudaram de classe.
- **Confirmação das correções:** 5 casos que viraram FN, rodados de novo com o build
  corrigido: os 5 passam de TRUE (errado) para UNKNOWN.
- Tempo mediano igual (13,3 s × 13,5 s).

## R11 — CASTLE, build 2c (com a correção 1, sem a 2), controle × slice

- Controle e slice **idênticos**: TP 53, TN 44, FN 14, FP 1, UNKNOWN 7 (overflow: TP 10,
  TN 8, FN 2 nos dois braços).
- **Comparação com R8:** o `787-2` saiu do TRUE errado (R8) para UNKNOWN; o ganho `125-3`
  de R8 não se repetiu.

## R13 — SV-COMP NoOverflows, 10 por categoria (Main, BusyBox), controle × slice

- Controle: 4 correct-true, 16 unknown. Slice: 5 correct-true, 15 unknown. **Zero erros.**
- Ganho: `loop-zilu/benchmark18_conjunctive.i` UNKNOWN → TRUE correto.
- Redução de instruções: mediana 10,2%, máximo 36,7%. Tempo mediano 12 s × 29,5 s.
- **Comparação com R7:** confirma que overflow tem redução maior que memória, mas nesta
  amostra quase tudo é UNKNOWN nos dois braços (orçamento de 120 s).

### Nota — `busybox sleep-3.i` (FALSE-MEMTRACK errado só com slice)

Não é defeito do slicing. Sem slice, o KLEE nem linka ("Linking globals named 'getopt':
symbol multiply defined" — o programa e a uClibc definem `getopt`/`getopt_long`) → UNKNOWN.
A fatia remove essas definições não usadas, o KLEE roda e acusa vazamento. Renomeando os
símbolos em conflito e rodando **sem slice**, o resultado também é FALSE-MEMTRACK: é um
falso positivo pré-existente do memtrack, que o controle escondia por não conseguir rodar.
Em aberto como defeito do memtrack (fora do escopo do slicing).

## R14 — rodada consolidada, controle × slice, com as duas correções (2026-09-27)

- **Build:** `feat/tacas-slicing-overflow` (2b + 2c + correções 1 e 2), os dois braços no
  mesmo build. Slicing ativo para reach, assert, memtrack, memcleanup e overflow.
- **Posterior à R14 (revisão final):** os intrínsecos de memória (`llvm.memcpy/memset/
  memmove`) passaram a ser critério e a fatia é recusada se o IR instrumentado não puder
  ser lido. Isso só acrescenta critério; o braço slice de MemSafety foi refeito em R14b.

| corpus | controle | slice | mudanças |
|---|---|---|---|
| CASTLE (119) | TP 53, TN 44, FN 14, FP 1, UNK 6, ERR 1 | **idêntico** | nenhuma; tempo mediano 56,8 × 57,0 s |
| MemSafety SV-COMP (50)* | 33 corretos, wrong-true 2, wrong-false 3 | 33 corretos, **wrong-true 1**, wrong-false 4 | 4 casos (abaixo) |
| MemCleanup (10) | 6 corretos | **idêntico** | tempo mediano 14 × 8 s |
| NoOverflows (20) | 4 corretos, 16 unknown | **idêntico** | — |
| Juliet escopo C (842) | TP 141, FN 117, TN 342, FP 12, UNK 100, TO 93, ERR 37 | TP 141, **FN 117**, TN 342, FP 12, UNK 90, TO 93, ERR 47 | 12 casos, todos CWE-121 |

\* reclassificado: as tarefas `Juliet_Test` do SV-COMP não declaram subpropriedade e o
classificador contava todo FALSE nelas como errado (corrigido: "any").

- **MemSafety, as 4 mudanças:** `csplit` FALSE-DEREF correto → TIMEOUT (perda);
  `CWE122 …rand_18_bad` **TRUE errado → FALSE-DEREF correto** (ganho); `CWE127 memmove`
  ERROR → UNKNOWN; `busybox sleep-3` UNKNOWN → FALSE-MEMTRACK (falso positivo pré-existente
  do memtrack, exposto porque a fatia deixa o KLEE linkar — ver nota acima).
- **Juliet, as 12 mudanças (CWE-121):** 2 UNKNOWN → TP (ganho); 2 TP → ERROR e 8 UNKNOWN
  → ERROR. Os TP perdidos eram detecções **acidentais**: o `memcpy` estoura a pilha e
  sobrescreve um ponteiro vizinho ("Reference to pointer was lost"); a fatia muda o
  layout da pilha e o acidente some — o memtrack não confere limites dentro da libc.
  Resultado honesto (UNKNOWN), classificado ERROR pelo runner do Juliet por causa do
  `KLEE: ERROR` do `memcpy`.
- **Comparação com R8–R13:** o TRUE errado que o slicing introduzia sumiu (Juliet FN 169
  na R10 → 117 = controle; CASTLE 787-2 e memsafety-cve frr/pacparser resolvidos). O
  controle também mudou por causa da correção 2: MemSafety correct-true 18 (R9) → 15,
  porque três TRUE "de sorte" (KLEE parado pelo timer) agora saem UNKNOWN.
- **Leitura:** nos corpora atuais o slicing é **neutro** em acertos, com trocas pontuais
  (ganhos e perdas em números iguais) e **sem TRUE errado novo**; fica mais rápido em
  MemCleanup. Os programas são pequenos e a fatia tem pouco a cortar (MemSafety: redução
  mediana 3%). O ganho medido de verdade desta linha está nas correções de corretude que o
  slicing expôs.

## R14b — MemSafety/MemCleanup, braço slice com o build final (2026-09-27)

- **Mudança desde R14:** intrínsecos de memória (`llvm.memcpy/memset/memmove`) como
  critério e recusa da fatia quando o IR instrumentado não pode ser lido (revisão final).
  Mesmas tarefas e controle de R14; classificação com a regra "any" do Juliet.

| memsafety (50) | correct-true | correct-false | wrong-true | wrong-false | unknown | error |
|---|---|---|---|---|---|---|
| controle (R14) | 15 | 18 | 2 | 3 | 9 | 3 |
| slice (R14) | 15 | 18 | 1 | 4 | 10 | 2 |
| **slice (R14b)** | 15 | **19** | **1** | 4 | 9 | 2 |

- **Controle → R14b:** `CWE122 …rand_18_bad` TRUE errado → FALSE-DEREF correto (ganho);
  `CWE127 memmove` ERROR → UNKNOWN; `busybox sleep-3` UNKNOWN → FALSE-MEMTRACK (falso
  positivo pré-existente do memtrack). A perda do `csplit` de R14 não se repetiu.
- **MemCleanup:** idêntico ao controle; tempo mediano 14 s → 7 s.
- **Leitura:** com o build final, o slicing de memória fica **34 × 33 corretos** e
  **1 × 2 TRUE errados** contra o controle nesta amostra — ligeiramente melhor, e sem
  nenhum erro novo atribuível à fatia.

## R15 — amostra Test-Comp: tacasv2 (develop) × v15, Cover-Error (2026-09-28)

- **Config:** build da `develop` depois do merge de #69/#68/#70/#71; `build_corpus.py
  --property cover-error --per-category 20` (213 tarefas, **todas pareadas** com a
  campanha v15 — amostragem aninhada), 300 s, TestCov 300 s, 3 shards por braço.
  Controle = híbrido sem slice (tacasv1 + correções); slice = `--slice`.

| braço | cobertas | TRUE errado | FALSE errado | tempo mediano |
|---|---|---|---|---|
| v15 controle | 111 | **27** | 0 | 43 s |
| v15 slice | 108 | 15 | 0 | — |
| **R15 controle** | **129** | **5** | 0 | **5 s** |
| R15 slice | 126 | 4 | 0 | 7 s |

- **Controle × v15:** +18 cobertas (+16%); pares discordantes 20 × 2 (só R15 × só v15).
  Por categoria (v15 → R15): ProductLines 14 → 20, Loops 13 → 16, Arrays 15 → 18,
  ECA 2 → 5, Sequentialized 0 → 2; Heap 19 → 18 (única perda).
- **TRUE errado 27 → 5:** a correção "KLEE parado pelo timer não é prova" confirmada em
  escala — a v15 respondia TRUE em 27 de 212 tarefas com bug.
- **Slice × controle:** 126 × 129 (pares discordantes 3 × 6) — empate técnico, leve
  desvantagem. TRUE errado 4 × 5: o slice elimina 2 (`pals_lcr…`) e introduz 2
  (`float-benchs/cast_union_tight.c`, `loops/insertion_sort-1-2.c`); 4 ERROR (ECA,
  ~330 s, sem suíte) só no slice.
- **Em aberto:** os 4–5 TRUE errados restantes (`seq-mthreaded/pals_*`,
  `float-benchs/sin_interpolated_index-1.c`, e os 2 do slice) e os 4 ERROR do slice.

### R15 — Cover-Branches, controle × v15 (mesma rodada)

- 120 tarefas (10 por categoria), todas pareadas com a v15; só o braço controle (o
  slicing não atua em Cover-Branches).
- **Cobertura média: v15 48,0% × R15 47,7%** — neutra. Melhor em 6 tarefas, pior em 3,
  igual em 111. Recursive 51,9 → 45,0% (a única queda relevante); XCSP 74,2 → 77,7%.
- Validação do TestCov mais limpa: VALIDATED 116 (v15: 107), VALIDATED_ABORTS 3 (v15: 10),
  TESTCOV_ERROR 1 (v15: 3). Tempo mediano 183 × 195 s.
- **Leitura:** o AFL++ não muda a cobertura de ramos nesta amostra; o ganho da tacasv2
  está no Cover-Error.

## R18 — validação da correção do abort() nos TRUE errados da R15 (2026-09-29)

- **Config:** `build_abort` (commit 1eb8a30b9), 300 s, as tarefas com TRUE errado da R15:
  6 no braço controle (sem slice), 2 no braço slice (`--slice`). Manifests
  `tacas-results/r18-{ctrl,slice}.tsv`.

| tarefa | braço | R15 | R18 |
|---|---|---|---|
| pals_lcr.4.1 | controle | TRUE errado | FAILED/COVERED (83 s) |
| pals_lcr-var-start-time.4.2 | controle | TRUE errado | FAILED/COVERED (175 s) |
| pals_STARTPALS_Triplicated.1 | controle | TRUE errado | FAILED/COVERED (9 s) |
| pals_floodmax.3.1 | controle | TRUE errado (R16) | FAILED/COVERED (24 s) |
| pals_floodmax.3.4 | controle | TRUE errado | UNKNOWN (256 s) |
| sin_interpolated_index-1 | controle | TRUE errado | **TRUE errado** (59 s) |
| cast_union_tight | slice | TRUE errado | FAILED/COVERED (1 s) |
| insertion_sort-1-2 | slice | TRUE errado | **TRUE errado** (3 s) |

- **Leitura:** 6 de 8 TRUE errados eliminados (5 viraram FAILED coberto, 1 UNKNOWN).
  `cast_union_tight` também era abort inline, não defeito do slice. Restam 2 com causa
  diferente: `sin_interpolated_index-1` (controle) e `insertion_sort-1-2` (slice) — investigar.

### R18 — causa dos 2 TRUE errados restantes (2026-09-29)

Reproduzidos à mão (`build_abort`, 300 s). **Causa comum:** o KLEE sai com 0 quando a fila
esvazia, e o frontend lia isso como exploração completa — mas nos dois casos ele tinha
**descartado caminhos**:

- `sin_interpolated_index-1` (controle): `silently concretizing (reason: floating point)
  expression (ReadLSB w64 0 non_det_double) to value 0` — o KLEE 3.1 não tem double
  simbólico; 1 caminho, "completo", TRUE.
- `insertion_sort-1-2` (slice): o VLA `int v[SIZE]` gerou `concretized symbolic size` e
  `null page access` — 2 estados mortos por erros do próprio KLEE (`partially completed
  paths = 2` no `info`), TRUE.

**Correção:** `kleeDroppedPaths()` generaliza o `kleeHaltedOnTimer`: HaltTimer, qualquer
"silently concretizing" em `warnings.txt`, ou `partially completed paths > 0` → a execução
é tratada como timeout (violação registrada vale; senão UNKNOWN). Revalidação: `sin` → **FAILED** numa execução e UNKNOWN noutra (o AFL++ da fase 1 acha ou
não o 180.0; sem `--seed-exchange` não há fase 3 — a frase anterior dizia o contrário e
estava errada), `insertion_sort` --slice → **UNKNOWN**. Nunca TRUE. Integração 40/40 (§30 nova: double concretizado; vermelha no
`build_seeds`, verde no novo). Uma §31 (abort dentro do `assert` da libc) foi descartada:
passava também no binário antigo — esse caminho não gera TRUE errado.

**Custo esperado:** programas com float/VLA que o KLEE "provava" passam a UNKNOWN. Medir
TRUE corretos no SV-COMP (R17) — é o preço da solidez.

## R16 — `--seed-exchange` (3a) × controle R15, Cover-Error (2026-09-29)

- **Config:** `build_seeds` (3a + revisão, **sem** as correções do abort e dos caminhos
  descartados — mesma base de veredito que o controle R15), `--seed-exchange`, amostra
  `r15-ce.tsv` (213, todas pareadas), 300 s, 3 shards.

| braço | cobertas | TRUE errado | ERROR | tempo mediano |
|---|---|---|---|---|
| R15 controle | 129 | 5 | 0 | 5 s |
| **R16 seeds** | **148** | 7 | 0 | 6 s |

- **+19 cobertas (+15%)**, pares discordantes **21 × 2**. Por categoria (controle → seeds):
  Sequentialized 2 → 8, XCSP 7 → 13, ECA 5 → 8, Recursive 7 → 9, ControlFlow 3 → 4,
  BitVectors 7 → 8; as demais iguais. Perdas: 2 ECA (`Problem13_label54`,
  `Problem10_label12`).
- **TRUE errado 7:** todos da família `pals_*` (abort inline) e `sin_interpolated_index-1`
  (double concretizado) — os dois defeitos que esta branch já corrige (1eb8a30b9,
  4c16013bc). O braço seeds expõe mais deles porque a fase 3 do AFL++ não roda depois de
  uma "prova" do KLEE.
- **Leitura:** a troca de sementes é o maior ganho medido na linha TACAS até aqui. A
  rodada limpa (R19, build final nos dois braços) confirma sem os TRUE errados.
- Cover-Branches do R16 ainda rodando.

### Correção da correção — poda por assunção não é caminho descartado (2026-09-29)

A primeira versão do `kleeDroppedPaths` lia `partially completed paths > 0` no `info`. Esse
contador inclui os caminhos **podados por assunção**: `klee_assume(0)` num caminho já falso
é um erro do KLEE (`user.err`, "invalid klee_assume call (provably false)"), e até
`klee_silent_exit` conta como parcial (medido num programa mínimo: os dois dão
`partially completed paths = 1`). Resultado: **nenhum programa com `assume_abort_if_not`
ou abort inline podia mais ser provado** — o §29 `safe.c` ia de TRUE para UNKNOWN.

Correção: `nondet_assume` (KLEE) poda com `klee_silent_exit(0)`, que não deixa arquivo; e
os caminhos descartados passam a ser lidos pelo que cada estado morto deixa no disco —
qualquer `*.err` ou `*.early` —, além do HaltTimer e do "silently concretizing". O §29
agora exige TRUE. `sin` e `insertion_sort` seguem sem TRUE errado (UNKNOWN; o segundo por
`ptr.err`).

**Efeito na R19:** o `install_r19` tem a versão com o contador. Em Test-Comp isso não muda
a cobertura (o veredito não pontua), só impede o `provedSafe` de encerrar a execução mais
cedo em programas com assunções. A R17 (SV-COMP, onde TRUE pontua) precisa do build
corrigido.

### R16 — Cover-Branches, `--seed-exchange` × controle R15 (2026-09-29)

- Mesma config da R16 Cover-Error (`build_seeds`), amostra `r15-cb.tsv` (120, pareadas).
- **Cobertura média: controle 47,7% × seeds 49,7%** (+2,0 p.p.); melhor em 26 tarefas, pior
  em 9. Tempo mediano 195 → 160 s.
- Por categoria (controle → seeds): ControlFlow 41,3 → 57,9, BitVectors 65,7 → 73,7,
  Recursive 45,0 → 53,0; **Loops 67,1 → 60,8** (uma tarefa: `geo2-ll_unwindbound50`
  −62,5 p.p.), XCSP 77,7 → 75,0 (`AllInterval-011` −27 p.p.).
- TestCov: VALIDATED 118 (R15: 116), VALIDATED_ABORTS 1 (3).
- **Leitura:** diferente da tacasv2 (neutra em CB), a troca de sementes melhora a cobertura
  de ramos, apesar de a suíte de CB sair só do KLEE — o KLEE semeado explora ramos que o
  controle não alcançava. As duas quedas grandes ficam para a R19 confirmar (ruído do
  fuzzer × efeito real).

## R17 — SV-COMP MemSafety/MemCleanup/NoOverflows, control × seeds × alternate (2026-09-29)

- **Config:** `install_r19` (commit 8c70e5a73), manifests regenerados com `build_corpus.py`
  (memsafety 10/categoria = 50, memcleanup 10, overflow 10/categoria = 20), 120 s.

| memsafety (50) | correct-true | correct-false | wrong-true | wrong-false | unknown | error |
|---|---|---|---|---|---|---|
| control | 10 | 19 | 2 | 3 | 10 | 6 |
| seeds | 10 | 20 | 1 | 3 | 10 | 6 |
| alternate | 9 | 20 | 1 | 3 | 10 | 7 |

- MemCleanup (10): control 6 corretos, seeds 6, alternate 6. NoOverflows (20): control e
  seeds 2 correct-true + 17 unknown; alternate idem (2 + 17 + 1 error) — os três iguais.
- **wrong-true comum aos três:** `CWE121…CWE193_char_declare_cpy_07_bad`. Causa: o modelo
  `ldv_strcpy` copia `strlen` bytes (sem o terminador) e o estouro real é a **leitura** em
  `printf("%s")` — que o KLEE executa como **chamada externa** (a uClibc do KLEE declara
  `printf` sem defini-lo: `calling external: printf(...)`), fora de qualquer checagem, e o
  memtrack não confere argumentos `%s`. Lacuna de solidez pré-existente; correção em
  aberto: checar a string de cada `%s` (e `puts`/`str*`) antes da chamada.
- **wrong-true só no control:** `CWE122…CWE129_rand_18_bad` — os braços com sementes o
  acham (FALSE correto).
- **Os 6 `error` são do classificador, não da ferramenta:** as tarefas
  `array-memsafety/*-alloca` têm `alloca` de tamanho não determinístico; o AFL++ acha um
  crash (pilha estourada), o replay imprime "Segmentation fault", e
  `verdict_classifier.sh` conta isso como falha — embora o Map2Check termine com UNKNOWN
  (o KLEE concretiza o tamanho → `model.err` → caminho descartado).
- **Leitura:** as sementes não pioram nada em SV-COMP e ganham 1 FALSE e eliminam 1 TRUE
  errado; a alternância perde 1 TRUE correto frente ao control (a estagnação corta o KLEE
  — o preço previsto na revisão).

## R19 — Test-Comp, parcial (2026-09-29)

- **Cover-Error, 171 tarefas pareadas nos 4 braços principais** (o shard 0 do braço seeds
  perdeu 42 tarefas por um incidente de escrita e está sendo completado):

| braço | cobertas | TRUE errado | ERROR | tempo mediano |
|---|---|---|---|---|
| R15 control (referência) | 101 | 5 | 0 | 6 s |
| control | 101 | **0** | 0 | 3 s |
| **seeds** | **120** | 0 | 0 | 5 s |
| alternate | 118 | 0 | 0 | 14 s |
| slice | 102 | 0 | 2 | 4 s |

- seeds × control: **+20 −1**; alternate × control: +17 −0; alternate × seeds: +4 −6;
  slice × control: +3 −2.
- **TRUE errado zerado em todos os braços** (eram 5 no control R15): as correções do abort e
  dos caminhos descartados confirmadas em escala.
- Braços slice-light/o2/ntscd/ptafs e Cover-Branches ainda rodando.

### R19 — diagnóstico das perdas de `--alternate-engines` (2026-09-29)

Contra o braço seeds, a alternância perde 6 e ganha 2; 5 das perdas são eca-*. Reproduzido
em `eca-rers2012/Problem06_label05.c` (300 s):

- **A troca KLEE → AFL++ estava limitada** aos 64 vetores mais recentes do KLEE. No braço
  seeds, a fase 3 do AFL++ recebe todos (4676), e o dry run dela encontra vetores que,
  completados com zeros depois do fim, chegam ao `reach_error` (`sig:06`, crashes com
  `op:dry_run`). É isso que cobre essas tarefas eca-* que o KLEE sozinho deixa UNKNOWN.
  O limite descartava justamente esses vetores. Ele tinha vindo de um laço cuja calibração
  nunca terminava, problema já resolvido com a leitura de zeros.
- **Cada fase do AFL++ recompilava os 3 binários:** ~24 s por fase em eca-*.
- **A estagnação fixa de 15 s cortava o AFL++ em ~17 s**, onde o híbrido fixo dava 60 s.

**Correção** (`fix(hybrid): the fuzzer gets all of KLEE's vectors, built once, with growing
patience`): sem limite na troca, cache dos binários por execução (`<hash>.build/`, também
beneficia a fase 3 do braço seeds) e paciência do AFL++ dobrando por rodada.
`Problem06_label05`: UNKNOWN → **FAILED em 157 s** (seeds: 281 s). Remedição na R23.

## R19 — resultado (2026-09-29)

**Cover-Error, 213 tarefas** (linhas filtradas pelo shard; os shards afetados pelo incidente
do descritor 3 foram completados com `-resume`):

| braço | cobertas | % | TRUE errado | ERROR | vs control | tempo mediano |
|---|---|---|---|---|---|---|
| control | 128 | 60,1 | 0 | 0 | — | 3 s |
| **seeds** | **151** | **70,9** | 0 | 0 | **+23 −0** | 5 s |
| alternate | 148 | 69,5 | 0 | 0 | +20 −0 | 9 s |
| slice | 128 | 60,1 | 0 | 4 | +3 −3 | 4 s |
| slice-light | 127 | 59,6 | 0 | 4 | +3 −4 | 4 s |
| slice-o2 (207) | 128 | 61,8 | 0 | 4 | +5 −3 | 4 s |
| slice-ntscd (181, parcial) | 113 | 62,4 | 0 | 4 | +2 −1 | 5 s |
| slice-ptafs (155, parcial) | 90 | 58,1 | 0 | 4 | +1 −3 | 5 s |

- **TRUE errado 0 em todos os braços** (R15 control: 5). As correções de veredito se
  confirmam em escala.
- **Sementes: +23 −0** contra o control, o melhor resultado da linha TACAS. Alternância:
  +20 −0. As 5 perdas eca-* dela frente ao seeds têm causa achada e corrigida (ver o
  diagnóstico acima); a remedição fica para a R23.
- **Slicing:** as variantes não mudam o quadro (±3). Nenhuma supera o slice simples com
  margem, e nenhum knob é promovido por enquanto. Os 4 ERROR de ECA continuam: o cache do
  2d eliminou o refatiamento, mas o passo do slice ainda levava 105 s (regex sobre o IR,
  ~45 s fora de qualquer orçamento) e as 3 compilações do AFL++ somavam até 0,75T.
  Correções: varredura sem regex e um orçamento único para as compilações.
  `Problem102_label34` com `--slice`: ERROR → UNKNOWN em 307 s.

**Cover-Branches, 120 tarefas:**

| braço | cobertura média | melhor / pior que o control |
|---|---|---|
| control | 44,7% | — |
| seeds | 46,1% | 31 / 13 |
| **alternate** | **50,3%** (119) | **43 / 5** |

- **A alternância é o melhor braço em Cover-Branches** (+5,6 p.p.), o que se explica pelas
  várias fases do KLEE, cada uma alimentada pelo corpus do fuzzer. O control da R19 (44,7%)
  ficou abaixo do da R15 (47,7%) na mesma amostra. A carga da máquina foi maior (11
  contêineres e falta de memória no fim), e isso pesa em Cover-Branches, que usa o
  orçamento inteiro.

## R21 — checagem de `%s` e correção do classificador, SV-COMP (2026-09-29)

- `install_r21`, control, contra a R17 control.
  - **MemSafety:** o CWE193 cpy bad foi de TRUE errado para **FALSE correto**. Os 6
    `error` das tarefas alloca viraram `unknown` (classificador corrigido).
  - **Falso positivo novo:** `CWE121…dest_char_declare_cpy_01_good` foi de TRUE correto
    para FALSE-DEREF errado. O `ldv_strcpy` do SV-COMP copia `strlen` bytes sem o
    terminador, e o terminador do buffer da versão good é um byte **não inicializado**:
    para o gabarito do SV-COMP ele é zero; numa execução nativa ou no KLEE (que preenche
    `alloca` com um padrão diferente de zero) não é.
  - **MemCleanup:** os 2 `error` viraram `unknown`.
- **Decisão:** a checagem de `%s` fica atrás de `MAP2CHECK_CHECK_CSTRINGS=1`, desligada
  por padrão, até ser medida numa amostra maior do Juliet (1 acerto × 1 erro em 50 não
  basta; pelos pesos do SV-COMP compensaria, mas não com essa amostra).
- **Incidente de harness (descritor 3):** o laço do harness lê o manifest pelo fd 3, e os
  filhos o herdavam, inclusive o programa analisado via chamadas externas do KLEE. O offset
  andou sob o laço: o shard 0 da R24 parou em 29 de 71, e o shard 0 do seeds da R19 recebeu
  linhas do shard 1. Os filhos agora rodam com o fd 3 fechado.

## INV-1 — `--add-invariants`: crab-llvm antigo × Clam (2026-09-30)

**Pergunta:** o `--add-invariants` do crab-llvm "funcionava", e o do Clam "não é a mesma
coisa"?

**O que se descobriu sobre o motor antigo** (release v7.3.1 do SV-COMP 2020, que roda em
`python:2.7-slim` com o clang do LLVM 6 embutido):
- Houve **duas configurações**.
  - Até 18/10/2018: `--crab-track=arr --crab-add-invariants=after-load`. Os invariantes
    saíam como `verifier.assume`, o NonDetPass os mapeava para `map2check_crab_assume`, e
    eles chegavam ao KLEE como `klee_assume`.
  - A partir da v7.3 (SV-COMP 2019 e 2020): `--crab-track=num
    --crab-add-invariants=block-entry --crab-promote-assume`. O `promote` emite
    `llvm.assume`, que o NonDetPass não mapeia e que o KLEE ignora (testado no KLEE 2.1 do
    release e no 3.1, com e sem `--optimize`). **Os invariantes das versões de competição
    não chegavam a lugar nenhum.**

**Geração, 90 programas** (40 de alcançabilidade da amostra Cover-Error e 50 de MemSafety),
timeout de 60 s:

| config | rodou | com invariante | total |
|---|---|---|---|
| OLD-A (antigo, até 2018) | 76 | 14 | 155 |
| OLD-B (antigo, v7.3; `llvm.assume`, inerte) | 35 | 33 | 224 698 |
| NEW-cur (Clam, `num` + `block-entry`) | 79 | 68 | 72 452 |
| NEW-A (Clam, `mem` + `after-load`) | 70 | 19 | 702 |

- A configuração que "funcionava" insere **poucos** invariantes, depois de leituras de
  memória. A atual do Clam insere **muitos**, na entrada de cada bloco. O perfil
  `memory` do Clam reproduz a ordem de grandeza da antiga.

**Efeito na análise, sonda num laço** (`n ≤ 1000`, versões segura e com bug em `n == 777`,
symex, 60 s):

| braço | seguro | com bug |
|---|---|---|
| sem `--add-invariants` | UNKNOWN (HaltTimer) | UNKNOWN |
| Clam padrão (18 invariantes) | UNKNOWN | UNKNOWN |
| Clam `memory` (0 invariantes) | **TRUE** | **FAILED** |
| Clam `none` (pipeline do Clam, 0 invariantes) | **TRUE** | **FAILED** |
| **sem Clam, `MAP2CHECK_PREOPT=ssa`** | **TRUE** | **FAILED** |

- **O ganho vem do pré-processamento, não dos invariantes.** O Clam compila com
  `-disable-O0-optnone` e deixa o módulo em SSA (`mem2reg`, `simplifycfg`). O Map2Check
  compila em `-O0` com `optnone`, cada variável local vira um objeto de memória no KLEE, e
  o laço não fecha no orçamento.
- Os 18 invariantes do perfil padrão não bastaram: o KLEE completou mais caminhos com eles
  (261 contra 184 sem), mas também parou no HaltTimer.
- **Hipótese a medir em escala (R25):** `MAP2CHECK_PREOPT=ssa` (sem Clam) é um ganho
  geral, e os invariantes só se pagam somados ao SSA, se se pagarem.
- Implementado nesta frente (`feat/tacas-invariants`):
  - `MAP2CHECK_CLAM_PROFILE=default|memory|none`;
  - log com a contagem de invariantes inseridos;
  - recuo para a compilação normal quando o Clam falha (antes, o pipeline ficava sem
    bitcode);
  - `MAP2CHECK_PREOPT=ssa`.

## R20, R22, R23, R24 — primeiros resultados (2026-09-30, madrugada)

Todos contra o braço equivalente da R19, nas tarefas em comum (linhas filtradas por shard).

| rodada | mudança medida | tarefas | cobertas (nova × R19) | +/− | TRUE errado |
|---|---|---|---|---|---|
| **R24 control** (completa) | replay dos vetores do KLEE, híbrido **sem** sementes | 213 | **146 × 128** | **+20 −2** | 0 |
| R24 seeds (parcial) | replay + sementes | 59 | 47 × 46 | +1 −0 | 0 |
| R20 seeds (parcial) | ranking `+cov` (3c) | 178 | 128 × 128 | +2 −2 | 0 |
| R23 seeds (parcial) | build final (cache do AFL++, orçamento, fd 3) | 177 | 132 × 128 | +5 −1 | 0 |

- **O replay dos vetores do KLEE traz o híbrido simples para perto do braço com sementes**
  (146 contra 151 da R19 seeds). As mudanças concentram-se em `pals_*`, eca-*, XCSP (`aim-*`,
  `CostasArray`, `AllInterval`) e `fuzzle`, justamente as tarefas em que o seeds ganhava
  com o dry run do AFL++. O mecanismo diagnosticado se confirma.
- **3c v1 (ranking):** neutro nesta amostra (+2 −2). O ranking só pesa quando a fila passa
  de 64 entradas.
- **Cover-Branches, R22** (corpus do AFL++ na suíte, `MAP2CHECK_FUZZER_SUITE=1`):
  - control, 99 tarefas: **49,2% × 44,8%**, 33 melhores e 2 piores; TestCov VALIDATED
    97/99;
  - seeds, 22 tarefas: 48,5% × 44,8%, 4 melhores e 3 piores.

## R21 CASTLE e dois incidentes (2026-09-30, madrugada)

- **CASTLE com `install_r21`** (checagem de `%s` ainda **ligada** nesse build): TP 54, TN 44,
  FN 12, UNKNOWN 5, **FP 4** (R14: TP 53, TN 44, FN 14, FP 1). Os 4 FP (787-1, 787-2,
  787-4, 822-3) são todos um `printf("%s")` de memória que o runtime não rastreia: buffer
  preenchido por `scanf` e `argv[0]`. É uma confirmação independente de que
  `MAP2CHECK_CHECK_CSTRINGS` deve continuar **desligado** (como está no branch da PR); os
  FN caíram de 14 para 12.
- **Incidente do escalonador** (corrige uma conclusão anterior): os escalonadores leem os
  jobs com `IFS=$'\t' read`, e o bash junta tabs consecutivos. Com a coluna de flags vazia,
  o conteúdo da coluna de ambiente escorregava para a de flags. Com isso:
  - o braço `ssa` da R25 passou `MAP2CHECK_PREOPT=ssa` ao Map2Check como **nome de
    arquivo** (71/71 ERROR). Relançado com `-e`;
  - **a primeira R22 control (120 ERROR) teve a mesma causa, e não falta de memória**,
    como registrado antes. O relançamento dela usou `-e` e é válido;
  - os outros braços têm flags não vazias ou nenhuma variável, e não foram afetados.

### R20 e R23 seeds, completas (2026-09-30, 01h)

| rodada | o que muda | cobertas (213) | vs R19 seeds (151) | vs R19 control (128) | TRUE errado |
|---|---|---|---|---|---|
| R20 seeds | + ranking `+cov` (3c v1) | 150 | +2 −3 | +22 | 0 |
| **R23 seeds** | build final (cache do AFL++, fd 3, hash, orçamento) | **154** | **+5 −2** | **+26** | 0 |

- **3c v1 é neutro** também na amostra completa: fica disponível, sem ganho medido.
- **O build final com sementes é o melhor resultado de Cover-Error da linha: 154/213
  (72,3%)**, contra 111 da v15 na mesma amostra.
- As perdas recorrentes (`Problem10_label12`, `Problem13_label54`) aparecem também no
  R20. São tarefas eca-* no limite do orçamento.

## Madrugada de 2026-09-30 — R22, R23, R24 e R25 completas

**Cover-Error, 213 tarefas** (referência: R19 control, 128):

| braço | cobertas | % | vs R19 control | TRUE errado | tempo mediano |
|---|---|---|---|---|---|
| R24 control (replay dos vetores do KLEE) | 146 | 68,5 | +20 −2 | 0 | 4 s |
| R23 seeds (build final) | 154 | 72,3 | +26 −0 | 0 | 4 s |
| **R24 seeds (build final + replay)** | **155** | **72,8** | **+27 −0** | 0 | 5 s |
| R23 alternate (build final) | 153 | 71,8 | +26 −1 | 0 | 12 s |

- R23 alternate × R19 alternate: +7 −2. A correção das perdas eca-* funcionou.
  R23 alternate × R23 seeds: +4 −5, um empate.
- **v15 → build final na mesma amostra: 111 → 155 cobertas, TRUE errado 27 → 0.**

**Cover-Branches, 120 tarefas** (R19 control 44,7%):

| braço | cobertura média | melhor / pior |
|---|---|---|
| R19 seeds | 46,1% | 31 / 13 |
| **R19 alternate** | **50,6%** | 44 / 5 |
| R22 control + corpus do AFL++ | 49,7% | 40 / 3 |
| R22 seeds + corpus do AFL++ | 49,4% | 36 / 8 |

**R25 (`--add-invariants` e SSA), shard 0 de Cover-Error (71 tarefas) e MemSafety (50):**

| braço | Cover-Error cobertas | ERROR | MemSafety (corretas / wrong-true / wrong-false) |
|---|---|---|---|
| off | 51 | 0 | 28 / 2 / 2 |
| ssa | 50 (+2 −3) | 0 | — |
| clam-none | 50 (+1 −2) | 2 | 25 / 1 / 4 |
| clam-default | 49 (+1 −3) | 4 | 27 / 1 / 4 |
| clam-memory | 44 (+2 −9) | 7 | 25 / 2 / 4 |

- **No híbrido, nem o SSA nem os invariantes ajudam.** O ganho que a sonda viu era do KLEE
  sozinho; no híbrido, o AFL++ e o replay dos vetores já decidem essas tarefas. Os braços
  com Clam **pioram**: mais FALSE errados em MemSafety (4 contra 2), e ERROR por o Clam
  estourar o orçamento, já corrigido com um limite de 0,2T.
- **Decisão:** `--add-invariants` continua opcional e desligado por padrão; o SSA não é
  promovido.
- **Incidente:** a mudança de versão no `CMakeLists.txt` fez o cache do `build_inv` ser
  regenerado, com o prefixo voltando para `release/` e o `ENABLE_CLAM` para OFF. O
  `release/` foi sobrescrito de novo e restaurado a partir do `install_v15` (idêntico,
  conferido com `diff`). As rodadas usaram installs congelados antes disso.
