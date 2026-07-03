# Chapter 6: Avaliação, Perspectivas e Conclusão

## Core Idea
O renascimento do Map2Check 2026 é um feito de engenharia, não de pesquisa: 35 commits, 84 arquivos, +5.785/-657 linhas para modernizar a stack completa (LLVM 6→16, New PM, C++17, CI/CD, Docker). Os resultados do TestComp 2026 (score 57) demonstram viabilidade competitiva, mas a taxa de 45.6% UNKNOWN e a ausência de testes E2E são as principais limitações a resolver.

## Resumo dos Resultados

| Métrica | Valor |
|:--------|:------|
| Commits | 35 |
| Arquivos modificados | 84 |
| Linhas adicionadas | +5.785 |
| Linhas removidas | -657 |
| Passes migrados | 9/9 |
| Unit tests | 7/7 |
| Passes carregando | 9/9 |
| TestComp 2026 Heap | 594 tasks |
| Score | 57 |
| Bugs reais (FALSE) | 56 (9.4%) |
| Bugs críticos corrigidos | 3 (KLEE flags, isRequired, target function) |

## Frameworks Introduzidos
- **Avaliação de Ferramentas de Verificação**: três dimensões — corretude (TRUE/FALSE), completude (% UNKNOWN), qualidade de engenharia (CI/CD, reprodutibilidade, artefato)
  - When to use: para autoavaliação contínua e planejamento de melhorias
  - How: medir cada dimensão separadamente, priorizar gargalos (hoje: UNKNOWN > CI/E2E tests > WASM)

## Key Concepts
- **UNKNOWN (45.6%)**: maior gargalo — causas prováveis: timeout de 300s insuficiente para alguns benchmarks, estratégia de busca do KLEE não otimizada para path exploration, solver SMT (Z3) sem tuning de heurísticas
- **Testes E2E ausentes**: 3 bugs críticos escaparam dos unit tests — pipeline completo (compile → instrument → link → execute) precisa ser automatizado no CI
- **C++17 dead store warnings**: warnings em headers da libc++ — documentado, não crítico, sem impacto funcional
- **WebAssembly (WASM)**: próximo vetor de crescimento — pipeline LLVM 16 estendido para módulos WASM, memory safety em runtimes (Wasmtime, Wasmer)

## Mental Models
- Use **UNKNOWN como KPI de engenharia**: 45.6% não é fracasso, é direcionamento de esforço — tuning de timeout, estratégias de solver, path exploration heuristics
- Pense em **CI/CD como teste de sanidade contínuo**: build automatizado + unit tests + static analysis + sanitizers + (futuro) E2E tests — cada push verifica a integridade da ferramenta
- Use **Docker como contrato de reprodutibilidade**: se funciona na imagem, funciona em qualquer lugar — elimina "funciona na minha máquina"

## Anti-patterns
- **Focar só em corretude e ignorar completude**: TRUE/FALSE precisos mas 45.6% UNKNOWN torna a ferramenta pouco útil na prática
- **Adiar testes E2E**: bugs de regressão que escapam de unit tests se acumulam e minam a confiança na ferramenta
- **Mencionar features fora do escopo no paper**: DG Library, AFL++, Coordenador, Smart Seeds são próximos passos mas NÃO fazem parte deste artigo — risco de rejeição por escopo inflado

## Limitações

1. **UNKNOWN alto (45.6%)**: investigar timeout tuning, estratégias de solver SMT, heurísticas de path exploration
2. **Testes E2E ausentes no CI**: pipeline completo precisa ser automatizado — todos os 3 bugs escaparam dos unit tests
3. **ControlFlow incomparável**: benchmarks usaram `no-overflow` em vez de reachability
4. **C++17 warnings**: dead store em libc++ headers (documentado, sem impacto funcional)

## Perspectivas (próximos passos)

1. **WASM**: expansão de benchmarks, modelagem formal da memória linear, testes em runtimes Wasmtime/Wasmer
2. **Redução de UNKNOWN**: tuning de timeout, estratégias de solver, heurísticas de path exploration
3. **Testes E2E no CI**: automatizar `compile → instrument → link → execute` com verificação de vereditos
4. **NÃO no escopo deste paper**: DG Library, AFL++, Coordenador, Smart Seeds — são trabalhos futuros

## Key Takeaways
1. Modernização de engenharia é pré-requisito, não luxo — sem LLVM 16 + CI/CD + Docker, não há competição possível
2. TestComp 2026 score 57 é prova de viabilidade, não de excelência — há espaço significativo para melhoria
3. 45.6% UNKNOWN é a métrica-alvo para o próximo ciclo de desenvolvimento
4. WASM é a próxima fronteira de memory safety — feature em desenvolvimento ativo, não trabalho futuro
5. Projeto open source, aberto a contribuições — repositório público no GitHub

## Connects To
- **Ch 4**: Resultados TestComp 2026 detalhados — fundamento empírico para as conclusões
- **Ch 3**: Arquitetura — base de engenharia que viabilizou os resultados
- **Pré-projeto Map2Check 2.0**: próximos passos arquiteturais (DG, AFL++, Smart Seeds) — fora do escopo do paper SBSeg
- **OpenSSF Best Practices**: checklist em andamento para badge de segurança
