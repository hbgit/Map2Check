# Plano da campanha completa pós-merge (Map2Check 9.0)

**Objetivo:** medir a 9.0 no **mesmo recorte da campanha v15** (relatório
`docs/Map2Check-Relatorio-v15.docx`), com a configuração padrão da 9.0, para uma
comparação pareada tarefa a tarefa. É a base do artigo e da decisão de release.

## O que roda

**Um braço só:** a configuração padrão da 9.0, que é o híbrido alternado com troca de
sementes e o corpus do fuzzer nas suítes de Cover-Branches. Os braços alternativos
(`--fixed-hybrid`, `--slice`, knobs) já foram medidos nas amostras (R15 a R26) e não
entram. A v15 **não roda de novo**: os resultados dela estão em `tests/*/results_v15*`
e o pareamento é feito pelo programa.

| corpus | tarefas | manifest | custo médio por tarefa* | horas-vaga |
|---|---|---|---|---|
| Test-Comp Cover-Error | 1 087 | `tests/testcomp/corpus/cover-error-q400.tsv` (o recorte da v15) | ~85 s + TestCov ≈ 110 s | ~33 |
| Test-Comp Cover-Branches | 2 765 (o mesmo recorte da v15, confirmado) | `tests/testcomp/corpus/cover-branches-q400.tsv` | ~240 s + TestCov ≈ 300 s | ~230 |
| Juliet (escopo C) | ~8 000 | `tests/juliet` (os 4 shards da v15) | ~36 s | ~80 |
| CASTLE | 250 (119 em escopo) | `tests/castle` | ~60 s | ~2 |
| SV-COMP MemSafety, MemCleanup e NoOverflows | por categoria, a decidir | `build_corpus.py` | ~36 s | depende |

\* medido nas rodadas R17, R19 e R26 com 300 s (Test-Comp) e 120 s (SV-COMP).

## Capacidade e duração

- Máquina: 16 núcleos, 23 GB. Com **no máximo 5 contêineres** a memória não fica crítica
  (com 8 ela ficou; ver `docs/backlog.md`, limite de memória do KLEE).
- Duração, com 5 vagas:
  - Cover-Error: ~7 h;
  - Juliet: ~16 h;
  - Cover-Branches: ~2 dias (2 765 tarefas);
  - CASTLE e SV-COMP por categoria: algumas horas.
  - Total: **~3,5 dias**.
- Ordem sugerida, das decisões mais baratas para as mais caras:
  1. CASTLE;
  2. Cover-Error;
  3. SV-COMP;
  4. Juliet;
  5. Cover-Branches.

## Regras da execução

- **Build:** a tag da 9.0 depois do merge, com o install congelado (copiado) antes do
  início, como nas rodadas R19 a R26.
- **Escalonador fora do repositório** (decisão do projeto). Ele precisa:
  - usar `-` em vez de campo vazio na lista de jobs (o bash junta tabs consecutivos);
  - retomar pelo CSV;
  - separar os shards por índice do manifest.
- **Validação de cada lote antes de seguir:**
  - nenhuma tarefa com ERROR em massa, que é sinal de problema de infraestrutura;
  - contar as tarefas com "did not finish its run" (KLEE morto por falta de memória) e
    repeti-las.
- **Registro:** cada corpus concluído entra em `docs/reports/tacas-experiment-log.md`,
  comparado à v15 com:
  - cobertas e TRUE errado (Cover-Error);
  - cobertura média (Cover-Branches);
  - TP, FN, TN e FP (Juliet e CASTLE);
  - correct e wrong (SV-COMP).

## Critérios de aceite para a release

- TRUE errado = 0 em Cover-Error e nenhum wrong-true novo no SV-COMP.
- Cover-Error e Cover-Branches: não piores que a v15 no pareado (a amostra aponta
  +46 cobertas em 213).
- CASTLE e Juliet: FP não maiores que os da v15, e FN iguais ou menores.
