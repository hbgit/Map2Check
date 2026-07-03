# Chapter 4: Features de Cibersegurança

## Core Idea
O Map2Check 2026 estabelece mapeamento direto entre propriedades de memory safety do SV-COMP e CWEs do MITRE, validado empiricamente no TestComp 2026 (Heap, 594 tasks, score 57, 56 bugs reais). 3 bugs críticos de regressão foram descobertos e corrigidos durante a validação — nenhum detectável pelos unit tests existentes.

## Frameworks Introduzidos
- **Mapeamento Propriedade SV-COMP → CWE → Pass**: framework que conecta a saída do verificador com a taxonomia de vulnerabilidades da indústria
  - When to use: para comunicar resultados de verificação formal para times de AppSec
  - How: cada propriedade de verificação (ex: valid-free, valid-deref) é associada a um CWE e ao pass responsável pela detecção

- **Checkpoint Automatizado**: framework de validação E2E com scripts `run_checkpoint.sh` + `verify_verdicts.sh`
  - When to use: para garantir que correções não introduzem regressões silenciosas
  - How: executa pipeline completo em benchmarks canônicos e compara vereditos com baseline

## Mapeamento de Memory Safety para CWEs

| CWE | Descrição | Propriedade SV-COMP | Pass |
|:----|:----------|:--------------------|:-----|
| **CWE-401** | Memory Leak | `valid-memcleanup` | MemoryTrackPass + runtime |
| **CWE-416** | Use-After-Free | `valid-free` + `valid-deref` | MemoryTrackPass |
| **CWE-415** | Double-Free | `valid-free` | MemoryTrackPass |
| **CWE-119 / CWE-787** | Buffer Overflow | `valid-memsafety` | OverflowPass + bounds check |

## Key Concepts
- **TestComp 2026 Heap**: sub-categoria `C.coverage-error-call.Heap` — 594 tasks com timeout 300s, Z3 solver, target `reach_error`
- **Resultados consolidados**: TRUE 264 (44.4%), UNKNOWN 271 (45.6%), FALSE 56 (9.4%), TIMEOUT 2 (0.3%) — Score: 57
- **Heap set**: 428 tasks — 39 FALSE, 203 TRUE, 184 UNKNOWN, 2 TIMEOUT
- **LinkedLists set**: 166 tasks — 18 FALSE, 61 TRUE, 87 UNKNOWN, 0 TIMEOUT
- **Smoke test validado**: `array-1.c` → TRUE (VERIFICATION SUCCEEDED), `array-2.c` → FALSE + counter-example (`nondet_int → 31763`, linha 19, main)

## 3 Bugs Críticos Encontrados

| Bug | Sintoma | Causa | Fix |
|:----|:--------|:------|:----|
| KLEE 3.x flags | `Unknown command line argument` | Flags removidas entre KLEE 2.0 → 3.x | Atualizar `caller.cpp` com flags da API atual |
| `isRequired()` | Passes pulados silenciosamente | `optnone` + New PM: passes não executam se não forem `required` | `isRequired() = true` nos 9 passes |
| Target function | `__VERIFIER_error` em vez de `reach_error` | env var vs `cl::opt`: nome da função-alvo passado de forma inconsistente | `-function-name` na CLI do KLEE |

## Mental Models
- Use **UNKNOWN não como fracasso, mas como sinal**: 45.6% UNKNOWN indica onde investir esforço (timeout tuning, estratégias de solver, heurísticas de path exploration)
- Pense em **testes unitários como necessários mas insuficientes**: 7/7 unit tests passando e ainda assim 3 bugs críticos indetectados → testes de integração E2E são indispensáveis
- Use **WASM como vetor de crescimento**: estender o pipeline LLVM 16 para WebAssembly abre nova classe de bugs (memory safety em runtimes Wasmtime/Wasmer)

## Anti-patterns
- **Confiar apenas em unit tests**: os 3 bugs críticos de regressão não foram pegos pelos 7 testes unitários existentes
- **Usar property errada**: ControlFlow benchmarks usaram `no-overflow` em vez de reachability — resultados incomparáveis
- **Ignorar false positives**: sem validação de contra-exemplos (re-execução concreta), bugs reportados podem ser falsos positivos

## Key Takeaways
1. Mapeamento CWE é essencial para tornar verificadores formais acionáveis na indústria de AppSec
2. TestComp 2026 Heap: 594 tasks, score 57, 56 bugs reais encontrados (9.4% FALSE positivo)
3. 45.6% UNKNOWN é a principal limitação — requer tuning de timeout e estratégias de solver SMT
4. 3 bugs de regressão descobertos: KLEE flags, isRequired(), target function — todos indetectáveis por unit tests
5. WASM é feature em desenvolvimento ativo — NÃO é trabalho futuro, estará no paper
6. Testes de integração E2E no CI são o próximo passo crítico de engenharia

## Connects To
- **Ch 3**: Arquitetura — cada pass da seção 3.2 é responsável por um CWE específico
- **Ch 5**: Demonstração — smoke test `array-2.c` será mostrado ao vivo com counter-example
- **Ch 6**: Conclusão — limitações (UNKNOWN 45.6%) e perspectivas (WASM, E2E tests)
- **MITRE CWE**: taxonomia padrão da indústria para classificação de vulnerabilidades
