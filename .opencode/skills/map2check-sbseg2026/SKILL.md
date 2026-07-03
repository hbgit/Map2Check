---
name: map2check-sbseg2026
description: "Contexto completo do artigo SBSeg 2026 SF — Map2Check 2026. Metadados de submissão, frameworks, capítulos, backlog, restrições de edital, dados consolidados e conhecimento técnico. Ative ao trabalhar nos docs do paper."
---

<!-- argument-hint: [topic, framework name, or chapter number] -->

# Map2Check 2.0 — SBSeg 2026 SF
**Autores**: Guilherme Lucas Pereira Bernardo (UFRN), Herbert Oliveira Rocha (orientador) | **Submissão**: 20/jul/2026 | **Sections**: 6 | **Single-blind** — nomes podem constar

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
| 4 | **WASM PoC E2E (1.7.3)** — detectar overflow em WASM via KLEE | **Alta** | Seção WASM do artigo | Dev |
| 5 | **WASM MemoryTrackPass (1.7.4)** — adaptar passes para IR wasm2c | **Alta** | PoC E2E | Dev |
| 6 | **WASM CLI --wasm (1.7.5)** — integrar WasmLifter ao map2check.cpp | **Alta** | 1.7.4 | Dev |
| 7 | **WASM Juliet benchmarks (1.7.6)** — compilar e tabular detecção | **Média** | 1.7.5 | Dev |
| 8 | **WASM seção artigo (1.7.7)** — escrever ~1 pág + figura pipeline | **Média** | 1.7.6 | Dev |
| 9 | Gravação do vídeo técnico | **Média** | Seção 5 (Demonstração) | usuário |
| 10 | Validação do build por terceiro (CTA) | **Baixa** | Checklist de artefato | usuário |
| 11 | Ajustes do orientador no artigo | **Alta** | Submissão | usuário |

## 🛡️ CTA Checklist (Avaliação de Artefato)

| Dimensão | Evidência |
|:---------|:----------|
| **Disponibilidade** | Repositório público + Docker image (GHCR) |
| **Funcionalidade** | Build passa + 7/7 unit tests + 9/9 passes carregando |
| **Reprodutibilidade** | Dockerfile.dev (Ubuntu 22.04) + scripts de benchmark |
| **Sustentabilidade** | CI/CD (GitHub Actions), static analysis (clang-tidy, cppcheck), sanitizers (ASAN/UBSAN/TSAN), docs atualizados |

## 📊 Consolidated Data

```
Stack:         LLVM 16.0 · KLEE 3.x · C++17 · Ubuntu 22.04 · Docker
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

WASM Pipeline:
  Branch:          feat-wasm-verification (47 files diff from feat-update)
  WABT:            1.0.41 (wasm2c)
  wasi-sdk:        33.0 (clang --target=wasm32-wasip1)
  WasmLifter:      ✅ modules/frontend/wasm_lifter.{hpp,cpp} compilando
  Pipeline script: ✅ test_wasm_pipeline.sh — 3 cenários C→WASM→LLVM IR validados
  Docker:          ✅ WABT + wasi-sdk no Dockerfile.dev
  CLI --wasm:      🔴 Não integrado
  Passes adaptados:🔴 Não iniciado (MemoryTrackPass p/ WASM, Map2CheckLibrary p/ w2c__start)
  PoC E2E:         🔴 Não testado (detectar overflow em WASM via KLEE)
  Juliet benchmarks:🔴 Não iniciado
  Seção no artigo: 🔴 Não iniciado (~1 pág, pipeline + resultados)
  Deadline MVP:    13/jul/2026
```

## 🖼️ Figure Inventory

| ID | Descrição | Seção |
|:---|:----------|:------|
| `fig1-timeline` | Timeline visual 2016→2026 | Sec 2 |
| `fig2-pipeline` | Diagrama do pipeline de verificação | Sec 3 |
| `fig3-passes` | Diagrama dos 9 passes com dependências | Sec 3 |
| `fig4-testcomp2026` | Gráfico de resultados TestComp 2026 | Sec 4 |
| `fig5-cicd` | Workflow CI/CD (GitHub Actions) | Sec 3 |
| `fig6-wasm-pipeline` | Pipeline WASM: .wasm → wasm2c → LLVM IR → Passes → KLEE | Sec 4 |

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
**Se funciona na imagem `map2check-dev` (Ubuntu 22.04 + LLVM 16 + KLEE 3.x), funciona em qualquer lugar.** O Dockerfile.dev é o contrato de reprodutibilidade — documenta cada dependência e versão.

### TestComp 2026 como Validação Empírica
**Resultados Heap (594 tasks):** TRUE 264 (44.4%), UNKNOWN 271 (45.6%), FALSE 56 bugs reais (9.4%), TIMEOUT 2 (0.3%). **Score 57.** UNKNOWN 45.6% é o principal gargalo e direciona o próximo ciclo de desenvolvimento.

### UNKNOWN como KPI de Engenharia
**Não é fracasso, é sinal de onde investir.** Causas: timeout 300s insuficiente, path exploration do KLEE não otimizada, solver SMT (Z3) sem tuning. Cada ponto percentual reduzido de UNKNOWN é ganho real de cobertura.

### Testes Unitários são Necessários mas Insuficientes
**7/7 unit tests passando e 3 bugs críticos escaparam.** Os bugs (KLEE flags, isRequired, target function) só foram detectados por smoke tests manuais. **Testes de integração E2E (compile → instrument → link → execute) no CI são indispensáveis.**

### WASM como Próximo Vetor de Growth
**Pipeline LLVM 16 estendido para WebAssembly.** Memory safety em runtimes WASM (Wasmtime, Wasmer) é uma nova classe de bugs. Feature em desenvolvimento ativo — NÃO é trabalho futuro, aparece no paper.

### Escopo do Paper — O que NÃO mencionar
- **NÃO**: DG Library, AFL++, Coordenador, Smart Seeds (fora do escopo — são próximos passos do pré-projeto Map2Check 2.0)
- **SIM**: LLVM 16, New PM, 9 passes, C++17, CI/CD, Docker, sanitizers, static analysis, TestComp 2026, CWE mapping, WASM

---

## Chapter Index

| # | Title | Pages | Figures | Data Sources | Key Frameworks |
|---|-------|-------|---------|--------------|----------------|
| [ch01](chapters/ch01-introducao-motivacao.md) | Introdução e Motivação | ~1 | — | `docs/pré-projeto -Map2Check 2.0...md` | Memory Safety, Verificação Híbrida, Smart Seeds, CWE mapping |
| [ch02](chapters/ch02-historia-evolucao.md) | História e Evolução | ~1 | fig1-timeline | `CHANGELOG.md`, `docs/migration/` | BMC → KLEE → Híbrido, Linha do tempo 2016-2026 |
| [ch03](chapters/ch03-arquitetura.md) | Arquitetura e Funcionalidades | ~2.5 | fig2-pipeline, fig3-passes, fig5-cicd | `docs/migration/1.3-*.md`, `docs/migration/1.4-*.md`, `docs/migration/1.5-*.md`, `.github/workflows/`, `Dockerfile.dev` | 9 Passes New PM, Pipeline 5 estágios, isRequired fix, CI/CD |
| [ch04](chapters/ch04-ciberseguranca.md) | Features de Cibersegurança | ~2 | fig4-testcomp2026 | `docs/migration/1.4-*.md`, `test-comp2026/simulation/` | CWE mapping, TestComp 2026, Smoke tests, 3 bugs críticos |
| [ch05](chapters/ch05-demonstracao.md) | Demonstração Planejada | ~0.5 | — | `test-comp2026/simulation/`, `Dockerfile.dev` | Pipeline E2E Docker, array-2.c, Vídeo técnico |
| [ch06](chapters/ch06-conclusao.md) | Avaliação, Perspectivas e Conclusão | ~1 | — | `docs/paper-sbseg-2026/data/consolidated-data.md` | Resultados, Limitações, WASM, Testes E2E |

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
- **KLEE 3.x** → ch02, ch03, ch04
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
