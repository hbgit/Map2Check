---
name: map2check-sbseg2026
description: "Contexto completo do artigo SBSeg 2026 SF — Map2Check 2026. Metadados de submissão, frameworks, capítulos, backlog, restrições de edital, dados consolidados e conhecimento técnico. Ative ao trabalhar nos docs do paper."
---

<!-- argument-hint: [topic, framework name, or chapter number] -->

# Map2Check 2.0 — SBSeg 2026 SF
**Autores**: Guilherme Lucas Pereira Bernardo (UFRN), Inácio Viana (UFRN), Francisco Nobre (UFRR), Herbert Oliveira Rocha (orientador, UFRR) | **Submissão**: 20/jul/2026 | **Sections**: 5 | **Single-blind** — nomes podem constar

## How to Use This Skill

- **Without arguments** — load core frameworks + submission context
- **With a topic** — ask about `pass migration`, `CWE mapping`, `isRequired fix`, `WASM`, `backlog`; I find and read the relevant chapter
- **With chapter** — ask for `ch03`; I load that specific chapter
- **Browse** — ask "what chapters do you have?" to see the full index

When you ask about a topic not covered below, I will read the relevant chapter file before answering.

---

## 📋 Submission Context (PRIORIDADE MÁXIMA)

| Campo | Valor |
|:------|:------|
| **Conferência** | SBSeg 2026 — Salão de Ferramentas (SF) |
| **Sistema de submissão** | JEMS3 |
| **Idioma** | Português |
| **Deadline submissão** | **20/jul/2026** |
| **Notificação** | 03/ago/2026 |
| **Camera ready + CTA** | 14/ago/2026 |
| **Evento** | 01–04/set/2026, Armação dos Búzios — RJ |
| **Modo** | Single-blind (autores conhecidos pelos revisores — nomes/instituições/emails OK) |
| **Template** | SBC (a ser obtido) |
| **Limite** | 8 pág corpo + até 2 pág referências/apêndices |
| **Vídeo obrigatório** | Sim (5-7 min) — URL pública, não precisa ser anônima |
| **Demonstração ao vivo** | Obrigatória — pelo menos 1 autor inscrito (presença CONFIRMADA) |
| **Modalidade** | Código Aberto (elegível a prêmios) |
| **IA Generativa** | Uso DECLARADO — ferramentas de IA como assistentes de escrita, conforme Código de Conduta SBC (https://sol.sbc.org.br/index.php/indice/conduta) |

## 📝 Backlog

| # | Item | Prioridade | Bloqueia | Owner |
|:--|:-----|:-----------|:---------|:------|
| 1 | Referências BibTeX (TACAS 2016, 2018, 2020 + concorrentes) | **Alta** | Seção 2 (História) + Referências | usuário |
| 2 | Template LaTeX SBC | **Alta** | Geração do PDF final | usuário |
| 3 | Aprovação do orientador no outline | **Alta** | Sprint 1 | usuário |
| 4 | ~~WASM PoC E2E (1.7.3)~~ | ~~Alta~~ | ~~Seção WASM~~ | ✅ **Concluído** |
| 5 | ~~WASM MemoryTrackPass (1.7.4)~~ | ~~Alta~~ | ~~PoC E2E~~ | ✅ **Concluído** |
| 6 | ~~WASM CLI --wasm (1.7.5)~~ | ~~Alta~~ | ~~1.7.4~~ | ✅ **Concluído** |
| 7 | WASM Juliet benchmarks (1.7.6) | Média | 1.7.5 | ✅ Concluído — 15 casos, 15/15 FALSE (com timeout 290s) |
| 8 | WASM seção artigo (1.7.7) | Média | 1.7.6 | ✅ Concluído — texto e tabela em main.tex, fig6.mmd criado, roteiro do vídeo pronto |
| 9 | Gravação do vídeo técnico | **Média** | Seção 5 (Demonstração) | usuário |
| 10 | Validação do build por terceiro (CTA) | **Baixa** | Checklist de artefato | usuário |
| 11 | Ajustes do orientador no artigo | **Alta** | Submissão | usuário |

## 🛡️ CTA Checklist (Avaliação de Artefato)

| Dimensão | Evidência |
|:---------|:----------|
| **Disponibilidade** | Repositório público + Docker image (GHCR) |
| **Funcionalidade** | Build passa + 7/7 unit tests + 9/9 passes carregando |
| **Reprodutibilidade** | Dockerfile.dev (Ubuntu 22.04) + scripts de benchmark |
| **Sustentabilidade** | CI/CD (GitHub Actions): build, unit tests, static analysis, sanitizers, e novo job E2E com Docker; docs atualizados |

## 📊 Consolidated Data

```
Stack:         LLVM 16.0 · KLEE 3.1 · C++17 · Ubuntu 22.04 · Docker
Migração:      35 commits · 84 arquivos · +5.785 / -657 linhas
Unit tests:    7/7 passing
Pass plugins:  9/9 loading (opt-16)
Smoke E2E:     2/2 loops benchmarks correct (array-1.c TRUE, array-2.c FALSE)

TestComp 2026 Heap (C.coverage-error-call.Heap):
  Total:    594 tasks   | Score: 57
  TRUE:     264 (44.4%)
  UNKNOWN:  271 (45.6%)
  FALSE:     56 (9.4%) — 56 bugs reais encontrados
  FALSE(free): 1
  TIMEOUT:    2 (0.3%)
  ├─ Heap (428):        39 FALSE, 203 TRUE, 184 UNKNOWN, 2 TIMEOUT
  └─ LinkedLists (166): 18 FALSE,  61 TRUE,  87 UNKNOWN, 0 TIMEOUT

Bugs críticos corrigidos:  3 (KLEE flags, isRequired, target function)
Bugs OverflowPass corrigidos: 3 (null pointer dereference em chamadas indiretas)

TestComp 2026 ControlFlow (no-overflow):       92 tasks, 30 FALSE, 17 TRUE, 45 UNK, 0 TO (1h43m)
TestComp 2026 ControlFlow (coverage-error-call): 138 tasks, 38 FALSE, 30 TRUE, 70 UNK, 0 TO (4h53m)

WASM Pipeline:
  Branch:          feat-wasm-verification
  WABT:            1.0.41 (wasm2c)
  wasi-sdk:        33.0 (clang --target=wasm32-wasip1)
  WasmLifter:      ✅ modules/frontend/wasm_lifter.{hpp,cpp} — lifting funcional
  Entry point:     ✅ w2c_*_start → main via generateWasmWrapperStatic
  CLI --wasm:      ✅ map2check --wasm modulo.wasm — pipeline único
  WasmRuntimeStubs:✅ KLEE-friendly (calloc/free), wasm_rt_trap → map2check_error
  Wrapper auto:    ✅ generateWasmWrapperStatic() — main() → wasm bridge
  Bounds check:    ✅ Per-allocation via dlmalloc interception (w2c_*_dlmalloc/dlfree)
                   implementado e validado
  Juliet benchmarks:✅ 15 casos (CWE-121/122/124/126/127), 15/15 FALSE
                   timeout 110s → UNKNOWN; timeout 290s → FALSE
  Integration tests:✅ tests/integration/test_wasm_{pipeline,entrypoint}.sh (3/3 + 3/3)
  Seção no artigo: 🟡 Texto base em main.tex, figura pipeline pendente
  KLEE E2E:        ✅ Pipeline completo: .wasm → LLVM IR → Passes → KLEE → FALSE
  CI build:        ✅ find_path corrigido, WasmRuntimeStubs opcional
  CI E2E:          ✅ Novo job e2e-wasm executa integração dentro do Docker
```

## 🖼️ Figure Inventory

| ID | Descrição | Seção | Status |
|:---|:----------|:------|:------|
| `fig1-timeline` | Timeline visual 2016→2026 | Sec 2 | ✅ TikZ criado (fig1-timeline.tikz.tex) |
| `fig2-pipeline` | Diagrama do pipeline de verificação (5 estágios) | Sec 3 | ✅ TikZ criado (fig2-pipeline.tikz.tex) |
| ~~`fig3-passes`~~ | ~~Diagrama dos passes~~ | — | Removida (orientador) |
| ~~`fig4-testcomp2026`~~ | ~~Gráfico de pizza TestComp~~ | — | Removida (orientador: nunca usar pizza) |
| ~~`fig5-cicd`~~ | ~~Workflow CI/CD~~ | — | Removida (substituída por parágrafo + OpenSSF) |
| `fig6-wasm-pipeline` | Pipeline WASM: .wasm → wasm2c → LLVM IR → Passes → KLEE | Sec 4 | 🟡 .mmd criado, aguardando renderização para PNG |

---

## Core Frameworks & Mental Models

### Pipeline de Verificação Híbrida
**Use fuzzing (LibFuzzer) como primeira linha** para bugs superficiais — rápido mas cego para branches complexos. **Use execução simbólica (KLEE) como solver de guardas** quando o fuzzer estagna — preciso mas caro. A combinação (iterative deepening) resolve o trade-off.

### Pass Migration Pattern (Legacy PM → New PM)
**Sempre herdar de `PassInfoMixin<NomeDoPass>`** e implementar `run(Function &, FunctionAnalysisManager &)`. **`isRequired() = true` é obrigatório** — sem isso, `opt -O0` pula o pass silenciosamente. Este foi o bug mais crítico da migração (3 bugs escaparam dos unit tests).

### Mapeamento Propriedade SV-COMP → CWE → Pass
**Cada propriedade de memory safety do SV-COMP mapeia para um CWE** do MITRE:
- `valid-free` → **CWE-415** (Double-Free) / **CWE-416** (Use-After-Free)
- `valid-deref` → **CWE-416** (Use-After-Free)
- `valid-memsafety` → **CWE-119/787** (Buffer Overflow)
- `valid-memcleanup` → **CWE-401** (Memory Leak)

O mapeamento torna resultados de verificação formal acionáveis para times de AppSec.

### Modernização de Engenharia como Pré-requisito
**CI/CD + Docker + static analysis + sanitizers não são luxo, são fundamento.** Sem eles: bugs de regressão passam despercebidos, resultados não são reproduzíveis, a ferramenta não compete em SV-COMP/TestComp.

### Docker Reproducibility Contract
**Se funciona na imagem `map2check-dev` (Ubuntu 22.04 + LLVM 16 + KLEE 3.1), funciona em qualquer lugar.** O Dockerfile.dev é o contrato de reprodutibilidade — documenta cada dependência e versão.

### TestComp 2026 como Validação Empírica
**Resultados Heap (594 tasks):** TRUE 264 (44.4%), UNKNOWN 271 (45.6%), FALSE 56 bugs reais (9.4%), TIMEOUT 2 (0.3%). **Score 57.** UNKNOWN 45.6% é o principal gargalo e direciona o próximo ciclo de desenvolvimento.

### UNKNOWN como KPI de Engenharia
**Não é fracasso, é sinal de onde investir.** Causas: timeout 300s insuficiente, path exploration do KLEE não otimizada, solver SMT (Z3) sem tuning. Cada ponto percentual reduzido de UNKNOWN é ganho real de cobertura.

### Testes Unitários são Necessários mas Insuficientes
**7/7 unit tests passando e 3 bugs críticos escaparam.** Os bugs (KLEE flags, isRequired, target function) só foram detectados por smoke tests manuais. **Testes de integração E2E (compile → instrument → link → execute) no CI são indispensáveis.** No WASM, 8 commits consecutivos de "fix(ci)" tentaram fazer o CI passar sem executar KLEE; o job `e2e-wasm` baseado em Docker foi criado justamente para quebrar esse ciclo.

### WASM como Próximo Vetor de Growth
**Pipeline LLVM 16 estendido para WebAssembly.** Memory safety em runtimes WASM (Wasmtime, Wasmer) é uma nova classe de bugs. Feature implementada e funcional — NÃO é trabalho futuro, aparece no paper com resultados da Juliet.

### Per-Allocation Bounds Checking via dlmalloc Interception
**O wasm2c gera funções de alocação com nome determinístico** (w2c_*_dlmalloc / w2c_*_dlfree). O MemoryTrackPass as intercepta por padrão de nome (`StringRef::contains`), registrando offset + size no AllocationLog existente. Cada load/store na linear memory é verificado contra as alocações registradas via `is_valid_allocation_address`. Funciona para heap (malloc-based: 3/3 Juliet CWE-122/126 FALSE), não para stack/globals (CWE-121/124/127: 12/12 UNKNOWN).

### Entrypoint Translation Pattern
**O wasm2c traduz `_start` → `w2c_*_start` e `main` → `w2c_*_original_main`.** Para unificar com o pipeline Map2Check (que espera `main`), geramos um wrapper C dinâmico (`generateWasmWrapperStatic`) que extrai o nome do módulo do entrypoint levantado, instancia o módulo WASM via `wasm2c_*_instantiate`, invoca `_start`, e libera via `wasm2c_*_free`. O wrapper é compilado para .bc e linkado com o IR levantado via `llvm-link`.

### Linear Memory Bounds Gap
**O bounds check nativo do wasm2c opera no nível da memória linear** (0 a `mem->size`), não por buffer individual. Um `strcpy` que ultrapassa o buffer escreve no próximo byte da stack, ainda dentro dos 64KB alocados. Para detectar per-buffer overflow, é necessário rastrear limites de cada alocação — viável via dlmalloc para heap, inviável para stack/globais (Fase 2, pós-SBSeg).

### Escopo do Paper — O que NÃO mencionar
- **NÃO**: DG Library, AFL++, Coordenador, Smart Seeds (fora do escopo — são próximos passos do pré-projeto Map2Check 2.0)
- **SIM**: LLVM 16, New PM, 9 passes, C++17, CI/CD, Docker, sanitizers, static analysis, TestComp 2026, CWE mapping, WASM

---

## Chapter Index

| # | Title | Pages | Figures | Data Sources | Key Frameworks |
|---|-------|-------|---------|--------------|----------------|
| [ch01](chapters/ch01-introducao-motivacao.md) | Introdução | ~1 | — | `docs/paper-sbseg-2026/sbseg-article/main.tex` | Memory Safety, Verificação Híbrida, CWE Top 25, Trabalhos Relacionados |
| [ch02](chapters/ch02-historia-evolucao.md) | História e Evolução | ~1 | fig1-timeline | `CHANGELOG.md`, `docs/migration/` | BMC→KLEE→Híbrido, Comparação com concorrentes (CPAchecker, Symbiotic, FuSeBMC, ESBMC) |
| [ch03](chapters/ch03-arquitetura.md) | Arquitetura e Funcionalidades | ~2.5 | fig2-pipeline | `docs/migration/1.3-*.md`, `.github/workflows/`, `Dockerfile.dev` | Pipeline 5 estágios, Técnicas (fluxo de dados, intervalos), OpenSSF |
| [ch04](chapters/ch04-ciberseguranca.md) | Análise Experimental | ~2 | — | `test-comp2026/simulation/` | CWE mapping, TestComp 2026 resultados, WASM lifting |
| ~~[ch05]~~ | ~~Demonstração Planejada~~ | — | — | — | Removida do artigo (orientador: demo não é conteúdo de paper) |
| [ch06](chapters/ch06-conclusao.md) | Conclusão | ~1 | — | `docs/paper-sbseg-2026/data/consolidated-data.md` | Resultados, Limitações (UNKNOWN 45.6%), Perspectivas (WASM, E2E tests) |

## Topic Index

- **AFL++** → ch01, Pré-projeto
- **Arquitetura (pipeline)** → ch03
- **ASAN / UBSAN / TSAN** → ch03, ch04
- **BMC (Bounded Model Checking)** → ch02
- **Buffer Overflow** → ch01, ch04
- **C++17** → ch03
- **CBMC** → ch02
- **Checkpoint automatizado** → ch04, patterns
- **CI/CD** → ch03
- **Crab-LLVM** → ch02
- **CWE-119 / 787** → ch01, ch04
- **CWE-401 (Memory Leak)** → ch01, ch04
- **CWE-415 (Double-Free)** → ch01, ch04
- **CWE-416 (Use-After-Free)** → ch01, ch04
- **Demonstração ao vivo** → ch05
- **DG Library** → Pré-projeto (fora do escopo paper)
- **Docker** → ch03, ch05
- **Execução Simbólica** → ch01, ch02, ch03
- **FuSeBMC** → ch01
- **isRequired fix** → ch03, ch04
- **Iterative deepening** → ch03, patterns
- **KLEE 3.1** → ch02, ch03, ch04
- **Legacy PM → New PM** → ch02, ch03, patterns
- **LibFuzzer** → ch02, ch03
- **LLVM 16** → ch03
- **Memory Safety** → ch01, ch04
- **New Pass Manager** → ch03, patterns
- **OpenSSF Best Practices** → ch03
- **Passes (9)** → ch03
- **Pipeline (5 estágios)** → ch03
- **SBSeg SF 2026** → ch05
- **Smart Seeds** → ch01, Pré-projeto
- **SMT Solver (Z3)** → ch01, ch03
- **SV-COMP** → ch01
- **Symbiotic** → ch01
- **TestComp 2026** → ch04, ch06
- **UNKNOWN (45.6%)** → ch04, ch06
- **WASM (WebAssembly)** → ch04, ch06
- **WASM lifter / WABT** → ch04, Implementation Plan
- **wasm2c** → ch04, Implementation Plan
- **wasi-sdk** → Implementation Plan
- **w2c__start** → Implementation Plan
- **dlmalloc interception** → ch04, patterns
- **entrypoint translation** → ch04, patterns
- **per-allocation bounds** → ch04, patterns
- **linear memory gap** → ch04, patterns
- **Juliet Test Suite** → ch04
- **generateWasmWrapperStatic** → ch04
- **WasmRuntimeStubs** → ch04
- **integration tests (WASM)** → tests/
- **Witness GraphML** → ch03

## Supporting Files

- [glossary.md](glossary.md) — all key terms with definitions
- [patterns.md](patterns.md) — all techniques and design patterns
- [cheatsheet.md](cheatsheet.md) — quick reference tables and decision guides

---

## Scope & Limits

**Coberto**: Conteúdo do paper SBSeg 2026 SF (Map2Check modernization) + pré-projeto Map2Check 2.0 + metadados de submissão + backlog + dados consolidados de TestComp 2026.

**Fora do escopo do paper** (mencionados em contexto, detalhados no pré-projeto): DG Library, AFL++, Smart Seeds, Coordenador.

**Fontes de referência no repo**: `docs/migration/` (passes, frontend, checkpoints), `.github/workflows/` (CI/CD), `test-comp2026/simulation/` (benchmarks), `Dockerfile.dev` (reprodutibilidade).

**Esta skill absorveu e substitui `sbseg-paper`** — todo o conteúdo de projeto (submissão, backlog, restrições, CTA) foi fundido aqui.
