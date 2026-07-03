# Chapter 3: Arquitetura e Funcionalidades

## Core Idea
O pipeline do Map2Check 2026 opera em 5 estágios: compilação C → LLVM IR (Clang-16), instrumentação por 9 passes (opt, New PM), linkagem com runtime (llvm-link), análise por KLEE 3.x/LibFuzzer, e veredito + witness GraphML. A infraestrutura CI/CD + Docker + OpenSSF Best Practices garante reprodutibilidade e sustentabilidade.

## Frameworks Introduzidos
- **Pipeline de 5 Estágios**:
  1. Compilação: `clang-16 -O0` → LLVM IR
  2. Instrumentação: `opt` com 9 passes New PM → bitcode instrumentado com chamadas à runtime
  3. Linkagem: `llvm-link` → bitcode + runtime library
  4. Análise: KLEE 3.x (execução simbólica) ou LibFuzzer (fuzzing)
  5. Veredito: TRUE/FALSE/UNKNOWN + witness GraphML com contra-exemplo

- **New Pass Manager (PassInfoMixin)**: API moderna do LLVM — substitui Legacy PM (FunctionPass/ModulePass)
  - When to use: sempre — Legacy PM está deprecado desde LLVM 13
  - How: herdar de `PassInfoMixin<T>`, registrar via `PassPluginLibraryInfo` e `registerPipelineParsingCallback`

## Técnicas Empregadas nos Passes

| Pass | Técnica | Vulnerabilidade coberta |
|:-----|:--------|:------------------------|
| **MemoryTrackPass** | Análise de fluxo de dados | Use-after-free (CWE-416), Double-free (CWE-415), Memory leak (CWE-401) |
| **OverflowPass** | Análise de intervalos | Buffer overflow (CWE-119/787), Integer overflow (CWE-190) |
| **GenerateAutomataTruePass** | Geração de autômato GraphML | Witness para validação externa (CPAchecker) |
| Demais passes | Suporte: predicados em laços, nondet, cobertura BB, asserts | Infraestrutura de verificação |

## Key Concepts
- **Witness GraphML**: formato padronizado SV-COMP descrevendo o caminho de execução até a violação como autômato — validável por ferramentas como CPAchecker
- **Runtime Library**: biblioteca C compilada a `.bc`, linkada ao bitcode instrumentado — implementa checks de runtime (bounds, alloc/free tracking, deref validation)
- **OpenSSF Best Practices**: programa de certificação de boas práticas para projetos open source — `openssf2024`
- **Sanitizers**: ASAN (AddressSanitizer), UBSAN (UndefinedBehavior), TSAN (ThreadSanitizer) — detectam bugs na própria ferramenta, não nos programas analisados
- **CI/CD (GitHub Actions)**: build automatizado com GCC e Clang, 7/7 unit tests, 9/9 pass plugin load tests, static analysis (clang-tidy, cppcheck)

## Mental Models
- Use **compilação -O0** com New PM porque preserva debug info para contra-exemplos precisos
- Pense em **witness GraphML como contrato de verificabilidade**: qualquer ferramenta pode validar o veredito reexecutando o witness
- Use **Docker como contrato de reprodutibilidade**: `Dockerfile.dev` (Ubuntu 22.04 + LLVM 16 + KLEE 3.x) documenta cada dependência
- Trate **sanitizers como primeira linha de defesa** da própria ferramenta — detectam bugs que unit tests perdem

## Anti-patterns
- **Misturar Legacy PM com New PM**: APIs incompatíveis — qualquer `ModulePass` legado quebra o pipeline `opt-16`
- **CI/CD sem testes E2E**: unit tests (7/7) não detectaram 3 inconsistências na API do New PM — testes de integração são indispensáveis
- **Build não containerizado**: "funciona na minha máquina" invalida resultados científicos — Docker é requisito mínimo de reprodutibilidade

## Worked Example
**Pipeline E2E — benchmark `array-2.c`:**

```bash
# 1. Compilar C → LLVM IR
clang-16 -emit-llvm -g -c array-2.c -o array-2.bc

# 2. Instrumentar com 9 passes
opt -load-pass-plugin=libMemoryTrackPass.so \
    -load-pass-plugin=libOverflowPass.so \
    ... \
    -passes='memory-track,overflow,...' \
    array-2.bc -o array-2-instr.bc

# 3. Linkar com runtime
llvm-link array-2-instr.bc \
    Map2CheckFunctions.bc \
    AnalysisModeMemtrack.bc \
    ... -o array-2-final.bc

# 4. Executar KLEE
klee --target-function=reach_error array-2-final.bc

# 5. Resultado: FALSE + counter-example (nondet_int → 31763)
```

## Key Takeaways
1. Pipeline em 5 estágios: compilar → instrumentar (9 passes NPM) → linkar → analisar → veredito + witness
2. MemoryTrackPass usa análise de fluxo de dados, OverflowPass usa análise de intervalos
3. Witness GraphML permite validação externa por CPAchecker — padrão SV-COMP
4. CI/CD + Docker + sanitizers + static analysis + OpenSSF = base de engenharia sustentável
5. 7/7 unit tests + 9/9 pass plugin load tests passando no CI
6. 3 inconsistências NPM descobertas durante migração — nenhuma detectável por unit tests

## Connects To
- **Ch 2**: História — do Legacy PM (LLVM 6) ao New PM (LLVM 16)
- **Ch 4**: Análise Experimental — resultados do pipeline no TestComp 2026
- **OpenSSF Best Practices**: certificação de segurança e qualidade — `openssf2024`
