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

**Título provisório:** *Map2Check 2026: Modernização Sustentável de uma Ferramenta de Verificação de Memory Safety com LLVM 16 e New Pass Manager*

---

## Progresso por Seção

| # | Seção | Páginas | Status | Última atualização |
|:---|:---|:---|:---|:---|
| 1 | Introdução e Motivação | ~1 | 🔴 0% — não iniciado | — |
| 2 | História e Evolução | ~1 | 🔴 0% — não iniciado | — |
| 3 | Arquitetura e Funcionalidades | ~2.5 | 🔴 0% — não iniciado | — |
| 4 | Features de Cibersegurança | ~2 | 🔴 0% — não iniciado | — |
| 5 | Demonstração Planejada | ~0.5 | 🔴 0% — não iniciado | — |
| 6 | Avaliação, Perspectivas e Conclusão | ~1 | 🔴 0% — não iniciado | — |
| — | Referências | ~1 | 🟡 Em backlog — aguardando usuário | — |
| — | Apêndice de Reprodutibilidade | ~1 | 🔴 0% — não iniciado | — |

**Progresso geral:** 0% (estrutura criada, conteúdo ainda não redigido)

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
| 5 | **Gravação do vídeo técnico** | Seção 5 (Demonstração) | 🟡 Média | 🟡 Aguardando usuário |
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
| 1 | Implementar WASM (pipeline LLVM-WASM) + testes SV-COMP | 🟡 Em andamento — após merge feat-update |
| 2–4 | Revisar outline com orientador + integrar resultados WASM | 🟡 Pendente |

**Entregável Sprint 0:** Outline aprovado + ambiente de build reprodutível validado.

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

## Sprint 2 — Cibersegurança + Demo + Vídeo (03–13/jul, ~10 dias)

| Dia | Tarefa | Seções | Status |
|:---|:---|:---|:---|
| 1–3 | Redigir Features de Cibersegurança | Sec 4 | 🔴 Não iniciado |
| 4–5 | Redigir Demonstração Planejada | Sec 5 | 🔴 Não iniciado |
| 6–7 | Redigir Avaliação, Perspectivas e Conclusão | Sec 6 | 🔴 Não iniciado |
| 8–9 | Gravar e editar vídeo técnico | — | 🔴 Não iniciado |
| 10 | Escrever Apêndice de Reprodutibilidade | Apêndice | 🔴 Não iniciado |

**Entregável Sprint 2:** Artigo completo em draft + vídeo publicado + checklist CTA.

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

### Iteração 0 — 2026-06-18
- Criada estrutura completa do artigo (diretórios, skill, figuras, outline, roteiro, dados).
- Revisão do edital oficial SBSeg 2026 SF (https://www.sbseg2026.uff.br/chamadas/sf/):
  - **Single-blind** (não precisa anonimizar) ✓
  - **Camera ready:** 14/ago/2026 (novo prazo) ✓
  - **CTA após notificação:** artefato refinado junto com camera ready ✓
  - **Demonstração ao vivo obrigatória:** 1 autor inscrito no evento (01–04/set, Búzios) ✅ CONFIRMADA
  - **Idioma:** Português ✅ DECIDIDO
  - **Uso de IA:** DECLARADO — ferramentas de IA usadas como assistentes de escrita/revisão
- Mudança de escopo: **WebAssembly não é mais apenas trabalho futuro** — será implementado na Sprint 0 (após merge feat-update → develop) com testes SV-COMP. Será apresentado como **feature em desenvolvimento ativo** na Seção 4.5.
- Ainda não redigido conteúdo textual das secções.
- BACKLOG: referências BibTeX + template SBC aguardando usuário.

---

*Última atualização: 2026-06-18*
