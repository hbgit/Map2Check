# tacas 2d — otimizações do slicing (design)

**Data:** 2026-09-29 · **Branch:** `feat/tacas-2d-slicing` (a partir de `feat/tacas-smart-seeds`)
**Aprovação:** o usuário delegou as decisões ("você tem permissão para fazer as decisões
que quiser") e pediu as abordagens recomendadas no brainstorming de 2026-09-29.

## Objetivo

Hoje o `--slice` só empata com o controle: 126 × 129 cobertas na R15 Cover-Error. Além
disso, ele tem 4 ERROR próprios (ECA). O objetivo é que ele passe a ganhar, sem nenhum
TRUE errado.

## Evidência que orienta o desenho

- **Os 4 ERROR de ECA** (`Problem08_label51`, `Problem102_label34`, `Problem102_label02`,
  `Problem103_label44`) seguem o mesmo roteiro:
  - o `sbt-slicer` estoura seu limite (0,2T = 60 s) na fase 1;
  - a fase 2 recria o `Caller` e **tenta fatiar de novo**, perdendo mais 60 s;
  - o processo passa do orçamento e o harness o mata aos 330 s, sem suíte.
- **Cada fase refatia do zero.** Numa tarefa em que o slicer funciona, isso custa o tempo
  do slicer uma vez por fase (2× no híbrido, 3× com `--seed-exchange`).
- **O dg deixa blocos vazios e funções mortas no `.bc` fatiado.** Nenhuma limpeza roda
  depois da fatia.

## Desenho

### 1. Fatiar uma vez por execução (cache do slice)

- **Onde fica:** um diretório `<cwd>/<programHash>.slice/` ao lado do scratch, no mesmo
  esquema do store de sementes, porque o scratch é recriado a cada fase.
- **Chave:** um hash (FNV-1a 64 bits, em hex) do **conteúdo do `.bc` de entrada**, somado
  ao rótulo dos critérios, à função de entrada e às flags extras do slicer. Chavear pelo
  conteúdo, e não pelo número da fase, garante que só se reaproveita a fatia de uma
  entrada idêntica.
- **O que se guarda:**
  - `<chave>.bc`: a fatia pronta, depois da limpeza (item 2);
  - `<chave>.failed`: marca que o slicer falhou ou estourou o tempo. Com ela, as fases
    seguintes vão direto para o programa inteiro, sem gastar o slicer de novo.
- **Consulta:** acontece em `Caller::runSlicer`.
  - Um acerto copia a fatia e registra "reusing the slice from an earlier phase".
  - Uma falha registrada devolve `false` com o aviso de sempre, acrescido de "(cached)".
- **Ciclo de vida:** o mesmo do store.
  - O diretório é apagado no início da execução (fase ≤ 1).
  - O guarda de escopo de `main` o remove no fim, exceto com `--debug`.
  - O guarda passa a cuidar dos dois diretórios, o de sementes e o de slice.
  - Só existe com `--slice`.

### 2. Limpeza depois da fatia (experimental)

- **Knob de ambiente:** `MAP2CHECK_SLICE_CLEANUP`, com os valores:
  - `none` (padrão até a medição);
  - `light`: `opt -passes='function(simplifycfg,dce),globaldce'`;
  - `o2`: `opt -O2`.
- **Onde roda:** dentro de `runSlicer`, depois de uma fatia bem-sucedida e antes de ela
  entrar no cache.
- **Se o `opt` falhar,** a fatia fica sem limpeza e a execução avisa.
- **Risco do `o2`:** ele explora comportamento indefinido e pode apagar laços sem efeito
  colateral. Isso muda a alcançabilidade. Por isso é só um braço de experimento, e o
  gate de TRUE errado = 0 decide.
- **Depois da medição,** o vencedor vira o padrão (commit separado).

### 3. Parâmetros do dg (experimental)

- **Knob de ambiente:** `MAP2CHECK_SLICER_FLAGS`, anexado sem alteração à linha do
  `sbt-slicer`. Os braços medidos são `--cda=ntscd` e `--pta=fs`.
- **Entra na chave do cache.**
- **Depois da medição,** o vencedor vira o padrão (commit separado).

### Fora do escopo

- **Cutoff-diverging consertado:** risco alto (é a causa do "Broken module"). Só entra
  se 1–3 não fecharem a diferença.
- **Cache dos demais artefatos** (binário do AFL++, `.bc` do KLEE): compilar e instrumentar
  custa segundos. Fica para o 3b, se as rodadas extras mostrarem esse custo.

## Testes

- **Unitários** (`SlicerTest.cpp`), sobre `sliceCacheKey`:
  - é determinística;
  - muda com o conteúdo, com o rótulo, com a entrada e com as flags.
- **Integração, seção nova:**
  - o programa VLA da seção 20, rodado no híbrido com `--slice`, mostra o aviso de falha
    do slicer uma vez por "slicer run" (debug) e depois "(cached)";
  - um programa fatiável roda o `sbt-slicer` uma vez só e as fases seguintes registram
    "reusing the slice".
- **Suíte existente:** 40/40, e ctest.

## Medição: rodada R19, amostra `r15-ce.tsv`, 300 s

A mesma rodada R19 mede também o 3b; ver o spec do 3b.

| braço | build | flags / env |
|---|---|---|
| controle | final | — |
| slice | final | `--slice` |
| slice-light | final | `--slice`, `MAP2CHECK_SLICE_CLEANUP=light` |
| slice-o2 | final | `--slice`, `MAP2CHECK_SLICE_CLEANUP=o2` |
| slice-ntscd | final | `--slice`, `MAP2CHECK_SLICER_FLAGS=--cda=ntscd` |
| slice-ptafs | final | `--slice`, `MAP2CHECK_SLICER_FLAGS=--pta=fs` |

- **Gate:** TRUE errado = 0 em todo braço promovido.
- **Critério de promoção:** mais cobertas que o `slice` puro, com pares discordantes a
  favor.
