# Issue #74 — re-rodada das categorias afetadas (2026-10-12)

Relatório da correção dos tipos `__VERIFIER_nondet_*` não modelados (issue #74) e da
re-rodada Cover-Error que mede o efeito dela nas condições oficiais da campanha V99.

## Resumo

- **Floats: 11/33 → 33/33** no Cover-Error, todas com score 1,0 nos 3 validadores oficiais.
  As 22 tarefas recuperadas davam `UNKNOWN` em ~810–865 s; agora dão `FAILED` em 0–92 s.
- **Sem regressão:** as 15 tarefas de outras categorias que já davam `FAILED` na v99
  continuam `FAILED`, com score 1,0 nos 3 validadores.
- **Intel-TDX: 0/12** na amostra, como antes. O #74 era necessário, mas não suficiente: o
  gargalo agora é a explosão de estados do KLEE ao gravar o log nondet, registrada na
  **issue #75**. Ela afeta qualquer tarefa com mais que poucas leituras nondet, não só a
  Intel-TDX.

## Configuração

| Item | Valor |
|------|-------|
| Correção | `05a157371` (branch `feat/nondet-missing-types`) |
| Build | `install_v99b` (o `install_v99` da campanha ficou intocado) |
| Imagem | `map2check-v9-eval:latest` |
| Limites | 900 s, 8 GB, 4 núcleos, `TESTCOV_S=0` (iguais aos da V99) |
| Tarefas | 60 de `tacas-results/V99b-affected.tsv` |
| Validação | `run.sh validate --property cover-error`, 3 validadores oficiais: `testcov-…-gcc-unformatted`, `testcov-…-clang-unformatted`, `testcoca-…-clang-formatted` |

Composição das 60 tarefas:

- **33 Floats:** a categoria inteira do corpus CE.
- **12 Intel-TDX:** amostra, semente 7499.
- **15 tarefas de guarda de regressão:** tarefas que davam `FAILED` na v99, em ProductLines
  (5), Loops (3), Sequentialized (2), Hardware (2), Arrays (2) e Heap (1).

A re-rodada ocupou uma das 2 vagas do launcher, em paralelo com o CE da v99. A carga da
máquina foi a mesma da campanha.

## Resultados, par a par com a v99

| Categoria | Tarefas | v99 `FAILED` | v99b `FAILED` | Score oficial v99b | Transições |
|-----------|--------:|-------------:|--------------:|-------------------:|------------|
| Floats | 33 | 11 | **33** | **33,0** | 22 `UNKNOWN→FAILED`, 11 `FAILED→FAILED` |
| Intel-TDX | 12 | 0 | 0 | — | 5 `UNKNOWN→UNKNOWN`, 7 não rodadas na v99 (SKIPPED) → `UNKNOWN` |
| ProductLines | 5 | 5 | 5 | 5,0 | `FAILED→FAILED` |
| Loops | 3 | 3 | 3 | 3,0 | `FAILED→FAILED` |
| Sequentialized | 2 | 2 | 2 | 2,0 | `FAILED→FAILED` |
| Hardware | 2 | 2 | 2 | 2,0 | `FAILED→FAILED` |
| Arrays | 2 | 2 | 2 | 2,0 | `FAILED→FAILED` |
| Heap | 1 | 1 | 1 | 1,0 | `FAILED→FAILED` |

**Validação oficial:** 48/48 suítes `true` em **cada um** dos 3 validadores, soma de score
48,0. As 26 tarefas que já davam `FAILED` na v99 também tinham score 1,0 lá, então não há
perda.

**Tempo:**

- Floats recuperadas: 0–92 s, mediana ~2 s. Na v99, as mesmas tarefas davam ~810–865 s.
- Guarda de regressão: a maioria ficou na mesma faixa. Algumas oscilaram, por exemplo
  `email_spec27_product17` (8 → 64 s) e `email_spec27_product18` (21 → 51 s). Outras
  ficaram mais rápidas, como `pals_lcr-var-start-time` (139 → 70 s) e `twocount32`
  (107 → 82 s).
- Essas variações vêm do sorteio do AFL++ e da carga do vizinho. Todas ficaram muito abaixo
  do orçamento de 900 s.

## Intel-TDX: por que continua zerada (issue #75)

O `failed external call: __VERIFIER_nondet_ulonglong` do #74 sumiu. Mesmo assim, as 12
tarefas terminam `UNKNOWN` em ~800 s.

Para isolar a causa, usei um programa sintético sem nenhum desvio antes do fim:
N leituras `__VERIFIER_nondet_int` e `reach_error` se `a[0] == 7 && a[N-1] == 42`.
KLEE puro, `install_v99b`:

| N | Veredito | Tempo | Caminhos parciais |
|--:|----------|------:|------------------:|
| 2 | `FAILED` | 0 s | 1 |
| 5 | `FAILED` | 28 s | 861 |
| 10 | `UNKNOWN` | 60 s | 1 550 |
| 100 | `UNKNOWN` | 120 s | 3 218 |
| 10 000 | `UNKNOWN` | 120 s | 0 completos |

**Diagnóstico pelo `run.istats` (N = 10):** 1 549 das 1 551 bifurcações estão sob
`map2check_target_function → map2check_exit → nondet_log_to_file → fprintf → vfprintf`.

Ou seja, o KLEE **já chegou ao `reach_error`**. O que impede o veredito é a gravação do
`klee_log.csv`, cujos valores ainda são simbólicos: o `vfprintf` do uclibc bifurca a cada
dígito de cada valor. Uma tarefa Intel-TDX lê ~31 500 valores por execução.

**Consequências:**

- Nessas tarefas, o híbrido depende só do AFL++.
- O efeito vale para qualquer categoria com muitas leituras nondet, inclusive no
  Cover-Branches.

**Correção proposta na #75:** concretizar os valores antes de gravar, só no runtime KLEE.

**Medido, mas não corrigido:**

- O AFL++ roda a ~97 exec/s nessas tarefas.
- O log faz um `malloc` por leitura, e esse `malloc` também vaza, porque `append_element`
  já copia a linha.

**Hipótese descartada:** a B-tree em disco (`ContainerBTree.c`) **não** está no caminho.
O build liga o `ContainerRealloc.bc`.

**Fora do escopo:** 44 das 658 tarefas exigem um valor igual a um endereço de `malloc`, o
que nenhum vetor reproduz no validador.

## Situação da campanha V99

- **Cover-Error:** as 256 Intel-TDX restantes foram marcadas `SKIPPED` (`UNKNOWN` por
  construção no `install_v99`). O `v99-ce-s1` termina as Hardware restantes.
- **Cover-Branches:** começou às 18h37 (`v99-cb-s0`), na vaga liberada pela re-rodada.
- **Placar oficial:** para a correção do #74 contar, as Floats do `install_v99b`
  substituem as da v99 (+22 no CE). O CB tem 2 009 tarefas que declaram `float`. Ele roda
  com o `install_v99` e herda a limitação; uma re-rodada CB das Floats com o `install_v99b`
  fica como próximo passo.

## Artefatos

Todos em `tacas-results/`:

- `V99b-affected.tsv`: lista das 60 tarefas.
- `V99b-ce-affected/results.csv`: vereditos e tempos.
- `V99b-suites/`: suítes.
- `V99b-ce-val/scores.csv`: validação oficial, 48 tarefas.
- `compare-v99b.py`: comparação par a par.
- `V99b-probe74/`: sonda de 300 s da #74.
