# tacas 3c — priorização de sementes, v1 (design)

**Data:** 2026-09-29
**Branch:** `feat/tacas-3c-seed-ranking`, a partir de `feat/tacas-memtrack-intrinsics`
**Aprovação:** decisões delegadas pelo usuário em 2026-09-29.

## Problema

O KLEE recebe no máximo 64 entradas da fila do AFL++ (`kMaxSeedsFromFuzzer`), escolhidas
pelas mais antigas por id. Quando a fila passa de 64, o que era novo no fim do fuzzing fica
de fora, e esse é justamente o caso dos programas maiores.

## v1 (esta branch)

- O AFL++ 4.40c marca com `,+cov` as entradas que alcançaram **arestas novas**. As demais só
  mudaram as contagens de execução de arestas já vistas. Medido: 2 de 18 entradas numa fila
  de 20 s. Não existe `.state/redundant_edges` nesta versão.
- `selectQueueEntries` põe as `+cov` primeiro, em ordem de id, depois as outras, também em
  ordem de id, e então corta no limite.
- Teste unitário: `SelectQueueEntries.PutsNewCoverageFirst`.

## Próximos passos (v2, fora desta branch)

- **Distância ao alvo** (Cover-Error): priorizar sementes cujo replay passa por blocos mais
  próximos de `reach_error` no CFG/SDG. O dg do slicing já tem o grafo. Exige um traço de
  blocos no replay, que o `TrackBasicBlockPass` pode fornecer.
- **KLEE → AFL++:** hoje seguem os 64 `.ktest` mais recentes. O KLEE não marca quais estados
  cobriram código novo; `--only-output-states-covering-new` mudaria a suíte de CB e não
  entra.

## Medição

É preciso uma rodada `seeds` e `alternate` com este build contra a R19 (os mesmos braços
sem ranking). Só tem efeito em tarefas cuja fila passa de 64 entradas.
