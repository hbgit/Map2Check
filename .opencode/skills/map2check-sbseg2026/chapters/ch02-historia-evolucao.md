# Chapter 2: História e Evolução

## Core Idea
O Map2Check percorreu 10 anos de evolução arquitetural — de BMC monolítico com CBMC até uma plataforma híbrida moderna — com um período de estagnação (2019-2025) que demandou renascimento completo da stack para voltar a ser competitivo.

## Frameworks Introduced
- **BMC (Bounded Model Checking)**: verificação exaustiva até profundidade K de loops — usado na origem (2016, TACAS: "Hunting Memory Bugs")
  - When to use: programas com loops de profundidade limitada
  - How: desenrola o programa até bound K e verifica propriedades via SAT/SMT
- **Execução Simbólica com KLEE**: migração de CBMC → LLVM + KLEE (2018, TACAS)
  - When to use: quando se precisa de contra-exemplos precisos e cobertura de caminhos profundos
  - How: interpreta LLVM IR com valores simbólicos, resolve path constraints com Z3
- **Abordagem Híbrida (2020, TACAS)**: fuzzing (LibFuzzer) + execução simbólica (KLEE) + invariantes indutivos (Crab-LLVM)
  - When to use: programas complexos onde nem fuzzing puro nem simbólico puro bastam
  - How: fases alternadas entre exploração rápida e resolução de guardas lógicas

## Key Concepts
- **CBMC**: C Bounded Model Checker — verificador BMC original usado na primeira versão
- **LibFuzzer**: motor de fuzzing do LLVM, guiado por cobertura via SanitizerCoverage
- **Crab-LLVM**: análise estática baseada em interpretação abstrata para geração de invariantes indutivos
- **New Pass Manager**: API C++ moderna do LLVM (v13+), substitui Legacy Pass Manager — requer reescrita de todos os passes
- **Opaque Pointers**: simplificação do LLVM IR (v15+), remove tipos dos ponteiros, afeta instrumentação

## Mental Models
- Pense na **evolução como camadas sobrepostas**: cada nova técnica (BMC → KLEE → LibFuzzer → Crab-LLVM) expande o envelope de bugs detectáveis
- Use a **linha do tempo como diagnóstico**: 2019-2025 sem releases → obsolescência técnica é risco existencial para ferramentas acadêmicas
- Pense no **renascimento como investimento de engenharia**, não como nova pesquisa: 35 commits, 84 arquivos, +5.785/-657 linhas para modernizar o que já existia

## Anti-patterns
- **Publicar e abandonar**: 3 artigos no TACAS (2016, 2018, 2020) sem manutenção contínua do código leva à estagnação
- **Depender de versões EOL**: LLVM 6.0 e Ubuntu 16.04 tornaram-se bloqueadores para uso em competições modernas
- **Subestimar custo de migração**: migrar 9 passes do Legacy PM para New PM + atualizar KLEE, LibFuzzer, e runtime library é esforço de meses

## Key Takeaways
1. Linha do tempo: 2016 (BMC/CBMC) → 2018 (LLVM+KLEE) → 2020 (Híbrido) → 2019-2025 (Estagnação) → 2026 (Renascimento)
2. Sustentabilidade de engenharia (CI/CD, Docker, reproducibilidade) é tão importante quanto inovação algorítmica
3. CWEs fornecem linguagem comum entre a ferramenta acadêmica e a indústria de segurança
4. O alinhamento com cibersegurança foi motivação estratégica para o renascimento
5. KLEE 3.x (LLVM 16) é uma migração crítica — flags e APIs mudaram significativamente desde a v2.0

## Connects To
- **Ch 1**: Introdução — contexto dos problemas de memory safety que motivaram a ferramenta
- **Ch 3**: Arquitetura — detalhamento dos 9 passes migrados e pipeline atual
- **Ch 4**: Cibersegurança — como a modernização viabilizou mapeamento CWE e TestComp 2026
- **Pré-projeto Map2Check 2.0**: DG Library, AFL++, Smart Seeds, Coordenador como próximos passos (fora do escopo do paper SBSeg)
