# Map2Check 2026 — Artigo SBSeg SF

**Documento-mestre de planejamento e progresso.**
Atualizado a cada iteração. Estado atual: Sprint 0 — Fundação.

---

## Metadados da Submissão

| Campo | Valor |
|:------|:------|
| **Conferência** | SBSeg 2026 — Salão de Ferramentas (SF) |
| **Deadline submissão** | 20 de julho de 2026 |
| **Notificação** | 03 de agosto de 2026 |
| **Camera ready** | 14 de agosto de 2026 |
| **Evento** | 01–04 de setembro de 2026, Armação dos Búzios — RJ |
| **Template** | SBC (a ser obtido pelo usuário) |
| **Limite** | 8 páginas de corpo (figuras e tabelas incluídas) + até 2 de referências/apêndices |
| **Modo** | Single-blind (autores conhecidos pelos revisores) |
| **Idioma** | Português |
| **CTA** | Avaliação de artefato integrada ao CTA — submissão APÓS notificação (até 14/ago) |
| **Vídeo obrigatório** | Sim — link de instalação/uso |
| **Demonstração ao vivo** | Obrigatória — pelo menos 1 autor inscrito no evento |
| **Sistema** | JEMS3 |
| **IA Generativa** | Verificar Código de Conduta SBC — declarar uso se aplicável |

**Título provisório:** *Map2Check 2026: Modernização Sustentável de uma Ferramenta de Verificação de Memory Safety com LLVM 16 e Extensão para WebAssembly*

---

## Progresso por Seção

| # | Seção | Páginas | Status | Última atualização |
|:---|:---|:---|:---|:---|
| 1 | Introdução e Motivação | ~1 | 🔴 0% — não iniciado | — |
| 2 | História e Evolução | ~1 | 🔴 0% — não iniciado | — |
| 3 | Arquitetura e Funcionalidades | ~2.5 | 🔴 0% — não iniciado | — |
| 4 | Features de Cibersegurança | ~2 | 🔴 0% — não iniciado | — |
| 5 | WebAssembly (Lifting + Verificação) | ~1 | 🟡 10% — plano detalhado em `docs/migration/1.7-wasm-pipeline.md` | 2026-06-23 |
| 6 | Demonstração Planejada | ~0.5 | 🔴 0% — não iniciado | — |
| 7 | Avaliação, Perspectivas e Conclusão | ~1 | 🔴 0% — não iniciado | — |
| — | Referências | ~1 | 🟡 Em backlog — aguardando usuário | — |
| — | Apêndice de Reprodutibilidade | ~1 | 🔴 0% — não iniciado | — |

**Progresso geral:** ~8% (estrutura criada, plano WASM detalhado, conteúdo do artigo em rascunho)

---

## Progresso Específico — WebAssembly (Map2Check-WASM)

**Documento de referência:** [`docs/migration/1.7-wasm-pipeline.md`](../migration/1.7-wasm-pipeline.md)

| Sub-etapa | Descrição | Status | Sessão |
|:---|:---|:---|:---|
| 1.7.1 | Infraestrutura WABT no Docker | ✅ Concluído — 2026-06-27 | 1 |
| 1.7.2 | WasmLifter (frontend) | ✅ Concluído — 2026-06-27 | 1 |
| 1.7.3 | PoC end-to-end manual | 🟡 Em andamento — teste manual validado | 2 |
| 1.7.4 | MemoryTrackPass adaptado para WASM | 🔴 Não iniciado | 2 |
| 1.7.5 | CLI `--wasm` + WasmBackend | 🔴 Não iniciado | 3 |
| 1.7.6 | Benchmarks Juliet WASM | 🔴 Não iniciado | 3 |
| 1.7.7 | Documentação e integração ao artigo | 🔴 Não iniciado | 4 |
| 1.7.8–1.7.12 | Taint analysis, threat modeling IoT, otimização SMT | 🔴 Trabalho futuro (pós-SBSeg) | — |

---

## Checklist de Artefato CTA

| Critério | Estado | Evidência |
|:---|:---|:---|
| **Disponibilidade** | ✅ OK | Repositório público (`hbgit/Map2Check`) + Docker image GHCR |
| **Funcionalidade** | ✅ OK | 7/7 unit tests + 9/9 passes carregam no opt-16 |
| **Reprodutibilidade** | ✅ OK | `Dockerfile.dev` + `test-comp2026/simulation/` scripts |
| **Sustentabilidade** | ✅ OK | CI/CD (GitHub Actions), `.clang-tidy`, `.cppcheck-suppressions.txt`, sanitizers, docs |

---

## BACKLOG (dependências do usuário)

| # | Item | Bloqueia | Prioridade | Estado |
|:---|:---|:---|:---|:---|
| 1 | **Referências BibTeX** (TACAS 2016, 2018, 2020 + outros) | Seção 2 (História) e Referências | 🔴 Alta | 🟡 Aguardando usuário |
| 2 | **Idioma** | Português | 🟢 OK | ✅ Decidido |
| 3 | **Template LaTeX SBC** | Geração do PDF final | 🔴 Alta | 🟡 Aguardando usuário |
| 4 | **Aprovação do orientador** no outline | Sprint 1 (redação das secções 1–3) | 🔴 Alta | 🟡 Aguardando usuário |
| 5 | **Gravação do vídeo técnico** (YouTube) | Seção 5 (Demonstração) | 🟡 Média | 🟡 Aguardando usuário |
| 6 | **Verificação Código de Conduta SBC** (IA generativa) | Declaração no artigo | 🟡 Média | 🟡 Aguardando usuário |
| 7 | **Presença no evento** (Búzios, 01–04/set) | Condição de aceitação | 🟢 OK | ✅ Confirmada |

---

## Sprint 0 — Fundação (17–22/jun, ~5 dias)

| Dia | Tarefa | Status |
|:---|:---|:---|
| 0 | Criar skill de persistência + diretório do artigo | ✅ Feito |
| 0 | Gerar figuras Mermaid (5 figuras) | ✅ Feito |
| 0 | Escrever PLAN.md + outline.md | ✅ Feito |
| 0 | Preparar roteiro do vídeo | ✅ Feito |
| 0 | Consolidar dados brutos em `data/` | ✅ Feito |
| 1 | Definir plano de implementação WASM (`docs/migration/1.7-wasm-pipeline.md`) | ✅ Feito |
| 1–2 | Implementar infraestrutura WABT no Docker + WasmLifter (1.7.1–1.7.2) | 🟡 Em andamento — sessão 1 |
| 2–3 | PoC end-to-end + adaptar MemoryTrackPass WASM (1.7.3–1.7.4) | 🔴 Não iniciado — sessão 2 |
| 3–4 | CLI `--wasm` + WasmBackend + benchmarks Juliet (1.7.5–1.7.6) | 🔴 Não iniciado — sessão 3 |
| 4 | Integrar resultados WASM ao outline + revisar com orientador | 🟡 Pendente |

**Entregável Sprint 0:** Outline aprovado + ambiente de build reprodutível validado + plano WASM detalhado.

---

## Sprint 1 — Núcleo Técnico e História (23/jun–02/jul, ~10 dias)

| Dia | Tarefa | Seções | Status |
|:---|:---|:---|:---|
| 1–2 | Redigir Introdução e Motivação (rascunho) | Sec 1 | 🔴 Não iniciado |
| 3–4 | Redigir História e Evolução (rascunho) | Sec 2 | 🔴 Não iniciado |
| 5–8 | Redigir Arquitetura e Funcionalidades (rascunho) | Sec 3 | 🔴 Não iniciado |
| 9–10 | Produzir figuras finais + tabular resultados | — | 🔴 Não iniciado |

**Entregável Sprint 1:** Seções 1–3 em draft + figuras + tabela de resultados consolidada.

---

## Sprint 2 — Cibersegurança + WASM + Demo + Vídeo (03–13/jul, ~10 dias)

| Dia | Tarefa | Seções | Status |
|:---|:---|:---|:---|
| 1–2 | Redigir Features de Cibersegurança | Sec 4 | 🔴 Não iniciado |
| 3–5 | Implementar pipeline WASM (sessões 2–3 de `1.7-wasm-pipeline.md`) | Sec 5 | 🟡 Em andamento — aguardando sessões |
| 5–6 | Redigir seção WebAssembly + resultados Juliet | Sec 5 | 🔴 Não iniciado |
| 7 | Redigir Demonstração Planejada | Sec 6 | 🔴 Não iniciado |
| 8 | Redigir Avaliação, Perspectivas e Conclusão | Sec 7 | 🔴 Não iniciado |
| 9–10 | Gravar e editar vídeo técnico | — | 🔴 Não iniciado |
| 10 | Escrever Apêndice de Reprodutibilidade | Apêndice | 🔴 Não iniciado |

**Entregável Sprint 2:** Artigo completo em draft + pipeline WASM funcional + vídeo publicado + checklist CTA.

---

## Sprint 3 — Revisão, Finalização e Submissão (14–20/jul, ~7 dias)

| Dia | Tarefa | Status |
|:---|:---|:---|
| 1–2 | Revisão técnica e de escrita (todas as secções) | 🔴 Não iniciado |
| 3 | Checagem do limite de 8 páginas de corpo + formatação SBC | 🔴 Não iniciado |
| 4 | Declaração de uso de IA generativa (se aplicável) conforme Código de Conduta SBC | 🔴 Não iniciado |
| 5 | Revisão final + leitura por terceiro | 🔴 Não iniciado |
| 6–7 | Geração do PDF final e submissão no JEMS3 | 🔴 Não iniciado |

**Entregável Sprint 3:** PDF submetido.

> **Nota sobre CTA:** A avaliação de artefato é feita APÓS a notificação de aceite (03/ago). O artefato (container Docker, scripts, documentação) será refinado e submetido junto com o camera ready (14/ago). Não precisa estar 100% finalizado na submissão do artigo.

> **Nota sobre demonstração ao vivo:** Pelo menos 1 autor deve estar inscrito no evento (01–04/set, Búzios) para demonstrar a ferramenta. Esta é uma condição de aceitação.

---

## Notas de Iteração

### Iteração 1 — 2026-06-20
- **Todas as 7 seções redigidas** em português (Introdução, História, Arquitetura, Cibersegurança, WebAssembly, Demonstração, Conclusão + Apêndice).
- **Macros LaTeX** criadas para todos os dados numéricos (\TotalTasks, \ScoreTestComp, etc.).
- **Tabelas** inseridas: passes de instrumentação, mapeamento CWE, resultados TestComp 2026, bugs críticos.
- **Figuras** referenciadas: timeline, pipeline, passes, TestComp 2026, CI/CD.
- **Referências** ainda com placeholders `~\cite{}` — aguardando usuário inserir as chaves BibTeX corretas.
- **PDF não compilado** — ambiente sem pdflatex. Compilação a ser feita pelo usuário.
- BACKLOG: referências BibTeX + figuras PNG + compilação PDF + gravação vídeo.

### Iteração 2 — 2026-06-23
- **Plano WASM detalhado** criado em `docs/migration/1.7-wasm-pipeline.md` com 4 sessões de implementação.
- **Migration plan atualizado** (`docs/map2check_migration_plan.md`) com Fase 1.7, riscos e dependências.
- **PLAN.md do SBSeg atualizado** com nova seção WebAssembly (Sec 5) e título provisório revisado.
- **Status das sub-etapas WASM:**
  - 1.7.1–1.7.2: 🟡 Em andamento (sessão 1 — infra + lifter)
  - 1.7.3–1.7.4: 🔴 Não iniciado (sessão 2 — PoC + passes)
  - 1.7.5–1.7.6: 🔴 Não iniciado (sessão 3 — CLI + benchmarks)
  - 1.7.7: 🔴 Não iniciado (sessão 4 — artigo)
  - 1.7.8–1.7.12: 🔴 Trabalho futuro (pós-SBSeg)

### Iteração 3 — 2026-06-27
- **Sessão 1 WASM concluída** (infra + WasmLifter):
  - Dockerfile.dev atualizado com WABT 1.0.27, wasi-sdk-20.0, lld-16
  - Pipeline validado: `C → WASM (clang-16 --target=wasm32-unknown-unknown) → C (wasm2c) → LLVM IR (clang-16)`
  - Módulo `WasmLifter` implementado (`wasm_lifter.hpp` + `wasm_lifter.cpp`) e compilando
  - Integrado ao `CMakeLists.txt` do frontend
  - Descoberta técnica: wasm32-wasi gera opcodes não suportados pelo wasm2c 1.0.27; usar `wasm32-unknown-unknown -nostdlib`
- **TestComp ControlFlow coverage-error-call concluído**: 138 tasks, 38 FALSE, 30 TRUE, score 38/138
- **Artigo ainda pendente** — Trilha A (atualização com dados TestComp) não iniciada

---

*Última atualização: 2026-06-27*
