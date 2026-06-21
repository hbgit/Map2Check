# Outline do Artigo — Map2Check 2026 (SBSeg SF)

**Instrução:** Este outline serve como guia de escrita. Cada item representa um parágrafo ou bloco de conteúdo. O estado de cada item é atualizado conforme o draft avança.

---

## Metadados do Edital (atualizado 2026-06-18)

| Campo | Valor |
|:------|:------|
| **Conferência** | SBSeg 2026 — Salão de Ferramentas (SF) |
| **Submissão** | 20/jul/2026 |
| **Notificação** | 03/ago/2026 |
| **Camera ready** | 14/ago/2026 |
| **Evento** | 01–04/set/2026, Búzios — RJ |
| **Modo** | Single-blind (autores conhecidos pelos revisores) |
| **Idioma** | Português ou inglês (a decidir) |
| **Limite** | 8 pág corpo + até 2 pág referências/apêndices |
| **Template** | SBC |
| **Modalidade** | Código Aberto (elegível a prêmios) |
| **Vídeo obrigatório** | Sim — URL de instalação e funcionalidades |
| **Demonstração ao vivo** | Obrigatória — pelo menos 1 autor inscrito |
| **CTA** | Após notificação (até 14/ago), integrado ao camera ready |
| **IA Generativa** | Verificar Código de Conduta SBC — declarar uso se aplicável |

---

## Seção 1 — Introdução e Motivação (~1 página)

### 1.1 O problema de memory safety em C/C++
- [ ] Abrir com estatísticas de vulnerabilidades de memória (CWEs 119, 416, 415, 401, 787)
- [ ] Citar que memory-safety bugs continuam entre as principais causas de CVEs exploráveis
- [ ] Contextualizar: sistemas embarcados, infraestrutura crítica, consequências de falhas

### 1.2 Verificação híbrida como abordagem promissora
- [ ] Breve explicação da combinação análise estática + dinâmica (fuzzing + execução simbólica)
- [ ] Citar trabalhos de ponta: FuSeBMC (smart seeds), Symbiotic (slicing + KLEE)
- [ ] Destacar que ferramentas de competição (SV-COMP, TestComp) impulsionam o estado da arte

### 1.3 Posicionamento do Map2Check
- [ ] Map2Check como verificador híbrido consolidado para programas C
- [ ] Menção às publicações anteriores (TACAS 2016, 2018, 2020)
- [ ] O problema: estagnação desde 2019 (LLVM 6.0, Ubuntu 16.04, último release nov/2019)

### 1.4 Contribuições desta versão (2026)
- [ ] Lista numerada de contribuições:
  1. Migração completa da stack: LLVM 6 → 16, New Pass Manager, C++17
  2. Infraestrutura de engenharia: CI/CD, Docker, sanitizers, static analysis
  3. Validação em benchmarks reais: TestComp 2026 (Heap, 594 tasks, score 57)
  4. Fortalecimento do posicionamento em cibersegurança (mapeamento CWE)

---

## Seção 2 — História e Evolução (~1 página)

### 2.1 Linha do tempo (2016→2026)
- [ ] **2016 (TACAS):** "Hunting Memory Bugs" — abordagem BMC com CBMC
- [ ] **2018 (TACAS):** Migração para LLVM + KLEE — execução simbólica como motor principal
- [ ] **2020 (TACAS):** Abordagem híbrida — fuzzing (LibFuzzer) + execução simbólica (KLEE) + invariantes indutivos (Crab-LLVM)
- [ ] **2019–2025:** Estagnação — último release, LLVM 6.0 obsoleto, dependências deprecadas
- [ ] **2026:** Renascimento — modernização sistemática da stack completa
- [ ] **Figura 1:** Timeline visual da evolução

### 2.2 O que motivou o renascimento
- [ ] Obsolescência técnica: LLVM 6.0 EOL, Ubuntu 16.04 EOL, KLEE 2.0 sem suporte
- [ ] Impossibilidade de competir em SV-COMP/TestComp com stack legada
- [ ] Necessidade de sustentabilidade de engenharia (CI/CD, reproducibilidade, qualidade de artefato)
- [ ] Alinhamento com cibersegurança: CWEs como linguagem comum entre verificação formal e indústria de segurança

---

## Seção 3 — Arquitetura e Funcionalidades (~2.5 páginas)

### 3.1 Pipeline de verificação (visão geral)
- [ ] Fluxo completo: Código C → clang-16 → LLVM IR → opt (passes) → llvm-link → runtime → KLEE / LibFuzzer → Veredito + Witness
- [ ] **Figura 2:** Diagrama do pipeline com caixas e setas

### 3.2 Passes de instrumentação (New Pass Manager)
- [ ] Descrição dos 9 passes migrados do Legacy PM para o New PM:
  - `AssertPass` — instrumenta asserts do verificador
  - `TargetPass` — marca função-alvo (e.g., `reach_error`)
  - `LoopPredAssumePass` — injeta predicados em laços
  - `Map2CheckLibrary` — renomeia `main` para `__map2check_main__`
  - `NonDetPass` — substitui `__VERIFIER_nondet_*` por chamadas simbólicas
  - `TrackBasicBlockPass` — rastreia cobertura de blocos básicos
  - `OverflowPass` — instrumenta operações aritméticas para overflow
  - `GenerateAutomataTruePass` — gera autômato de witness (GraphML)
  - `MemoryTrackPass` — instrumenta acessos a memória (alloc, free, deref)
- [ ] **Figura 3:** Diagrama dos passes com inputs/outputs e dependências
- [ ] Destacar: `isRequired()` como fix crítico (sem isso, `opt -O0` pula todos os passes)

### 3.3 Motores de análise
- [ ] **KLEE 3.x** (LLVM 16): execução simbólica, geração de testes, contra-exemplos
- [ ] **LibFuzzer** (LLVM 16): fuzzing guiado por cobertura, rápido para bugs superficiais
- [ ] **Iterative deepening:** combinação dos dois motores em fases

### 3.4 Modernização de engenharia (pilar de sustentabilidade)
- [ ] **LLVM 16 + New Pass Manager:** compatibilidade com pipelines modernos, Opaque Pointers
- [ ] **C++17:** `std::filesystem`, structured bindings, modernização do frontend
- [ ] **CI/CD (GitHub Actions):** build automatizado, unit tests (7/7), pass plugin load tests (9/9)
- [ ] **Docker:** `Dockerfile.dev` (Ubuntu 22.04 + LLVM 16) — reprodutibilidade garantida
- [ ] **Static analysis:** `.clang-tidy` (C++), `.cppcheck-suppressions.txt` (C), `scripts/run-static-analysis.sh`
- [ ] **Sanitizers:** ASAN, UBSAN, TSAN targets no CMake
- [ ] **OpenSSF Best Practices:** checklist em andamento para badge
- [ ] **Figura 5:** Workflow CI/CD (diagrama GitHub Actions)
- [ ] **CTA:** Esta infraestrutura pontua diretamente nos critérios de artefato (disponibilidade, funcionalidade, reprodutibilidade, sustentabilidade)

---

## Seção 4 — Features de Cibersegurança (~2 páginas)

### 4.1 Mapeamento de memory safety para CWEs
- [ ] **CWE-401:** Memory Leak — detecção via `MemoryTrackPass` + runtime library
- [ ] **CWE-416:** Use-After-Free — detecção via tracking de ponteiros
- [ ] **CWE-415:** Double-Free — detecção via log de alocações
- [ ] **CWE-119 / CWE-787:** Buffer Overflow — detecção via `OverflowPass` + bounds checking
- [ ] **Tabela:** Mapeamento propriedade SV-COMP → CWE → pass responsável

### 4.2 TestComp 2026 — Execução Heap
- [ ] Contexto: TestComp 2026, sub-categoria `C.coverage-error-call.Heap`
- [ ] Configuração: timeout 300s, Z3, target `reach_error`, Docker single-container
- [ ] **Tabela de resultados consolidados:**
  - TRUE: 264 (44.4%)
  - UNKNOWN: 271 (45.6%)
  - FALSE: 56 (9.4%)
  - TIMEOUT: 2 (0.3%)
  - Total: 594 tasks | Score: 57
- [ ] **Tabela por set:**
  - Heap (428 tasks): 39 FALSE, 203 TRUE, 184 UNKNOWN, 2 TIMEOUT
  - LinkedLists (166 tasks): 18 FALSE, 61 TRUE, 87 UNKNOWN, 0 TIMEOUT
- [ ] **Figura 4:** Gráfico de barras ou pizza dos resultados

### 4.3 Smoke test e validação de vereditos
- [ ] Execução E2E manual em benchmarks `loops/array-1.c` e `loops/array-2.c`
- [ ] `array-1.c`: TRUE (unreachable) → VERIFICATION SUCCEEDED ✅
- [ ] `array-2.c`: FALSE (reachable) → VERIFICATION FAILED + counter-example ✅
- [ ] Counter-example validado: `__VERIFIER_nondet_int() → 31763`, linha 19, escopo `main`
- [ ] Framework de checkpoint automatizado: `run_checkpoint.sh`, `verify_verdicts.sh`

### 4.4 Bugs críticos encontrados e corrigidos
- [ ] **Tabela dos 3 bugs:**
  | Bug | Sintoma | Causa | Fix |
  |:----|:--------|:------|:----|
  | KLEE 3.x flags | `Unknown command line argument` | Flags removidas na v3.x | Atualizar `caller.cpp` |
  | `isRequired()` | Passes pulados silenciosamente | `optnone` + New PM | `isRequired()=true` em 9 passes |
  | Target function | `__VERIFIER_error` em vez de `reach_error` | env var vs `cl::opt` | Passar `-function-name` na CLI |
- [ ] Destacar: nenhum desses bugs era detectável pelos testes unitários existentes (7/7)
- [ ] Implicação: a necessidade de testes de integração E2E

### 4.5 Suporte a WebAssembly (WASM) — Em Desenvolvimento
- [ ] Extensão do pipeline LLVM 16 existente para análise de módulos WASM
- [ ] Backend LLVM-WASM: compilação C → LLVM IR → WASM → instrumentação → verificação
- [ ] Vetor de ataque: memory safety em runtimes WASM (Wasmtime, Wasmer)
- [ ] Testes preliminares em benchmarks SV-COMP adaptados para WASM
- [ ] Status: implementação em andamento, resultados preliminares na submissão
- [ ] **NÃO é trabalho futuro — é feature em desenvolvimento ativo**

---

## Seção 5 — Demonstração Planejada (~0.5 página)

### 5.1 O que será demonstrado ao vivo
- [ ] Pipeline completo em Docker: compilação → instrumentação → execução → veredito
- [ ] Exemplo interativo: execução em benchmark `loops/array-2.c` mostrando:
  - compilação com `map2check`
  - instrumentação pelos 9 passes
  - execução com KLEE
  - geração de counter-example
- [ ] Execução do framework TestComp 2026 (subset) mostrando throughput

### 5.2 Equipamentos necessários
- [ ] Laptop com Docker instalado
- [ ] Imagem `map2check-dev` pré-construída (ou build no local, ~30 min)
- [ ] Subconjunto de benchmarks SV-COMP (`testcomp26` tag)
- [ ] Acesso à internet (para pull da imagem Docker, se necessário)

### 5.3 Vídeo técnico (obrigatório no edital)
- [ ] Link do vídeo (será preenchido após gravação)
- [ ] Duração: 5–7 minutos
- [ ] Conteúdo: instalação (Docker), build, execução E2E, resultados TestComp 2026
- [ ] **Roteiro detalhado:** ver `video/roteiro-gravacao.md`
- [ ] URL será incluída no artigo (não precisa ser anônima — single-blind)

### 5.4 Demonstração ao vivo (condição de aceitação)
- [ ] Pelo menos 1 autor deve estar inscrito no evento (01–04/set, Búzios)
- [ ] A demonstração será o pipeline E2E em Docker + benchmarks SV-COMP
- [ ] Equipamentos: laptop com Docker + imagem map2check-dev + benchmarks

---

## Seção 6 — Avaliação, Perspectivas e Conclusão (~1 página)

### 6.1 Resumo dos resultados
- [ ] Modernização completa da stack: 35 commits, 84 arquivos, +5.785/−657 linhas
- [ ] 9 passes migrados para New PM, 7/7 unit tests passando, 9/9 passes carregando
- [ ] TestComp 2026 Heap: 594 tasks, score 57, 56 bugs reais encontrados (9.4%)
- [ ] 3 bugs críticos de regressão descobertos e corrigidos durante a migração

### 6.2 Limitações
- [ ] Taxa de UNKNOWN alta (45.6%): necessita investigação de timeout tuning e estratégia de solver
- [ ] Ausência de testes de integração E2E no CI: todos os 3 bugs foram perdidos pelos unit tests
- [ ] ControlFlow benchmarks usaram property `no-overflow` — incomparável com modo reachability
- [ ] C++17 dead store warnings em libc++ headers (documentado, não crítico)

### 6.3 Perspectivas
- [ ] Aprofundamento do suporte a WebAssembly: expansão dos benchmarks, modelagem formal da memória linear WASM
- [ ] Redução da taxa de UNKNOWN: tuning de timeout, estratégias de solver SMT, heurísticas de path exploration
- [ ] Testes de integração E2E no CI: automatizar o pipeline completo compile → instrument → link → execute
- [ ] (NÃO mencionar: DG Library, AFL++, Coordenador, Smart Seeds — fora do escopo deste artigo)

### 6.4 Conclusão
- [ ] O Map2Check renasceu como plataforma sustentável de verificação de memory safety
- [ ] A modernização de engenharia (LLVM 16, CI/CD, Docker, static analysis) é um pré-requisito fundamental para qualquer avanço futuro
- [ ] Os resultados do TestComp 2026 demonstram viabilidade competitiva
- [ ] O projeto está aberto a contribuições (open source, repositório público)

---

## Apêndice — Reprodutibilidade (~1 página)

### A.1 Build com Docker
- [ ] Comandos para build da imagem: `docker build -f Dockerfile.dev -t map2check-dev .`
- [ ] Comandos para execução: `docker run -it --rm map2check-dev`
- [ ] Verificação: `clang-16 --version`, `opt --version`, `cmake --version`

### A.2 Execução de benchmarks
- [ ] Comandos para smoke test: `map2check --property-file ...`
- [ ] Comandos para TestComp 2026: `cd test-comp2026/simulation && ./run_checkpoint.sh`
- [ ] Verificação de vereditos: `./verify_verdicts.sh`

### A.3 CI e qualidade
- [ ] Link para GitHub Actions (nomes/instituições podem constar — single-blind)
- [ ] Comandos para static analysis: `./scripts/run-static-analysis.sh`
- [ ] Comandos para sanitizers: `cmake -DENABLE_SANITIZER_ADDRESS=ON ...`

### A.4 Declaração de IA Generativa (se aplicável)
- [ ] Verificar Código de Conduta SBC: https://sol.sbc.org.br/index.php/indice/conduta
- [ ] Declarar uso de ferramentas de IA no processo de escrita/revisão se aplicável

---

*Estado do outline: Sprint 0 — estrutura completa, aguardando redação secção a secção.*
*Última atualização: 2026-06-18 (revisado com edital oficial SBSeg 2026 SF)*
