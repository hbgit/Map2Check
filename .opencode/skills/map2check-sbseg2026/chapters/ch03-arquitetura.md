# Chapter 3: Arquitetura e Funcionalidades

## Core Idea
O pipeline Map2Check modernizado: Código C → clang-16 → LLVM IR → opt (9 passes New PM) → llvm-link → runtime → KLEE/LibFuzzer → Veredito + Witness. A migração para New Pass Manager + C++17 + CI/CD estabelece a base de engenharia para qualquer avanço futuro.

## Frameworks Introduced
- **Pipeline de Verificação em 5 Estágios**:
  1. Compilação: `clang-16 -emit-llvm` → LLVM IR com debug info
  2. Instrumentação: `opt` carrega 9 passes New PM como plugins
  3. Linking: `llvm-link` une bitcode instrumentado + runtime library
  4. Análise: KLEE (execução simbólica) ou LibFuzzer (fuzzing)
  5. Veredito: TRUE/FALSE/UNKNOWN + Witness GraphML (GenerateAutomataTruePass)

- **New Pass Manager (PassInfoMixin)**: API moderna do LLVM para escrever passes de análise e transformação
  - When to use: sempre — Legacy PM está deprecado desde LLVM 13
  - How: herdar de `PassInfoMixin<MyPass>`, implementar `run(Function &F, FunctionAnalysisManager &AM)`

## 9 Passes Migrados

| Pass | Função | Dependência |
|:-----|:-------|:------------|
| **AssertPass** | Instrumenta asserts do verificador (`__VERIFIER_assert`) | — |
| **TargetPass** | Marca função-alvo (e.g. `reach_error`) | — |
| **LoopPredAssumePass** | Injeta predicados (`__VERIFIER_assume`) em laços | — |
| **Map2CheckLibrary** | Renomeia `main` → `__map2check_main__` | — |
| **NonDetPass** | Substitui `__VERIFIER_nondet_*` por chamadas simbólicas | — |
| **TrackBasicBlockPass** | Rastreia cobertura de blocos básicos | — |
| **OverflowPass** | Instrumenta operações aritméticas para detecção de overflow | — |
| **GenerateAutomataTruePass** | Gera autômato de witness (GraphML) | TrackBasicBlockPass |
| **MemoryTrackPass** | Instrumenta acessos a memória (alloc, free, deref) | Map2CheckLibrary |

## Key Concepts
- **`isRequired()` fix crítico**: sem `isRequired() = true`, `opt -O0` pula todos os passes silenciosamente — passes devem declarar que são sempre requeridos, independente do nível de otimização
- **Witness GraphML**: formato padronizado SV-COMP para contra-exemplos — descreve caminho do erro como autômato
- **Runtime Library**: biblioteca linkada ao bitcode instrumentado que implementa checks de runtime (bounds, alloc/free tracking)
- **Iterative Deepening**: combinação KLEE + LibFuzzer em fases — fuzzer explora rápido, KLEE resolve branches bloqueados
- **Sanitizers**: ASAN (AddressSanitizer), UBSAN (UndefinedBehavior), TSAN (ThreadSanitizer) — compilação com sanitizers para detectar bugs na própria ferramenta

## Anti-patterns
- **Passes sem `isRequired()`**: o New PM não executa passes que não se declaram required quando o nível de otimização é O0 — bug silencioso crítico
- **Misturar Legacy PM com New PM**: APIs incompatíveis — qualquer `ModulePass` legado quebra o pipeline
- **Hardcodar flags do KLEE**: flags removidas entre versões (ex: KLEE 2.0 → 3.x) causam falhas silenciosas se não forem verificadas

## Worked Example
**Exemplo de pipeline E2E (smoke test):**

```bash
# 1. Compilar C → LLVM IR
clang-16 -emit-llvm -g -c loops/array-2.c -o array-2.bc

# 2. Instrumentar com 9 passes
opt -load-pass-plugin=build/lib/libAssertPass.so \
    -load-pass-plugin=build/lib/libTargetPass.so \
    ... \
    -passes="assert,target,loop-pred,map2check,non-det,track-bb,overflow,automata,memory-track" \
    array-2.bc -o array-2-instr.bc

# 3. Linkar com runtime
llvm-link array-2-instr.bc build/lib/libMap2CheckRuntime.bc -o array-2-linked.bc

# 4. Executar KLEE
klee --target-function=reach_error array-2-linked.bc

# 5. Verificar veredito
# array-2.c: FALSE (reachable) → VERIFICATION FAILED + counter-example
```

## Key Takeaways
1. O pipeline modernizado preserva a mesma arquitetura conceitual (compilar, instrumentar, analisar) mas com stack atualizada
2. O fix `isRequired()` foi o bug mais crítico — sem ele, nenhum passe executava com `opt -O0`
3. 9/9 passes carregando como plugins no New PM, 7/7 unit tests passando
4. CI/CD (GitHub Actions) + Docker (Ubuntu 22.04 + LLVM 16) garantem reprodutibilidade
5. Static analysis (`clang-tidy`, `cppcheck`) e sanitizers (ASAN, UBSAN, TSAN) elevam qualidade do código da própria ferramenta
6. OpenSSF Best Practices em andamento — checklist para badge de segurança

## Connects To
- **Ch 2**: Linha do tempo — como a arquitetura evoluiu de BMC monolítico para pipeline híbrido
- **Ch 4**: Cibersegurança — como cada pass mapeia para CWEs e resultados TestComp
- **Ch 5**: Demonstração — pipeline E2E será mostrado ao vivo
- **SV-COMP Benchmarks**: formato padronizado de tarefas de verificação usado para validação
