# Handoff TACAS — 2026-09-29

Ponto de partida para a próxima sessão (antes do auto-compact). Branch ativa:
**`feat/tacas-smart-seeds`** (local, **ainda não enviada**; HEAD `1eb8a30b9` + este doc).
Regras do usuário que continuam valendo:

- **Nunca mergear** nada: sempre abrir PR, com base `develop`.
- Branches se chamam `feat/…`. **Não mexer no gatilho do `ci.yml`**.
- **Registrar cada mini-rodada** em `docs/reports/tacas-experiment-log.md` e comparar com
  as anteriores, uma a uma.
- LibFuzzer está abandonado: o AFL++ é a evolução.
- Commits e PRs em inglês, docs e relatórios em pt-BR.

---

## 1. O que está na branch (ainda não está em PR)

| commit | conteúdo |
|---|---|
| `b66254631` | 3a: o corpus do fuzzer chega ao KLEE como sementes tipadas (`--seed-exchange`) |
| `3a89b4684` | revisão do 3a: replays limitados e condicionados à fase, store limpo, `provedSafe` |
| `30966444f`, `14c850680` | R15 no log (Cover-Error e Cover-Branches) |
| `1eb8a30b9` | **fix(verdict):** o `abort()` do programa vira `map2check_assume(0)` (NonDetPass `rewriteAbortCallsToPrune`) |

A decisão do usuário foi mandar a correção do abort nesta mesma PR, descrita como correção
de bug.

Estado dos testes: integração **39/39** (`tests/integration/test_testcomp_regressions.sh`),
ctest **11/11**. A seção 30, rascunhada em `scratchpad/t30.sh`, **não foi adicionada**: ela
cobre um `assert()` da libc (`__assert_fail`) que ainda chama abort *dentro da libc* e para
o KLEE com exit 0. Avaliar se isso ainda gera um TRUE errado; se gerar, tratar
`--exit-on-error-type=Abort` quando o abort não é a violação registrada.

Specs e planos: `docs/superpowers/specs/2026-09-28-tacasv3a-smart-seeds-plumbing-design.md`
e `plans/2026-09-28-tacasv3a-smart-seeds-plumbing.md` (ledger em `.superpowers/sdd/`).

## 2. Rodadas em andamento ou pendentes (dados fora do repo)

Tudo fica em `/home/guilherme/github/prism/tacas-results/`: manifests `r15-*.tsv` e
`r18-*.tsv`, scripts `r1{5,6,8}-run.sh`, saídas `R1x-*`.

Os contêineres são lançados de dentro do repo, com `D=../tacas-results`:

```
docker run -d --rm -u root --name <nome> -v $(pwd):/workspace -v $D:/scratch map2check-dev:aflpp \
  bash /scratch/rNN-run.sh <cover-error|cover-branches> <braço> <shard> <nshards> /scratch/<manifest>.tsv "<EXTRA_FLAGS>"
```

A `EXTRA_FLAGS` usa `--seed-exchange` no braço seeds e `--slice` no braço slice. O
`rNN-run.sh` fixa `MAP2CHECK_PATH`: `build_seeds/install` na R16, `build_abort/install` na
R18. O harness é resumível pelo CSV.

- **R18 (validação do abort): concluída** — 6 de 8 TRUE errados resolvidos; restam `sin_interpolated_index-1` (controle) e `insertion_sort-1-2` (slice). Ver log.
- **R16 (Cover-Error e Cover-Branches com `--seed-exchange`, `build_seeds`, sem o fix do
  abort):**
  - Contêineres `r16-ce-seeds-{0,1,2}` e `r16-cb-seeds-{0,1}`.
  - No momento do handoff: CE 126/213 e CB 44/120.
  - Parcial anterior: 58 cobertas contra 50 do controle R15 em 74 pares.
  - Comparar com o controle da R15 e registrar no log.
- **Rodada limpa, a lançar:** como o fix do abort muda os dois braços, rodar **controle e
  seeds, ambos com o build final** (reconstruir `build_abort` ou um `build_final`), na
  amostra `r15-ce.tsv`/`r15-cb.tsv`. Isso isola o efeito das sementes. Registrar como R19.
- **R17, a lançar:** SV-COMP MemSafety e NoOverflows (±ReachSafety), com e sem
  `--seed-exchange`. Usar `tests/memsafety/run_memsafety_evaluation.sh` com
  `PROPERTY=memsafety|memcleanup|overflow` e `EXTRA_FLAGS`.
- **Depois disso:** push de `feat/tacas-smart-seeds` e PR para `develop` com o 3a, a
  revisão, o `provedSafe`, o fix do abort e as rodadas R15–R19. A descrição deve trazer os
  números. **Sem merge.**

## 3. Próximas frentes (pedido do usuário: 2d, 3b, 3c, pendências antigas e o cronograma)

Seguir o fluxo superpowers em cada frente: brainstorming → spec → plan → execução nativa
com ledger → revisão final por subagente opus → PR. Cada frente numa branch `feat/…`
própria, a partir de `feat/tacas-smart-seeds` enquanto a PR desta não for mergeada.

### 2d — otimizações do slicing

O slice ainda empata com o controle: 126 × 129 na R15.

1. **Fatiar uma vez só no híbrido.** Hoje cada fase cria um Caller e refatia. O ideal é
   guardar o `.bc` fatiado ao lado do store, no mesmo esquema de `seedStorePath`.
2. **Cutoff consertado:** `-cutoff-diverging` com o `exit(0)` recebendo `!dbg` e saída
   silenciosa. Hoje ele fica desligado porque gera "Broken module" no KLEE.
3. **`opt -O2` depois do slice**, para limpar o código morto que o dg deixa.
4. **Avaliar os parâmetros do dg:** `--cda ntscd` e `--pta fs`.
5. **Corretude:** investigar o TRUE errado de `loops/insertion_sort-1-2.c`, que só
   aparece no slice, e os **4 ERROR de ECA** (~330 s, sem suíte), que também só aparecem
   com `--slice`.

Métrica: amostra `r15-ce.tsv` com `--slice` contra o controle, com TRUE errado igual a 0
como gate. Os critérios atuais ficam em `modules/frontend/` (slicer). Os testes estão em
`SlicerTest.cpp` e nas seções 13–25 da integração.

### 3b — laço alternado com detecção de estagnação

- **Hoje:** o esquema é fixo, AFL++ 0.2T → KLEE 0.6T → AFL++ 0.2T, com
  `args.phase` em `map2check.cpp`.
- **Objetivo:** alternar as engines enquanto houver ganho. Troca de engine quando a
  engine atual estagna, ou seja, não entra nada novo na queue do AFL, ou o KLEE não
  cobre novos BBs por N segundos.
- **Reusar:**
  - `feedsKleePhase`, que hoje só alimenta na fase 1 e precisa generalizar;
  - `exportFuzzerCorpusAsKtests`;
  - `exportKleeVectorsAsSeeds`;
  - o store persistente.
- **Cuidados:**
  - o teto de 5% do tempo para replay;
  - `provedSafe` encerra tudo;
  - a violação encontrada encerra tudo.

### 3c — priorização de sementes

- **Hoje:** `selectQueueEntries` pega as 64 primeiras por id (`kMaxSeedsFromFuzzer`).
- **Objetivo:** ranquear por novidade de cobertura, usando os BBs do
  TrackBasicBlockPass ou o bitmap do AFL. Opcionalmente, pela distância/densidade no SDG
  até o alvo, reusando o dg do slicing.
- Os testes unitários ficam em `SeedStoreTest.cpp`.

### Pendências antigas de corretude

- **MemoryTrackPass não instrumenta os intrínsecos do LLVM 16** (`llvm.memcpy/memset/memmove`).
  O pass procura os nomes com tipos antigos (`p0i8`), então as cópias passam sem checagem.
- **Falso positivo do memtrack no busybox `sleep-3`**, na amostra NoOverflows/MemSafety.
- **A mensagem "did not build within Ns" é enganosa** quando o que houve foi um erro de
  link do AFL.
- **Minors adiados** nos ledgers `.superpowers/sdd/*/progress.md`.
- **Worktree velho `../Map2Check-2c`** (`feat/tacas-slicing-overflow`, já mergeado): a
  remoção precisa de `--force`. **Pedir permissão** antes. O worktree
  `Map2Check-fuzzer-adaptive` está como prunable.

### Atualizar `docs/migration-schedule.md`

- O documento está defasado: o cronograma é de 01/jun/2026 a 30/mai/2027 e ainda traz as
  métricas antigas.
- Refletir o que a linha TACAS já entregou:
  - tacasv1 (AFL++ 4.40c com CmpLog);
  - 2a/2b/2c (slicing por propriedade);
  - as correções de veredito (HaltTimer, vetor vazio, abort);
  - 3a (smart seeds).
- Registrar os números da R15 (Cover-Error: 129 contra 111 cobertas na v15, TRUE errado
  27 → 5, mediana 43 s → 5 s).
- Registrar como próximos marcos: 2d, 3b, 3c e a campanha completa sequencial depois do
  merge.

## 4. Builds úteis

- **`build_abort/install`:** HEAD atual, com o fix do abort.
- **`build_seeds/install`:** 3a sem o fix do abort.
- **`build_v15`/`install_v15`:** referência v15.
- **Reconstrução:** dentro do `map2check-dev:aflpp`, rodar
  `cmake .. -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm -DCMAKE_INSTALL_PREFIX=<dir>/install`.
  **Sempre fixar o prefixo**: um prefixo revertido já sobrescreveu `release/` uma vez.
