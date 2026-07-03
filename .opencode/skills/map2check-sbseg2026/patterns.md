# Patterns — Map2Check SBSeg 2026

## Hybrid Verification Pipeline
**When to use**: programas C complexos onde nem fuzzing puro nem execução simbólica pura bastam
**How**: fases alternadas — LibFuzzer explora rápido bugs superficiais, KLEE resolve branches com magic numbers via path constraints
**Trade-offs**: fuzzer é rápido mas cego para branches complexos; KLEE é preciso mas sofre de path explosion

## Pass Migration Pattern (Legacy PM → New PM)
**When to use**: migrar qualquer pass LLVM do Legacy Pass Manager
**How**:
1. Herdar de `PassInfoMixin<NomeDoPass>` em vez de `ModulePass`/`FunctionPass`
2. Implementar `PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM)` ou `run(Module &M, ModuleAnalysisManager &AM)`
3. Registrar com `PassPluginLibraryInfo` e callback `PB.registerPipelineParsingCallback`
4. **Sempre** retornar `isRequired() = true` — sem isso, `opt -O0` pula o pass
**Trade-offs**: New PM é mais verboso mas oferece análise lazy e paralelismo

## CWE Mapping Pattern
**When to use**: tornar resultados de verificação formal acionáveis para times de AppSec
**How**:
1. Identificar qual propriedade SV-COMP cada pass verifica (ex: valid-free → CWE-415)
2. Documentar mapeamento em tabela: Propriedade → CWE → Pass responsável
3. Gerar output que referencia CWEs diretamente (ex: "CWE-416 detected at line 42")
**Trade-offs**: nem toda violação de propriedade SV-COMP tem CWE correspondente direto

## Checkpoint Validation Pattern
**When to use**: garantir que correções não introduzem regressões silenciosas
**How**:
1. Selecionar benchmarks canônicos (ex: `array-1.c`, `array-2.c`)
2. Executar pipeline completo e capturar veredito + counter-example
3. Comparar com baseline (veredito esperado + contra-exemplo esperado)
4. Automatizar com scripts `run_checkpoint.sh` + `verify_verdicts.sh`
**Trade-offs**: cobre apenas os benchmarks selecionados, não detecta regressões em paths não testados

## Docker Reproducibility Pattern
**When to use**: garantir que resultados são reproduzíveis em qualquer máquina
**How**:
1. `Dockerfile.dev` baseado em Ubuntu 22.04 + LLVM 16 + KLEE 3.x
2. Instalar todas as dependências no Dockerfile (sem "funciona na minha máquina")
3. Build automatizado no CI (GitHub Actions)
4. Documentar comandos exatos de build e execução
**Trade-offs**: imagem Docker é grande (~GBs) mas elimina barreiras de instalação e garante reprodutibilidade científica

## Iterative Deepening Pattern
**When to use**: combinar fuzzing + execução simbólica eficientemente
**How**:
1. Fase 1: LibFuzzer explora com seeds aleatórias, cobre caminhos superficiais
2. Fase 2: Sementes que estagnam (sem novos branches) são passadas ao KLEE
3. Fase 3: KLEE resolve path constraints, gera novas entradas que desbloqueiam caminhos
4. Repetir até timeout ou cobertura-alvo atingida
**Trade-offs**: custo de transição entre fases (serialização/deserialização de estado) pode dominar em programas pequenos

## Static Analysis + Sanitizers Quality Gate
**When to use**: manter qualidade do código da própria ferramenta de verificação
**How**:
1. `.clang-tidy` config para C++, `.cppcheck-suppressions.txt` para C
2. CMake targets: ASAN, UBSAN, TSAN
3. Script `run-static-analysis.sh` executa todas as verificações
4. CI (GitHub Actions) roda static analysis + sanitizers em cada push
**Trade-offs**: sanitizers adicionam overhead de runtime (2-5x), mas detectam bugs que testes unitários perdem
