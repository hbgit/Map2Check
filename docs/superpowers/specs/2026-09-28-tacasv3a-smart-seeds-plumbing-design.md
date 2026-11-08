# tacasv3a — Smart seeds, parte 1: o encanamento da troca AFL++ ↔ KLEE

**Data:** 2026-09-28
**Branch:** `feat/tacas-smart-seeds` (a partir de `develop`, que já tem a tacasv2)
**Baseline:** tacasv2 com `--seed-exchange` desligado (o híbrido default)
**Status:** rascunho — aguardando revisão
**Linha:** `decisions/tacas-afl-slicing-roadmap.md` (ai-memory); Fase 3.3 do
`docs/migration-schedule.md`. Registro de rodadas: `docs/reports/tacas-experiment-log.md`.

---

## 1. Objetivo

Fazer a troca de sementes entre os motores **acontecer de verdade** e medir o efeito. A
v15 mediu "sem ganho" com o `--seed-exchange`, mas, como a seção 2 mostra, as sementes
praticamente nunca chegavam ao outro motor. Esta etapa conserta o encanamento; o laço
alternado com detecção de estagnação (3b) e a priorização das sementes (3c) vêm depois,
cada uma com spec e medição próprios.

**Métrica de sucesso** (decidida com o usuário): Test-Comp — tarefas cobertas no
Cover-Error e cobertura média no Cover-Branches — **e** vereditos de propriedades do
SV-COMP (MemSafety, NoOverflows, ReachSafety), sempre contra a tacasv2 sem a troca, no
mesmo corpus e no mesmo build.

---

## 2. Diagnóstico do `--seed-exchange` atual

| # | Defeito | Onde |
|---|---|---|
| 1 | As sementes não atravessam fases: cada fase cria um `Caller` que apaga o diretório de trabalho, e `seeds/` mora dentro dele | `Caller::Caller` (`rm -rf <hash>`), `Caller::seedDirectory` |
| 2 | Fuzzer → KLEE lê o `klee_log.csv` do diretório **atual**, que acabou de ser recriado vazio; e mandaria um vetor só | `exportFuzzerVectorAsKtest` |
| 3 | O corpus do AFL++ nunca chega ao KLEE, embora o KLEE aceite um diretório de sementes | idem |
| 4 | A terceira fase (KLEE → AFL++) quase não roda: o KLEE usa até 0,8× o orçamento e sobram segundos | `map2check.cpp` (fase 3) e orçamentos em `executeAnalysis` |

---

## 3. Design

### 3.1 Armazém de sementes persistente

- Diretório `<cwd>/<programHash>.seeds/`, **ao lado** do diretório de trabalho (não dentro
  dele), criado na primeira fase e reaproveitado pelas seguintes:
  - `afl/` — entradas para o `-i` do AFL++ (vetores do KLEE convertidos em bytes, mais o
    corpus que o próprio AFL++ descobriu);
  - `ktest/` — sementes para o `--seed-dir` do KLEE;
  - `replay/` — diretório isolado onde as entradas do AFL++ são reexecutadas.
- Removido pelo `main` depois da última fase, salvo em `--debug`.
- Só existe com `--seed-exchange`. Sem a flag, nada muda em relação à tacasv2.

### 3.2 Fuzzer → KLEE: conversão por replay

Ao fim da fase AFL++ (sem violação encontrada):

1. Para cada entrada de `afl-out/default/queue/` (em ordem de id, até **64**):
   - roda o binário de witness **dentro de `replay/`**, com a entrada no stdin e teto de
     2 s. Em `replay/` os arquivos que o runtime grava (`map2check_property`,
     `klee_log.csv`, …) não tocam o diretório da fase — um replay não pode sobrescrever
     uma violação já registrada;
   - lê o `replay/klee_log.csv` com `readNonDetLogAsObjects` e grava
     `ktest/afl-<id>.ktest` com `writeKtestFile`; apaga o log antes da próxima entrada;
   - descarta vetores vazios e duplicados (mesmos bytes).
2. Registra `Seeded KLEE with N vectors from AFL++`.

Na fase KLEE, com `ktest/` não vazio:
`--seed-dir=<store>/ktest --allow-seed-extension --allow-seed-truncation --seed-time=<¼ do orçamento do KLEE>`.
O `--seed-time` impede que reproduzir sementes consuma a fase inteira.

### 3.3 KLEE → fuzzer

- `exportKleeVectorsAsSeeds` passa a gravar em `<store>/afl/` (hoje grava em `seeds/`,
  que é apagado). A fase AFL++ usa `<store>/afl/` como `-i`.
- A cópia do corpus do AFL++ de volta ao armazém (hoje em `seeds/`) passa a gravar em
  `<store>/afl/`.

### 3.4 Orçamento com a troca ligada

- AFL++ **0,2**, KLEE **0,6**, AFL++ **0,2** do orçamento (hoje 0,2 / até 0,8 / o que sobrar).
- Sem `--seed-exchange`: inalterado (0,2 / 0,8), para a tacasv2 continuar medindo o mesmo.

### 3.5 O que fica igual

- O default continua **sem** troca. Promover a troca a default é decisão para depois da
  medição.
- Nenhuma mudança nos motores, na instrumentação ou no slicing.

---

## 4. Referência comparada

| Aspecto | FuSeBMC v4 | tacasv3a | Por quê |
|---|---|---|---|
| Direção | fuzzer ↔ BMC, alternado | AFL++ → KLEE → AFL++ | O laço alternado é o 3b |
| Formato | seeds do fuzzer usados pelo BMC via "smart seeds" | replay no binário de witness para obter o vetor tipado | O KLEE precisa de um objeto por leitura nondet; o runtime já registra os tipos |
| Priorização | por cobertura | ordem de id, teto 64 | Ranking é o 3c |

---

## 5. Testes

Integração (`test_testcomp_regressions.sh`, seção de seed exchange):

1. **As sementes atravessam fases.** Programa em que o AFL++ acha caminhos e o KLEE
   resolve uma guarda (a mesma `seed.c` da seção 11): com `--seed-exchange --debug`, o log
   tem `Seeded KLEE with N vectors from AFL++` com N > 0 **e**
   `Seeded the fuzzer corpus with M vectors from KLEE` com M > 0, e o armazém `<hash>.seeds/`
   existe ao lado do diretório de trabalho.
2. **O replay não sobrescreve uma violação.** Programa em que o próprio AFL++ acha o bug:
   com `--seed-exchange`, o veredito continua FAILED.
3. **Sem `--debug`, nada fica para trás.** Depois do run, não existe `*.seeds/` no diretório.
4. **Sem a flag, nada muda.** Nenhum `*.seeds/` e nenhuma linha `Seeded` (atualiza a
   seção 11, que hoje olha `seeds/` dentro do diretório de trabalho).

Unitário: a lógica pura — seleção das entradas da fila (ordem, teto, deduplicação por
bytes) e o nome do armazém — em funções testáveis sem os motores.

---

## 6. Avaliação (mini-rodadas, registradas no log)

- **Test-Comp:** o manifesto da R15 (Cover-Error 213, Cover-Branches 120), 300 s, braço
  `--seed-exchange` contra o controle da R15 (mesmo build, sem a troca).
- **SV-COMP:** amostras de MemSafety e NoOverflows da R14 (e uma de ReachSafety, com o
  mesmo `build_corpus.py`), com e sem `--seed-exchange`, 120 s.
- Métricas: tarefas cobertas e cobertura de ramos; corretos, **wrong-true/wrong-false**;
  N e M (sementes trocadas por tarefa) para confirmar que a troca aconteceu.

---

## 7. Riscos

| Risco | Mitigação |
|---|---|
| Replay de 64 entradas custa tempo | Teto de 2 s por entrada; binário já compilado; medir o custo por tarefa |
| Semente do AFL++ com vetor que não corresponde aos objetos do KLEE (tamanho/ordem) | `--allow-seed-extension/--allow-seed-truncation`; o vetor vem do mesmo runtime que o KLEE usa |
| KLEE gasta a fase reproduzindo sementes | `--seed-time` = ¼ do orçamento do KLEE |
| Mudar o orçamento (0,2/0,6/0,2) muda o efeito junto com a troca | Só com `--seed-exchange`; a medição compara contra o controle 0,2/0,8, e o efeito é o do pacote |
