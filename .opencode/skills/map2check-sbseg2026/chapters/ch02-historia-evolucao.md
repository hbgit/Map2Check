# Chapter 2: História e Evolução

## Core Idea
O Map2Check passou por 3 fases arquiteturais (BMC → KLEE → Híbrido) em 10 anos, mas perdeu competitividade por falta de features que os concorrentes adotaram em cada checkpoint: CPAchecker já usava múltiplas estratégias em 2016, Symbiotic integrava slicing com KLEE em 2018, e FuSeBMC introduziu smart seeds com orquestração dinâmica em 2020.

## Frameworks Introduzidos
- **BMC (2016, TACAS)**: Map2Check usava CBMC como único solver BMC para "Hunting Memory Bugs"
  - Concorrente: CPAchecker já combinava análise de valor, predicate abstraction e BMC com múltiplas estratégias
- **Execução Simbólica (2018, TACAS)**: migração para LLVM + KLEE, path-based symbolic execution
  - Concorrente: Symbiotic integrava program slicing estático (DG) com KLEE — otimização ausente no Map2Check
  - Concorrente: ESBMC usava portfólio de solvers SMT (Z3, Boolector, Yices) para reduzir UNKNOWN
- **Híbrido (2020, TACAS)**: fuzzing (LibFuzzer) + execução simbólica (KLEE) + invariantes indutivos (Crab-LLVM) em iterative deepening
  - Concorrente: FuSeBMC introduzia smart seeds com orquestração BMC→fuzzer, atingindo >81% cobertura — Map2Check operava motores em fases estanques

## Key Concepts
- **CPAchecker**: framework de verificação configurável com múltiplas estratégias — referência desde SV-COMP 2013
- **Iterative Deepening**: estratégia de aprofundamento progressivo alternando fuzzing e execução simbólica
- **Crab-LLVM**: análise estática baseada em interpretação abstrata para geração de invariantes indutivos — `crabllvm2021`
- **Portfólio de solvers SMT**: uso combinado de Z3, Boolector, Yices para maximizar a taxa de resultados conclusivos
- **Estagnação (2019-2025)**: LLVM 6.0 EOL (2019), Ubuntu 16.04 EOL (2021), KLEE 2.0 fork perdeu compatibilidade com Z3

## Mental Models
- Pense na **evolução como débito técnico acumulado**: sem manutenção contínua, 7 anos de stack parada exigem migração completa (35 commits, 84 arquivos, +5.785/-657 linhas)
- Use **comparação com concorrentes como diagnóstico**: cada checkpoint revela a feature ausente (multi-estratégia em 2016, slicing em 2018, orquestração em 2020)
- Pense no **renascimento como pré-requisito de engenharia**: a base moderna (LLVM 16, CI/CD, Docker) é condição necessária para qualquer avanço algorítmico futuro

## Anti-patterns
- **Publicar e abandonar**: 3 artigos TACAS (2016, 2018, 2020) sem manutenção contínua → estagnação de 7 anos
- **Depender de fork customizado**: KLEE 2.0 fork perdeu compatibilidade com Z3 e bibliotecas de runtime — usar upstream sempre que possível
- **Motores sem orquestração dinâmica**: fases estanques perdem eficiência vs. smart seeds com realimentação em tempo real

## Key Takeaways
1. 2016: BMC com CBMC — CPAchecker já usava múltiplas estratégias, Map2Check dependia de 1 solver
2. 2018: KLEE + LLVM — Symbiotic integrava slicing, ESBMC usava portfólio de solvers
3. 2020: Fuzzing + KLEE + Crab-LLVM — FuSeBMC introduzia smart seeds com >81% cobertura
4. 2019-2025: Estagnação total — LLVM 6 EOL, Ubuntu 16.04 EOL, sem CI/CD
5. 2026: Renascimento — 35 commits, 84 arquivos, modernização completa
6. A ausência de slicing, orquestração dinâmica e portfólio de solvers são débitos técnicos identificados

## Connects To
- **Ch 1**: Introdução — contexto dos concorrentes e motivação para o renascimento
- **Ch 3**: Arquitetura — o pipeline modernizado que endereça a estagnação técnica
- **Ch 4**: Análise Experimental — resultados que validam a viabilidade competitiva após o renascimento
- **Pré-projeto Map2Check 2.0**: próximos passos (DG Library, AFL++, Smart Seeds) — features que os concorrentes já tinham
