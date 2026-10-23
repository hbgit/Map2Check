# Handoff — campanha V99 no ambiente oficial da Test-Comp (2026-10-11)

Ponto de partida para a próxima sessão (Claude). O objetivo da sessão foi: **rodar a
campanha completa da 9.0 no ambiente oficial da Test-Comp 2026**, com sv-benchmarks
fixado na `testcomp26`, limites oficiais e validação pelos validadores oficiais.

Regras do usuário que continuam valendo:
- **Nunca mergear**; sempre PR com base `develop`.
- Branches `feat/…`. **Não mexer no gatilho do `ci.yml`**.
- Registrar cada mini-rodada em `docs/reports/tacas-experiment-log.md`.
- Commits/PRs em inglês; docs em pt-BR.
- **Nunca rodar `cmake .` num `build_*` sem `-DCMAKE_INSTALL_PREFIX`** (o prefixo volta
  para `release/`).

## 1. Branch e commits

`feat/testcomp-official-env`, base `feat/map2check-9.0`. HEAD `44d2160ab`. Commits
próprios: `e5a86a779` (validadores oficiais sob BenchExec), `cb99fe7bd` (runner
generate/validate), `866d35028` (archive do AFL++), `44d2160ab` (staging das
propriedades locais da tarefa).

**Nada foi pushado.** Há trabalho **não commitado** (ver §5).

## 2. Decisões do usuário nesta sessão

- Rerrodada com **limites oficiais** (900 s; memória ajustada a **8 GB** para 2 vagas
  nesta máquina de 23 GB; 4 núcleos).
- **Cover-Error inteira** + **Cover-Branches 3 500 estratificado** (proporcional ao
  tamanho de cada categoria oficial).
- **sv-benchmarks fixado na `testcomp26`** (`7efe28dd29576b46927b7a34e8f742bd90966a75`).
- Build = ponta atual (`44d2160ab`; o código do tool é o da `feat/map2check-9.0` + o
  wrapper/tool-info novos).
- Geração pelo **harness** (retomável), validação pelos validadores oficiais.

## 3. O que está rodando agora

- **Campanha `v99`**, launcher `tacas-results/v99-campaign.sh` (PID 33936). Contêineres
  `v99-ce-s0/s1` (imagem `map2check-v9-eval:latest`, `--memory=8g --cpus=4`,
  `--restart=unless-stopped`, retoma pelo CSV, done-markers `.v99-done-*`).
  - **Cover-Error: ~1 579/1 902** processadas; 697 geraram teste; suítes em
    `tacas-results/V99-suites/`, CSVs em `tacas-results/V99-ce_s*/results.csv`.
  - **Cover-Branches: 0/3 500** — só entra quando o CE fechar (o launcher já os
    enfileira).
  - Build `install_v99` (script de build no relatório do ambiente oficial), archive
    `tacas-results/V99/map2check-testcomp.zip`.
- **Validação oficial preliminar** (`validate-v99-ce.sh`, 3 validadores: gcc-unformatted,
  clang-unformatted, testcoca): fechou **673/682 = 98,7%** TRUE por ≥1 validador.
  scores.csv em `tacas-results/V99-ce-prelim/scores.csv`. Re-rodar quando o CE avançar.

## 4. Issue aberta (pendência principal)

- **#74** — "NonDetPass: tipos `__VERIFIER_nondet_*` não modelados zeram tarefas
  (Intel-TDX, Floats) para UNKNOWN" — https://github.com/hbgit/Map2Check/issues/74
  - `NonDetPass.cpp` modela só 16 tipos; faltam `float, longlong, ulonglong, _Bool,
    u8/u16/u32, charp`.
  - Medido: 147 tarefas `UNKNOWN` afetadas (ulonglong 113, float 22, u32 12) →
    **Intel-TDX 125, Floats 22**.
  - A correção exige recompilar o tool → foi decidido **não** mexer no build no meio da
    campanha (build único). Fica como follow-up.

## 5. Trabalho não commitado (revisar e commitar)

- `tests/testcomp/official/testcomp.py` — fixes: (a) staging das propriedades locais do
  task (já em `44d2160ab`); (b) **disco**: remove `--no-compress-results`, apaga o
  `results/<validador>` (o `.files` é o grosso), o `sv-benchmarks/` staged e o `tool/`
  após cada validador — validado: 3 tarefas → 20 KB (antes 140 → 8 GB). Knob
  `MAP2CHECK_KEEP_RESULTS=1` preserva o scratch para debug.
- `tests/testcomp/run_testcomp_evaluation.sh` — `TESTCOV_S=0` desliga o TestCov interno
  (a validação é pelos validadores oficiais).
- `tests/testcomp/build_corpus_official.py` — **novo**: corpus estratificado
  PROPORCIONAL ao tamanho, lendo categorias/excludes do def oficial. (Não existia
  estratificação proporcional no repo; o `build_corpus.py` usa cota igual por categoria.)
- `tests/testcomp/corpus/cover-error-official.tsv` (1 902) e
  `cover-branches-3500.tsv` (3 500).
- `tests/testcomp/official/bench-defs/excludes/ECA-excludes.set` — exclude oficial do
  ECA (exclui 1 027 de ~1 059 ECA; sem ele o corpus super-representava ECA).
- Docs: `docs/reports/2026-10-06-ambiente-testcomp-oficial.md` (novo) e entrada no
  `tacas-experiment-log.md`.

## 6. Próximos passos

1. **Fechar o CE** (as ~323 restantes são Intel-TDX/Hardware, lentas e UNKNOWN) e deixar
   o **CB** (3 500) rodar; o launcher já cuida disso.
2. **Validar oficialmente** as suítes conforme a geração avança (rodar `validate-v99-ce.sh`
   periodicamente; depois idem para o CB com `--property cover-branches`).
3. **Relatório da campanha** (pt-BR) com a comparação pareada e o placar oficial de CE e
   CB; entrada no `tacas-experiment-log.md`.
4. **Commitar** o trabalho do §5 (cada um no seu assunto).
5. **Issue #74**: implementar o fix do `NonDetPass`, recompilar e re-rodar as categorias
   afetadas (ou a campanha com build novo) — combinado como follow-up.
6. **tcerr**: a validação best-of (`clang-unformatted` + `testcoca`) morreu no meio; refazer
   quando a máquina desafogar (script `tacas-results/official/validate-tcerr-fast.sh`).
7. **Windows/WSL**: o `ext4.vhdx` precisa ser compactado no Windows (o WSL liberou ~15 GB
   internos — o lixo era `official/` 8,3 GB + imagens/build cache do Docker; o vhdx não
   encolhe sozinho). Após `wsl --shutdown`, **relançar o launcher v99** (não sobrevive a
   reboot).

## 7. Números atuais

- Cover-Error geração: ~1 579/1 902 (83%); 697 com teste (44,1% das processadas);
  consolidado ~36,6% do corpus.
- Cover-Error validação oficial (682): **673 = 98,7%**.
- Categorias fracas: Intel-TDX 0/658, Hardware ~5/65 (orçamento/nondet), Floats ~11/33
  (nondet `float`, issue #74).
