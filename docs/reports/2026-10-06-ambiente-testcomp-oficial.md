# Ambiente oficial da Test-Comp 2026

Reprodução local do ambiente da competição, para (a) revalidar as suítes com os
validadores oficiais e (b) medir com as condições que a Test-Comp de fato usa — e
não com o nosso teto calibrado.

**Branch:** `feat/testcomp-official-env` (a partir de `feat/map2check-9.0`).
**Commits:** `e5a86a779`, `cb99fe7bd`, `866d35028`, `44d2160ab`.

## 1. Motivo

A análise da campanha da 9.0 mostrou que o teto de **50 casos por suíte**
(`kMaxBranchTestCases`) foi calibrado no **nosso** ambiente: TestCov 3.7, 300 s por
tarefa, **sem** BenchExec. A competição **não** limita o tamanho da suíte. Para saber se
o teto ajuda ou atrapalha, os testes precisam rodar como a competição roda.

## 2. O que a competição usa (tag `testcomp26`)

De `gitlab.com/sosy-lab/test-comp/bench-defs`, tag `testcomp26`:

- **Imagem:** `registry.gitlab.com/sosy-lab/benchmarking/competition-scripts/user`,
  Ubuntu 24.04.3, clang 18 e gcc 13. O `Dockerfile` fixa o digest
  `sha256:2a38216d…`. A imagem "full" do TestCov no fm-tools é a mesma.
- **BenchExec 3.34** (submódulo `829bf7a458`).
- **Validadores:**
  - TestCov 3.11.post3 (Zenodo `17698774`);
  - TestCoCa `testcomp26` (Zenodo `17819536`).
- **Validação:** 10 min, 7 GB, 2 núcleos e `--timelimit-per-run 50`. São **cinco**
  definições: TestCov com gcc e com clang, formatado e não formatado, mais o TestCoCa.
- **Pontuação** (`competition-scripts`, `export_adjusted_csvs.py`, `update_score_tester`):
  - **Cover-Branches:** o **máximo** da cobertura entre os validadores;
  - **Cover-Error:** basta **um** validador responder TRUE.
- **Geradores:** 15 min de CPU, 15 GB, 4 núcleos, `resultfiles **/test-suite`. Não existe
  `map2check.xml` na tag (o nosso segue o modelo do `klee.xml`).

## 3. O que foi construído

Em `tests/testcomp/official/`:

- **`Dockerfile`** — imagem `map2check-testcomp-env`: a base oficial fixada por digest,
  BenchExec 3.34, TestCov 3.11.post3 e TestCoCa baixados do Zenodo com verificação de
  `sha256`.
- **`init.sh`** — entrypoint que cria o cgroup separado do BenchExec (cgroups v2, como o
  `doc/benchexec-in-container.md` prescreve). Sem ele o BenchExec não limita nem mede.
- **`bench-defs/`** — as **cinco** definições oficiais, sem alterações, mais o
  `map2check.xml` (não oficial, modelado no `klee.xml`) para **gerar** as suítes.
- **`testcomp.py`** — dois passos, cada um uma rodada de BenchExec:
  - `generate`: monta o archive (`utils/testcomp-archive/make-archive.sh`) e roda sob o
    `map2check.xml`; cada `test-suite/` vira um `.zip` no layout dos validadores;
  - `validate`: as cinco definições oficiais, com o task set e os caminhos reescritos,
    e as pontuações fundidas como a competição funde.
- **`run.sh`** — sobe o contêiner (`--privileged --cap-drop=all --device /dev/fuse`),
  monta o repositório em `/m2c` e os dados em `/data`, e **traduz** os caminhos da linha
  de comando para dentro do contêiner.

Em `utils/`:

- **`map2check-testcomp-wrapper.py`** — passa a apontar o AFL++, `AFL_PATH`, `AFL_CC` e os
  passes para as cópias **empacotadas** no archive, quando existem; fora do archive, os
  padrões de instalação de desenvolvimento continuam valendo.
- **`moduleBenchExec/map2check_testcomp.py`** — módulo tool-info do BenchExec (separado do
  `map2check.py`, que é o adaptador SV-COMP). Tira o orçamento de `cputime` quando o run
  set não tem `walltime` — que é o caso da Test-Comp.
- **`moduleBenchExec/__init__.py`** — para o BenchExec carregar
  `moduleBenchExec.map2check_testcomp`.
- **`testcomp-archive/make-archive.sh`** — zip **autocontido** (~99 MB) com LLVM 16, KLEE 3.1
  e AFL++ 4.40c. Reconstrói o `RUNPATH` de cada ELF (via `patchelf`) em vez de usar
  `LD_LIBRARY_PATH`, e recusa symlinks não resolvidos.

## 4. Como rodar

```sh
# imagem (uma vez)
docker build -t map2check-testcomp-env tests/testcomp/official

# archive do Map2Check: roda na imagem de desenvolvimento, que tem LLVM16/KLEE/AFL++
docker run --rm -v "$PWD:/workspace" -w /workspace map2check-dev:aflpp \
  utils/testcomp-archive/make-archive.sh /workspace/install_v9b /workspace/map2check.zip

# gerar as suítes num corpus pequeno
tests/testcomp/official/run.sh generate --property cover-branches \
  --tasks tests/testcomp/corpus/cover-branches-q40.tsv \
  --archive map2check-testcomp.zip --suites OUT/suites --out OUT/gen

# validar com os cinco validadores oficiais
tests/testcomp/official/run.sh validate --property cover-branches \
  --tasks tests/testcomp/corpus/cover-branches-q40.tsv \
  --suites OUT/suites --out OUT/val
```

## 5. Verificação

- **Os cinco validadores concordam** em 3 tarefas (smoke2), sob o BenchExec:

| tarefa | gcc | clang | TestCoCa |
|---|---|---|---|
| `array-fpi/s52iff_overflow` | 81,25% | 81,25% | 81,25% |
| `xcsp/AllInterval-011` | 90,91% | 86,36% | 90,91% |
| `eca-rers2012/Problem10_label28` | 37,81% | 37,81% | 37,81% |

- Uma suíte ECA de 50 casos foi validada em **~40 s** pelo TestCov 3.11, enquanto o
  TestCov 3.7 estourava os 300 s do harness.
- O pacote roda na **imagem oficial**, com KLEE e AFL++ funcionando.
- `run.sh generate` sob o BenchExec gerou as 3 suítes, com `--timelimit 120s` e
  `--memorylimit 6GB`.

## 6. Problemas resolvidos no caminho

- **Overlay de `/home`:** o repositório é montado em `/m2c` e os dados em `/data`; o
  `/home` do contêiner fica livre, porque o BenchExec o sobrepõe (e um overlay sobre um bind
  mount abaixo dele falha com "Failed to create overlay mount"). Usa `fuse-overlayfs`.
- **TestCov escrevendo ao lado do programa:** as entradas vão para um diretório de staging.
- **`--format` escrevendo no diretório da ferramenta:** o BenchExec roda a partir de uma
  **cópia** da ferramenta.
- **`${taskdef_name}` inclui `.yml`:** o layout das suítes mantém o nome completo.

## 7. Achado e correção: propriedades locais da tarefa (2026-10-07)

Ao validar as **140 suítes** de Cover-Branches que deram `TESTCOV_ERROR` na campanha da
9.0 (TestCov 3.7, 300 s), todas saíram **`MISSING`**.

- **Causa raiz:** `testcomp.py:stage_tasks()` copiava o `.yml`, os `input_files` e o
  `properties/` **global** — mas não os arquivos de propriedade **locais da tarefa**.
  `eca-rers2018/Problem14.yml` referencia ~100 `properties/unreach-call-*.prp` relativos à
  própria tarefa. O BenchExec resolve **todas** as propriedades de uma definição ao
  carregar o task set; faltando um arquivo, ele **aborta a rodada inteira** — e com ela o
  resultado de todas as 140 tarefas. O smoke2 passou porque `eca-rers2012` não tem
  propriedades locais.
- **Correção** (`44d2160ab`): `stage_tasks` copia o `properties/` da tarefa quando existe.
- **Verificação:** com a correção, `eca-rers2018/Problem14` valida (`done`, ~2,4% nos cinco
  validadores) e o BenchExec não aborta mais.

> Ressalva da rodada completa: o host de 25 GB ficou **sob swap** (`7 GB × 3 threads`) e o
> próprio BenchExec avisou `System has swapped during benchmarking. Benchmark results are
> unreliable!`. Os números finais do `gcc-unformatted` do tcerr saíram; os das variantes
> `formatted` saíram quase todos `TIMEOUT` e **não devem ser levados a sério** desta rodada.

## 8. Questões em aberto

- O `sv-benchmarks` local (ago/2026, `cd68d4ed`) é mais novo que o fixado na `testcomp26`
  (`7efe28dd`). Decidir se a rerrodada **fixa** a versão da competição.
- O `make-archive.sh` lê o `install_v9b` (build de `6848d2395`).

## 9. Próximos passos

1. Recalibrar o **teto de 50** com o validador oficial (o teto não existe na competição).
2. Implementar a **seleção da suíte por cobertura**.
3. CI da Test-Comp: reproduzir este ambiente num job — **adiado** por decisão de sessão
   (custo ~20–35 min/run, `--privileged`/`/dev/fuse`); a alternativa barata é um teste
   unitário do `stage_tasks`. Ver `docs/reports/2026-10-07-handoff-testcomp-oficial.md`.
