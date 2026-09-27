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
