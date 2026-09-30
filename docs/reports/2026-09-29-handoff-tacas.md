# Handoff TACAS — 2026-09-29 (atualizado à tarde)

Ponto de partida para a próxima sessão. Regras do usuário que continuam valendo:

- **Nunca mergear**: sempre abrir PR, com base `develop`.
- Branches se chamam `feat/…`. **Não mexer no gatilho do `ci.yml`**.
- **Registrar cada mini-rodada** em `docs/reports/tacas-experiment-log.md` e comparar uma
  a uma.
- LibFuzzer está abandonado.
- Commits e PRs em inglês; docs e relatórios em pt-BR.
- **Nunca rodar `cmake .` num `build_*` sem `-DCMAKE_INSTALL_PREFIX`.** O prefixo volta
  para `release/` e o `ninja install` sobrescreve a instalação de referência.

Em 2026-09-29 o usuário delegou as decisões ("você tem permissão para fazer as decisões que
quiser") e pediu, nesta ordem:
1. investigar os TRUE errados restantes;
2. implementar o 2d e o 3b com as abordagens recomendadas;
3. lançar os testes ao final.

## 1. Pilha de branches (locais, **nenhuma enviada ainda**)

Cada branch parte da anterior. As PRs devem ser abertas em cadeia (cada uma com base na
anterior, ou todas contra `develop` em ordem).

| branch | commits próprios (resumo) |
|---|---|
| `feat/tacas-smart-seeds` | 3a (sementes), revisão, `provedSafe`, **fix abort → assume**, **fix caminhos descartados** (+ duas correções dele), logs R15/R16/R18 |
| `feat/tacas-2d-slicing` | 2d: slice uma vez por execução (cache `<hash>.slice/`), knobs `MAP2CHECK_SLICE_CLEANUP` e `MAP2CHECK_SLICER_FLAGS`, `opt` limitado |
| `feat/tacas-3b-alternation` | fix AFL (zeros após o fim da entrada); 3b `--alternate-engines`; correções da revisão; `migration-schedule.md` atualizado |
| `feat/tacas-memtrack-intrinsics` | fix: `memset/memcpy/memmove` checados no LLVM 16 (despacho por `MemSetInst`/`MemTransferInst`); fix: mensagem de erro de link do AFL++ |
| `feat/tacas-3c-seed-ranking` | 3c v1: entradas `+cov` da fila vão primeiro para o KLEE (spec `2026-09-29-tacas-3c-seed-ranking-design.md`); logs R16-CB/R17/R19 parcial; fix do classificador (segfault do replay do AFL não é ERROR) |
| `feat/tacas-memtrack-strings` | fix: `%s`/`puts` checam a string antes da chamada (o `printf` roda como externa no KLEE → TRUE errado do Juliet CWE193) |
| (3b, depois da R19) | `fix(hybrid): the fuzzer gets all of KLEE's vectors, built once, with growing patience` — as 5 perdas eca-* da alternância |
| `feat/tacas-klee-vector-replay` (acima de fuzzer-suite) | vetores do KLEE executados nativamente com zeros após o fim, depois de cada fase KLEE sem violação (o ganho eca-* do braço seeds, agora explícito e também no híbrido simples) |
| `feat/tacas-fuzzer-suite` | knob `MAP2CHECK_FUZZER_SUITE=1`: o corpus do AFL++ entra na suíte de Cover-Branches (até metade, sem duplicatas) |

- **Ponta da pilha:** `feat/tacas-3c-seed-ranking` (a R19 e a R17 usam o build de
  `8c70e5a73`; a R20 usa `install_r20`, da ponta com o 3c).
- **Testes:** unitários 12/12; integração 46 seções.
- **Specs:**
  - `docs/superpowers/specs/2026-09-29-tacas-2d-slicing-optimizations-design.md`;
  - `docs/superpowers/specs/2026-09-29-tacas-3b-engine-alternation-design.md`, que inclui
    a seção "Revisão".
- **Revisão final (subagente opus):** achados 1–9 tratados.
  - 1: o contador "partially completed paths" também conta podas por assunção e impedia
    qualquer prova. Corrigido.
  - 2–3: KLEE que morreu e "skipping fork". Corrigidos.
  - 4–9: suíte de Cover-Branches entre fases, orçamento das fases, SIGINT duplo, paciência
    do KLEE e flags. Corrigidos.

## 2. Correções de veredito desta sessão (todas testadas)

- **`abort()` do programa vira poda** (`map2check_assume(0)`). Resolveu 6 de 8 TRUE errados
  da R15 e também o falso positivo do memtrack no `busybox sleep-3`: o `bb_show_usage()`
  chamava `abort()` com `argv` ainda alocado.
- **KLEE que descartou caminhos não é prova.** Os casos são:
  - "silently concretizing" (double simbólico: `sin_interpolated_index-1`);
  - qualquer `*.err` ou `*.early` (VLA: `insertion_sort-1-2`);
  - HaltTimer;
  - "skipping fork" ou "over memory cap";
  - `info` sem "done: completed paths" (crash).
- **`nondet_assume` do KLEE usa `klee_silent_exit`.** O `klee_assume(0)` gerava `user.err`.
- **AFL++:** leitura após o fim da entrada devolve 0. Antes ela recomeçava o buffer, o
  `while(nondet())` nunca terminava e o `afl-fuzz` abortava no dry run.
- **Memtrack:** intrínsecos de memória do LLVM 16 passaram a ser checados.

## 3. Resultados já registrados no log

- **R16 (Cover-Error, sementes 3a × controle R15):** **148 × 129 cobertas** (+15%), pares
  21 × 2. Os 7 TRUE errados vêm dos dois defeitos já corrigidos.
- **R18:** a correção do abort eliminou 6 de 8 TRUE errados. Os 2 restantes eram
  caminhos descartados, também corrigidos.

## 4. Rodadas em execução (dados em `/home/guilherme/github/prism/tacas-results/`)

- **Build:** `install_r19`, cópia congelada do `build_abort/install` no commit `8c70e5a73`
  (registrado em `r19-commit.txt`).
- **R19:**
  - Escalonador: `r19-sched.sh`, rodando com `setsid`, log em `r19-sched.log`. Os jobs estão
    em `r19-jobs.tsv`, com no máximo 11 contêineres ao mesmo tempo.
  - Cover-Error (`r15-ce.tsv`): control, seeds, alternate, slice, slice-light, slice-o2,
    slice-ntscd, slice-ptafs, 3 shards cada.
  - Cover-Branches (`r15-cb.tsv`): control, seeds, alternate, 2 shards cada.
  - Resultados em `R19-<prop>-<arm>_s<k>/results.csv`.
  - A primeira tentativa foi descartada (`discarded-r19a/`): o build tinha o contador de
    parciais errado e os defeitos da revisão.
- **R17:**
  - Escalonador: `r17-sched.sh`, log em `r17-sched.log`. Divide as vagas com a R19.
  - SV-COMP MemSafety (50), MemCleanup (10) e NoOverflows (20), 120 s, braços control,
    seeds e alternate.
  - Manifests `r17-*.tsv`, regenerados com `build_corpus.py` e os mesmos parâmetros da
    R9/R13. Os originais se perderam em diretórios temporários.
  - Resultados em `R17-<prop>-<arm>/`.
- **R20:** `r20-sched.sh`, que espera a R19 lançar seus 30 jobs. Mede Cover-Error seeds e
  alternate com o ranking do 3c (`install_r20`), para comparar com os mesmos braços da R19.
- **R21:** `r21-sched.sh`. MemSafety e MemCleanup (control) e o CASTLE com `install_r21`
  (checagem de `%s`), para comparar com a R17 e a R14. Gate: nenhum falso positivo novo.
- **R22:** `r22-sched.sh`, que espera a R19. Cover-Branches control, seeds e alternate
  com `MAP2CHECK_FUZZER_SUITE=1` (`install_r23`: o `r22-run.sh` foi reapontado), para
  comparar com a R19.
- **R23:** `r23-sched.sh`, que espera a R19. Cover-Error seeds e alternate com o build
  final (`install_r23`: ponta `feat/tacas-fuzzer-suite`, com a alternância corrigida — sem
  limite na troca KLEE → AFL++, cache dos binários do AFL++, paciência crescente).
  Comparar com a R19. A R20 ficou só com o braço seeds, que isola o 3c.
- **R24:** `r24-sched.sh`, que espera a R19. Cover-Error control e seeds com `install_r24`
  (ponta `feat/tacas-klee-vector-replay`). Comparar o control com o da R19 (efeito do
  replay de vetores) e o seeds com o da R23.
- **Incidente da R19:** o shard 0 do braço seeds tinha 38 linhas do shard 1 e faltavam 42
  tarefas dele. Foi relançado como `r19-ce-seeds-0-resume` (e `control-0-resume` para 2
  faltantes). Na análise, filtrar as linhas pelo índice no manifest (`i % 3 == shard`).
- **R16 Cover-Branches** (`r16-cb-seeds-*`, build antigo) terminando; comparar com a R15.

**Análise pendente:**
- **Cover-Error:** cobertas, TRUE errado e pares discordantes de cada braço contra o
  control. Gate: TRUE errado = 0.
- **Cover-Branches:** cobertura média.
- **R17:** correct-true, correct-false, wrong-true, wrong-false.
  - Um wrong-true bloqueia o braço.
  - Contar TRUE corretos, porque a parada por estagnação pode custar provas.
- **2d:** promover o vencedor entre light, o2, ntscd e ptafs a padrão, com commit próprio.
- **3b:** promover `--alternate-engines` a padrão do híbrido se ele ganhar de seeds.

## 4b. Estado em 2026-09-29, noite

- **Resultados já no log:** R19 (Cover-Error e Cover-Branches) e R21.
  - Cover-Error: seeds +23 −0 contra o control; alternate +20 −0.
  - Cover-Branches: alternate 50,3% contra 44,7%.
  - TRUE errado 0 em todos os braços.
- **Correções da noite:**
  - fd 3 do harness fechado para os filhos;
  - varredura do IR sem regex (2d);
  - orçamento único para as 3 compilações do AFL++ (3b);
  - checagem de `%s` atrás de `MAP2CHECK_CHECK_CSTRINGS=1`, por falso positivo em Juliet
    good.
  - A ponta passou na integração 50/50.
- **Knobs de slicing:** nenhum promovido (as variantes ficaram em ±3).
- **Na fila** (8 vagas, reduzidas por falta de memória): R20, R21-castle, R22, R23, R24
  (e o shard `r24-ce-control-0-resume`). As rodadas usam installs anteriores às correções
  da noite. Isso só afeta slice em programas enormes e o harness, que foi corrigido no repo
  e vale para contêineres novos.

- **R22 control relançado** (`r22-cb-control-*-rerun`): a primeira tentativa deu ERROR nas
  120 tarefas ("cannot create std::vector larger than max_size()", input ilegível no hash)
  durante o pico de falta de memória; os dados foram para `discarded-r22a/`. O hash agora
  falha com mensagem clara (`fix(frontend): an unreadable input program...`).

## 4c. Próxima frente, por decisão do usuário: a lacuna Crab-LLVM → Clam

Depois de fechar a campanha atual e **antes das outras lacunas** (floats, memória não
inicializada, busca dirigida). O ponto de partida:
- O motor antigo era a cadeia de forks `hbgit/crab-llvm` (branch `dev-llvm-6.0`), com
  `hbgit/crab`, `hbgit/sea-dsa` e `hbgit/llvm-dsa`. Os commits públicos do hbgit nesses
  forks são só de porte e build para o LLVM 6. As especializações de que o usuário lembra
  não aparecem ali; **perguntar onde estão**.
- O release do SV-COMP 2020 (`v7.3.1`, `map2check-rc-v7.3-svcomp20.zip`) traz o motor
  compilado em `bin/crabllvm`, com Apron (domínios `oct` e `pk`). O domínio padrão era
  `zones`.
- No Clam `dev16` do `Dockerfile.dev`, o padrão também é `zones`, mas nem Apron, nem Elina,
  nem LDD, nem PPLite são compilados: `oct`, `pk` e `boxes` ficam indisponíveis. O
  comentário do Dockerfile que fala em "intervalos" está errado.
- O `--add-invariants` sobre o Clam **nunca foi medido** (o gate previsto é CASTLE e Juliet
  sem perder detecção).
- **Sonda de 2026-09-29, num programa com laço.** O motor do v7.3.1 roda em
  `python:2.7-slim` com o clang do LLVM 6 que vem no próprio release.
  - As flags do v7.3.1, tiradas das strings do binário, incluem `--crab-promote-assume`:
    **17 invariantes emitidos como `llvm.assume`**.
  - O Clam, com as flags atuais, emite **18 como `verifier.assume`**, que o NonDetPass
    converte em `klee_assume`.
  - O NonDetPass antigo só reconhecia `verifier.assume`, e o **KLEE ignora `llvm.assume`**
    (testado no 3.1). Então, no v7.3.1, os invariantes **não chegavam ao KLEE como
    restrição**. Se ajudavam, era pelo otimizador do LLVM, que dobra ramos com base em
    `llvm.assume`. O mecanismo é outro, e é provavelmente isso que "não é a mesma coisa"
    quer dizer.
  - Os dois motores produzem fatos no estilo de zones (`x − y ≤ c`).
- **INV-1 feito** (log de experimentos). Os invariantes estavam inertes desde a v7.3
  (out/2018). O ganho observado vinha do pré-processamento em SSA, não dos invariantes.
  Branch `feat/tacas-invariants`:
  - `MAP2CHECK_CLAM_PROFILE=default|memory|none`;
  - `MAP2CHECK_PREOPT=ssa`, só para alcançabilidade e assert;
  - build `build_inv` com `-DENABLE_CLAM=ON` e os links `lib/klee/runtime` e `lib/clang`
    criados à mão.
- **R25 na fila** (`r25-sched.sh`, `install_r25`):
  - Cover-Error, shard 0 (71 tarefas): off, ssa, clam-none, clam-default, clam-memory;
  - MemSafety (50): off, clam-none, clam-default, clam-memory.
  - Gate: nenhuma detecção perdida.
- Primeiro passo (feito, ver INV-1): um **inventário diferencial**. Rodar o motor do v7.3.1 e o Clam `dev16`
  nos mesmos programas e comparar os invariantes, antes de portar qualquer coisa.

## 5. Depois das rodadas

1. Registrar R17 e R19 no log.
2. Promover os knobs vencedores.
3. Push das 4 branches e PRs em cadeia, sem merge, com os números na descrição.
4. **3c:** ranking de sementes (novidade de cobertura, densidade SDG).
5. **Ideia anotada:** usar o corpus do AFL++ como casos de Cover-Branches, via replay
   tipado. Hoje a suíte de CB sai só do KLEE.
6. **Perda de 2026-09-29:** o `release/results/map2check_castle.2026-07-17_04-53-54.results.sv-comp19_map2check.txt`
   foi apagado por engano e não tem cópia. Regera-se com `tests/castle/run_castle_benchexec.sh`.
   `release/` foi restaurado a partir de `install_v15/`.
7. O worktree velho `../Map2Check-2c` precisa de `--force` para ser removido. **Pedir
   permissão** antes.

## 6. Campanha completa da 9.0 (lançada em 2026-09-30, 14h16)

- **Lançador:** `../tacas-results/v9-campaign.sh`, rodando com `setsid`, log em
  `../tacas-results/v9-campaign.log`. Ele espera o R26 terminar e roda sozinho, na ordem
  do plano: CASTLE, Cover-Error (1087, 3 shards), Juliet (grupos a–d, como a v15) e
  Cover-Branches (2765, 6 shards).
- **Build:** `install_v9`, cópia congelada da ponta de `feat/map2check-9.0`
  (`v9-commit.txt`). Um commit de código novo antes do merge exige rodar de novo a parte
  afetada.
- **Imagem:** `map2check-v9-eval:latest`, que é a `map2check-dev:aflpp` com o TestCov.
- **Condições da v15:** `--memory=4g`, `--cpus=2` (Test-Comp) e `--cpus=1` (Juliet e
  CASTLE), 300 s, `PER_FAMILY=10`; no máximo 5 contêineres.
- **Resultados:** `../tacas-results/V9-*`, pareados com `tests/*/results_v15*`.
