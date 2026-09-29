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
| `feat/tacas-memtrack-intrinsics` | fix: `memset/memcpy/memmove` checados no LLVM 16 (despacho por `MemSetInst`/`MemTransferInst`) |

- **Ponta da pilha:** `feat/tacas-memtrack-intrinsics` @ `8c70e5a73`, mais este documento.
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
