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
