# Test-Comp: checklist de progresso

Estado das frentes do [plano de desenvolvimento](../.opencode/Plano%20de%20desenvolvimento_Testcomp.md)
(local, fora do repositório). Este arquivo é o índice vivo: cada item aponta para a
evidência que sustenta o estado declarado.

**Última atualização:** 2026-08-22 (noite)

## Legenda

| Marca | Significado |
|---|---|
| ✅ | Feito e verificado, com evidência apontada |
| 🟡 | Parcial — o que falta está dito explicitamente |
| ⬜ | Não iniciado |
| ❌ | Tentado e rejeitado, com motivo registrado |

---

## H1 — Conformidade (submissão válida e pontuável)

| Item | Estado | Evidência / o que falta |
|---|---|---|
| **H1.1** Emissor de test suite XML | ✅ | `modules/frontend/test_suite/`. `--generate-test-suite` emite `metadata.xml` + `testcase-N.xml` no formato de intercâmbio. 13 testes unitários + 12 asserções de integração. **Custo real muito abaixo das 2 pw estimadas**: o runtime já registrava os valores em ordem de consumo, faltava só serializar |
| **H1.2** Conversor ktest→XML | ✅ | **Colapsou dentro do H1.1.** O log é escrito pelo binário instrumentado em tempo de execução, então já carrega os valores concretos do KLEE. Não precisou de conversor separado |
| **H1.3** Vetores de entrada por caminho | ✅ | **Resolvido por outro caminho que o planejado.** O KLEE já grava um `.ktest` por caminho explorado, com os objetos simbólicos em ordem de consumo; `ktest_reader.cpp` os lê. A alternativa (o runtime gravar um log por estado) foi **medida e descartada**: levou um run de 1s/`FALSE` para 100s/`SUCCEEDED` errado, porque cada escrita é chamada externa que o KLEE executa concretamente. 16 testes unitários fixam o parse contra bytes montados à mão |
| **H1.4** Empacotamento BenchExec / fm-tools | ✅ | `utils/moduleBenchExec/map2check_testcomp.py` (`BaseTool2`) + `utils/map2check-testcomp-wrapper.py`, ambos no zip de release. 15 asserções contra o benchexec real em `tests/integration/test_benchexec_toolinfo.py`. **Falta:** registrar em `fm-tools` (fora deste repositório). O `map2check.py` SV-COMP segue em `BaseTool` 1.x — obsoleto mas ainda presente no benchexec 3.35, então não está morto |
| **H1.5** E2E no CI com TestCov | ✅ | Job `Test-Comp Validation (TestCov)`, 6/6 contra manifesto medido. Ver `tests/testcomp/` |

## H2 — Eficácia

| Item | Estado | Evidência / o que falta |
|---|---|---|
| **H2.1** Modo reachability enxuto | 🟡 | **Premissa do plano desatualizada.** O plano supõe "9 passes sempre ativos"; `caller.cpp:141-218` mostra que reachability já roda só `nondet-pass` + `target-pass` + `map2check-library`. O overhead residual é o runtime sempre linkado, não os passes. O critério "redução ≥30%" precisa ser re-derivado |
| **H2.2** Orquestração com time-slicing | ⬜ | Default híbrido atual (LibFuzzer 0.2× → KLEE 0.8×) preservado |
| **H2.3** Seed exchange fuzzer↔KLEE | ⬜ | — |
| **H2.4** Slicing pré-simbólico | ⬜ | Ver nota de prioridade abaixo |
| **H2.5** Modo Cover-Branches | ✅ | `--cover-branches` emite um test case por caminho do KLEE, `coversError` falso (são caminhos, não violações), teto de 500. Wrapper e tool-info **aceitam** a propriedade. Validado ponta a ponta: 7 test cases num programa de 6 ramos, **TestCov 100,0%**. Gate no CI (`test_cover_branches.sh`) mede cobertura pelo validador, não inspecionando XML |

### Nota de prioridade sobre H2

O plano atribui os 45,6%/50,7% de UNKNOWN a explosão de caminhos e conclui que o
acionável é orquestração e poda (H2.2–H2.4, 4–8 pw). Duas coisas medidas depois
disso mudam o quadro, e **nenhuma das duas é explosão de caminhos**:

1. O baseline v5 mostra que **887 de 2.525 casos (35%) recebem entrada por externas
   não resolvidas**, e 575 se perdem. Quatro fontes de entrada ficam em exatamente
   **50,0%** de acerto entre os decididos — assinatura de provar a variante segura e
   errar a vulnerável.
2. O **achado K, corrigido**: o timeout descartava violações que a ferramenta
   já tinha encontrado e gravado em disco. `isTimeout()` tinha precedência sobre
   a propriedade violada, então um FALSE legítimo virava UNKNOWN só porque o
   orçamento acabou *depois* do trabalho estar feito.

**O que a medição do v6 respondeu.** No CASTLE, a correção do K mudou **zero**
vereditos — e o motivo não é que K fosse pequeno, é que o **achado B o mascara**:
dos 22 casos indecididos no v5 que rerodaram, 20 usam o modo degenerado, onde
nenhuma violação chega a ser registrada e portanto K não tem o que resgatar.
Medir K exige um corpus onde os modos sejam reais — o Juliet, ainda em execução.

**Recomendação de sequência:** corrigir o achado B antes de comprometer qualquer
pessoa-semana em slicing. Ele bloqueia 45% das execuções do CASTLE *e* impede
medir o efeito de K nesse corpus.

---

## Frentes fora do plano original

| Frente | Estado | Evidência |
|---|---|---|
| **Invariantes (`--add-invariants`)** | ✅ **vivo e provado em CI** | Clam `dev16` pinado, `-DENABLE_CLAM=ON`, **desligado por default**. O job `Invariants (Clam)` passou de `ABSENT` para `PRESENT (exit 0)` quando a imagem republicou; a sonda do Dockerfile confirma `Clam injects invariants: OK`. Issue #54 fechada. Ver [a revisão da dependência](reports/2026-08-16-crabllvm-review.md) |
| Invariantes: gate diferencial p/ promover a default | ⬜ | Task 7 do [plano de revival](superpowers/plans/2026-08-16-add-invariants-revival.md). Depende de rodar sobre corpus real |
| **Baseline CASTLE + Juliet (v5)** | ✅ | PR #55. CASTLE 98,2% precisão, Juliet 90,8% (99,2% descontando artefatos de mapeamento) |
| Corrida v6 (depois/antes) | 🟡 | **CASTLE fechado** (217/250): precisão 98,2%, recall 74,0%; excluindo o modo degenerado do achado B, recall 79,4%. Juliet: 3 de 4 fatias fechadas, `c` em andamento. **Regra de atribuição: invariantes DESLIGADOS na corrida principal**, senão overflow + timeout + amostragem + invariantes ficam conjuntamente não creditáveis |
| **Corpus Test-Comp estratificado** | 🟡 | `fetch-benchmarks.sh` + `build_corpus.py` + `run_testcomp_evaluation.sh`. 372 tarefas cover-error e 480 cover-branches, cota por subcategoria e passo determinístico. Corrida de 2026-08-22 em andamento; ver [as notas de leitura](../tests/testcomp/RUN-2026-08-22-NOTES.md) — **o orçamento é 60s contra os 900s da competição, não são scores** |
| **Alcançabilidade (CWE-843/628/770/835)** | ⬜ | Adiado por decisão. Sob a lente SV-COMP as quatro colapsam em uma: só 835 mapeia para categoria pontuável (Termination), e os wrappers declaram não suportar |

---

## Defeitos encontrados e corrigidos nesta fase

Todos com evidência em [findings](reports/2026-08-12-castle-juliet-findings.md).

| # | Defeito | Estado |
|---|---|---|
| **H** | Use-after-scope no log de nondet: `NONDET_CALL.value` guardava o endereço de um parâmetro já morto. **O log saía vazio em toda execução simbólica** | ✅ valor inline em união |
| **I** | `map2check_nondet_double` marcava o tipo como `UNSIGNED` | ✅ |
| **J** | Estados bifurcados do KLEE sobrescreviam o `klee_log.csv` uns dos outros | ✅ flush só na violação |
| — | `--add-invariants` aceito e ignorado sem rastro na entrada `.bc` (issue #54) | ✅ sai com código 3 |
| — | Invocação legada do Crab: `--llvm-pp-loops` e `--crab-promote-assume` anulavam a injeção | ✅ removidas, medido |
| — | Pass 2 do CASTLE era re-execução byte-idêntica do pass 1 em todo o v5 | ✅ reconhece código 3 |
| **K** | `isTimeout()` descartava violações já encontradas e gravadas: `TARGET-REACHED found` → `Forcing timeout` → `VERIFICATION UNKNOWN`. Disparava em 80% das execuções do Juliet v5 | ✅ resgate restrito a evidência confiável (`c4ef3a355`) |
| — | Imagem: Clam compilava com GCC 11 em vez de clang-16 (o `ENV CC` está 88 linhas abaixo do `RUN`), e GCC rejeita o que o clang só avisa. **A receita nunca compilou** | ✅ compiladores nomeados no `CLAM_CFG` (#58) |
| — | `Dockerfile.dev` só era compilado **depois** do merge, então um Dockerfile inválido chegava à `develop` sem nada poder barrá-lo | ✅ o workflow de publish roda em PRs que tocam o arquivo, com `push: false` (#58) |
| — | Clam e suas três dependências vinham de branches móveis; `crab@dev` mudou em 21/08 | ✅ pinados por SHA como `ARG` (#59) |

## Defeitos abertos

| # | Defeito | Prioridade |
|---|---|---|
| **K2** | Custo por leitura nondet cresce muito: 1 → 21 → 114 → 861 caminhos parciais para 2 → 4 leituras. Nenhum `.err`, então não são erros | Média — segundo fio a puxar |
| **G** | `main()` descarta o retorno de `map2check_execution()`; código de saída nunca reflete a análise | Média |
| **B** | **Medido no v6.** Modo reachability degenerado com `target-function-name main`: 98 de 217 execuções, **zero vereditos FALSE**, 59 programas vulneráveis, 0 detectados. 12 TN e 5 FN pontuados sobre um oráculo que só sabe responder "correto" | **Alta** — 🟡 o caso trivial agora se anuncia (`TargetPass`); falta corrigir o mapeamento CWE→modo do harness, o que **muda a pontuação do baseline** e por isso espera o v6 fechar |
| — | Garantia de ordenação do corpus depende de um único programa (`two_guards.c`) | Baixa — K já foi corrigido; falta reconferir se ainda vale |
| — | `CLAUDE.md` linka um `.clang-format` que não existe | Baixa |

---

## Infraestrutura de verificação

| O quê | Contagem |
|---|---|
| `ctest` (unitários) | 8 |
| Integração | ~110 asserções em 11 scripts |
| Conformidade Test-Comp | 6 programas |
| Jobs de CI | 10, todos verdes na PR #59 (o 10º builda a imagem no próprio PR) |

Gates que provei que **conseguem falhar** (um gate nunca visto vermelho não protege
ninguém): o do TestCov, invertendo a ordem dos `<input>` no emissor; e o de
invariantes, que falhava com `exit 0 but no verifier.assume in any bitcode` antes da
correção.
