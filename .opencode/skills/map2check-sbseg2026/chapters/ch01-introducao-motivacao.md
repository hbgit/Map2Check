# Chapter 1: Introdução e Motivação

## Core Idea
Memory safety em C/C++ é uma das principais causas de CVEs exploráveis. A verificação híbrida — combinando análise estática e dinâmica — é a abordagem mais promissora para detectar esses bugs, e o Map2Check se posiciona como ferramenta consolidada nesse espaço, renascida em 2026 após 7 anos de estagnação tecnológica.

## Frameworks Introduced
- **Verificação Híbrida**: combinação de fuzzing (LibFuzzer) + execução simbólica (KLEE) + invariantes indutivos (Crab-LLVM)
  - When to use: quando se precisa de cobertura de código profunda sem explosão de caminhos
  - How: fases alternadas entre exploração rápida (fuzzer) e resolução de guardas complexas (simbólico)
- **Smart Seeds (FuSeBMC)**: injeção de sementes geradas por BMC no fuzzer para desbloquear caminhos
  - When to use: quando o fuzzer estagna em branches protegidos por "números mágicos"

## Key Concepts
- **Memory Safety**: ausência de bugs como use-after-free, double-free, buffer overflow, memory leak
- **CWE-119/CWE-787**: Buffer Overflow — acesso fora dos limites de buffer
- **CWE-416**: Use-After-Free — acesso a memória já liberada
- **CWE-415**: Double-Free — liberação dupla do mesmo ponteiro
- **CWE-401**: Memory Leak — vazamento de memória alocada
- **SV-COMP/TestComp**: competições internacionais de verificação de software que impulsionam o estado da arte
- **LLVM IR**: representação intermediária do LLVM, base para análise e instrumentação
- **Execução Simbólica**: técnica que analisa programas com valores simbólicos, construindo path constraints resolvidas por solvers SMT

## Mental Models
- Use **fuzzing** como primeira linha de ataque — rápido para bugs superficiais, mas cego para branches complexos
- Use **execução simbólica** como solver de guardas — lento mas preciso para desbloquear caminhos com magic numbers
- Pense na **modernização de engenharia** como pré-requisito para qualquer avanço científico — sem CI/CD e Docker, resultados não são reproduzíveis
- Pense em **CWE como linguagem comum** entre verificação formal e indústria de segurança — torna resultados acionáveis para times de AppSec

## Anti-patterns
- **Execução monolítica sem orquestração**: motores operando em fases distintas sem compartilhar aprendizado geram redundância e timeouts
- **Depender de stack legada**: LLVM 6.0 EOL, Ubuntu 16.04 EOL, KLEE 2.0 sem suporte torna a ferramenta incomparável em competições
- **Ignorar engenharia de software**: sem CI/CD, sanitizers, static analysis, bugs de regressão passam despercebidos nos unit tests

## Key Takeaways
1. Bugs de memory safety continuam entre as principais causas de CVEs exploráveis globalmente
2. Ferramentas de competição (SV-COMP, TestComp) são o benchmark de facto para verificadores
3. A estagnação desde 2019 (LLVM 6, Ubuntu 16.04) impedia o Map2Check de competir
4. As 4 contribuições da versão 2026: (1) LLVM 6→16 + New PM + C++17, (2) CI/CD + Docker + sanitizers, (3) TestComp 2026 Heap score 57, (4) mapeamento CWE
5. Symbiotic (slicing + KLEE) e FuSeBMC (smart seeds + BMC) definem o estado da arte atual
6. Abordagem híbrida resolve o trade-off entre velocidade do fuzzing e precisão da execução simbólica

## Connects To
- **Ch 2**: História e evolução — de BMC (CBMC) à hibridização completa
- **Ch 3**: Arquitetura — pipeline de verificação e passes de instrumentação
- **Ch 4**: Cibersegurança — mapeamento CWE e resultados TestComp 2026
- **CWEs**: linguagem padrão MITRE para classificação de vulnerabilidades
