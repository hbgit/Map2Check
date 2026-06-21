# 2. Historia e Evolucao

**Estado:** Rascunho nao iniciado.  
**Fontes:** CHANGELOG.md, docs/migration/.  
**Figuras:** Figura 1 (timeline).

---

## 2.1 Linha do tempo (2016->2026)

[PARAGRAFO A REDIGIR]
- 2016 (TACAS): "Hunting Memory Bugs" — abordagem BMC com CBMC
- 2018 (TACAS): Migracao para LLVM + KLEE — execucao simbolica como motor principal
- 2020 (TACAS): Abordagem hibrida — fuzzing (LibFuzzer) + execucao simbolica (KLEE) + invariantes indutivos (Crab-LLVM)
- 2019-2025: Estagnacao — ultimo release, LLVM 6.0 obsoleto, dependencias deprecadas
- 2026: Renascimento — modernizacao sistematica da stack completa
- Figura 1: Timeline visual da evolucao

## 2.2 O que motivou o renascimento

[PARAGRAFO A REDIGIR]
- Obsolescencia tecnica: LLVM 6.0 EOL, Ubuntu 16.04 EOL, KLEE 2.0 sem suporte
- Impossibilidade de competir em SV-COMP/TestComp com stack legada
- Necessidade de sustentabilidade de engenharia (CI/CD, reproducibilidade, qualidade de artefato)
- Alinhamento com ciberseguranca: CWEs como linguagem comum entre verificacao formal e industria de seguranca

---

*Nota: Aguardando redacao na Sprint 1. BACKLOG: referencias BibTeX das publicacoes TACAS.*
