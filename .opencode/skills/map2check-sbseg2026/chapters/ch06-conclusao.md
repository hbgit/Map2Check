# Chapter 6: Conclusão

## Core Idea
O Map2Check 2026 é um feito de engenharia — 35 commits, 84 arquivos, +5.785/-657 linhas para modernizar a stack completa. A validação no TestComp 2026 (score 57) comprova viabilidade competitiva. Os próximos passos são reduzir UNKNOWN (45.6%), adicionar testes E2E no CI e completar o suporte WASM.

## Resumo dos Resultados

| Métrica | Valor |
|:--------|:------|
| Commits | 35 |
| Arquivos modificados | 84 |
| Linhas adicionadas/removidas | +5.785 / -657 |
| Passes migrados (Legacy PM → New PM) | 9/9 |
| Unit tests | 7/7 |
| Pass plugin load tests | 9/9 |
| TestComp 2026 Heap | 594 tasks, score 57 |
| Bugs reais detectados (FALSE) | 56 (9.4%) |

## Frameworks Introduzidos
- **Avaliação de Ferramentas de Verificação em 3 Dimensões**:
  1. Corretude: TRUE/FALSE acurácia — 56 bugs reais detectados
  2. Completude: % UNKNOWN — 45.6% inconclusivos
  3. Qualidade de engenharia: CI/CD, Docker, sanitizers, OpenSSF, reprodutibilidade
  - When to use: autoavaliação contínua e planejamento de melhorias
  - How: medir cada dimensão separadamente, priorizar gargalos

## Key Concepts
- **UNKNOWN (45.6%)**: maior gargalo — timeout de 300s insuficiente, path exploration não otimizada, solver Z3 sem tuning
- **Testes E2E ausentes**: 3 inconsistências NPM escaparam dos unit tests — pipeline completo (compile → instrument → link → execute) precisa ser automatizado no CI
- **WASM**: pipeline de lifting funcional (wasm2c 1.0.41 + wasi-sdk 33.0), passes em adaptação — feature em desenvolvimento ativo
- **OpenSSF Best Practices**: certificação em processo — atende critérios de disponibilidade, funcionalidade, reprodutibilidade, sustentabilidade

## Mental Models
- Use **UNKNOWN como KPI de engenharia**: 45.6% direciona o próximo ciclo de desenvolvimento
- Pense em **modernização como pré-requisito, não como fim**: LLVM 16, CI/CD, Docker são a base para avanços algorítmicos futuros
- Use **TestComp como validação externa**: resultados reproduzíveis por terceiros via Docker + BenchExec
- Trate **WASM como crescimento orgânico do pipeline**: mesmo IR, mesmos passes, novo formato de entrada

## Limitações

1. **UNKNOWN alto (45.6%)**: timeout tuning, estratégias de solver SMT, heurísticas de path exploration
2. **Testes E2E ausentes no CI**: pipeline completo não automatizado — 3 bugs escaparam dos unit tests
3. **Sem baseline comparativo no TestComp**: resultados reportados sem comparação direta com concorrentes na mesma categoria
4. **WASM em desenvolvimento**: pipeline de lifting validado, mas passes ainda não adaptados para modelo de memória linear

## Perspectivas

1. **WASM**: completar adaptação dos passes para memória linear, benchmarks Juliet CWE-119/416 compilados para WASM
2. **Redução de UNKNOWN**: tuning de timeout, portfólio de solvers SMT, slicing estático (DG Library)
3. **Testes E2E no CI**: automatizar compile → instrument → link → execute com verificação de vereditos
4. **NÃO no escopo deste paper**: DG Library, AFL++, Smart Seeds, Coordenador — são trabalhos futuros do pré-projeto Map2Check 2.0

## Key Takeaways
1. Modernização de engenharia é pré-requisito para qualquer avanço — sem LLVM 16, CI/CD, Docker não há competição possível
2. TestComp 2026 score 57 é prova de viabilidade, não de excelência — há espaço significativo para melhoria
3. 45.6% UNKNOWN é a métrica-alvo para o próximo ciclo de desenvolvimento
4. WASM é a próxima fronteira de memory safety — pipeline de lifting funcional, passes em adaptação
5. Projeto open source, código público, infraestrutura de artefato validada — pronto para contribuições da comunidade

## Connects To
- **Ch 4**: Resultados TestComp 2026 e CWE mapping — fundamento empírico para as conclusões
- **Ch 3**: Arquitetura — base de engenharia que viabilizou os resultados
- **Pré-projeto Map2Check 2.0**: próximos passos (DG, AFL++, Smart Seeds) — fora do escopo do paper SBSeg
- **WASM Implementation Plan**: `docs/paper-sbseg-2026/wasm-implementation-plan.md`
