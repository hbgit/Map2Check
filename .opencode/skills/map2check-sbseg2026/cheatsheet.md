# Cheatsheet — Map2Check SBSeg 2026

## Decision Rules

| Situação | Ação | Por quê |
|:---------|:-----|:--------|
| Pass LLVM não executa com `opt -O0` | Adicionar `isRequired() = true` | New PM só executa passes required em O0 |
| KLEE reclama de flag desconhecida | Verificar API da versão KLEE atual | Flags mudaram entre KLEE 2.0 → 3.x |
| Target function errada (ex: `__VERIFIER_error`) | Passar `-function-name=reach_error` na CLI | Nome da função-alvo inconsistente entre env var e cl::opt |
| UNKNOWN > 40% nos benchmarks | Investigar timeout tuning (300s → 600s?) + estratégias de solver | Timeout insuficiente ou path exploration não otimizada |
| Precisa garantir reprodutibilidade | Dockerfile.dev (Ubuntu 22.04 + LLVM 16) | Ambiente idêntico em qualquer máquina |
| Unit tests passando mas bugs escapam | Implementar testes E2E (compile → instrument → link → execute) | Unit tests não cobrem integração entre componentes |

## Trade-off Matrix: Pipeline Stages

| Estágio | Custo (tempo) | Cobertura | Precisão |
|:--------|:-------------|:----------|:---------|
| Fuzzing (LibFuzzer) | Baixo (segundos) | Alta (superficial) | Baixa (false positives) |
| Execução Simbólica (KLEE) | Alto (minutos) | Média (profunda) | Alta (contra-exemplos precisos) |
| Invariantes Indutivos (Crab-LLVM) | Médio | Baixa (propriedades específicas) | Alta (provas formais) |

## Thresholds & Defaults

| Parâmetro | Valor padrão | Significado |
|:----------|:------------|:------------|
| Timeout TestComp | 300s por task | Se verificação não converge em 5 min → UNKNOWN |
| Solver SMT | Z3 | Resolve path constraints na execução simbólica |
| Target function | `reach_error` | Função que indica violação de propriedade |
| Optimization level | O0 | Sem otimização para preservar debug info |
| Docker base | Ubuntu 22.04 | Sistema operacional do container de build |

## Tells & Smells

| Sinal | Diagnóstico provável |
|:------|:---------------------|
| Todos os passes carregam mas vereditos errados | `isRequired()` faltando → passes não executam de fato |
| `Unknown command line argument` no KLEE | Flags da versão KLEE 2.0 usadas com KLEE 3.x |
| UNKNOWN consistente em benchmarks com loops | Timeout insuficiente ou path explosion em laços |
| FALSE em benchmark que deveria ser TRUE | Bug de regressão na instrumentação — rodar checkpoint |
| Cobertura de branches estagnada após N iterações | Magic number bloqueando fuzzer — acionar KLEE |

## Checklist: Publicação SBSeg SF

- [ ] Submissão até 20/jul/2026
- [ ] 8 páginas (corpo) + até 2 páginas (referências/apêndices)
- [ ] Template SBC
- [ ] Vídeo técnico obrigatório (5-7 min)
- [ ] Pelo menos 1 autor inscrito no evento
- [ ] Demonstração ao vivo (Docker + pipeline E2E)
- [ ] CTA integrado ao camera ready (até 14/ago)
- [ ] Single-blind (autores conhecidos)
- [ ] Código aberto (elegível a prêmios)

## WASM Pipeline Quick Reference

| Comando | Descricao |
|:--------|:----------|
| `map2check --wasm --memtrack modulo.wasm` | Pipeline completo: lift + instrument + KLEE |
| `map2check --wasm --check-overflow modulo.wasm` | Deteccao de overflow aritmetico no WASM |
| `/opt/wasi-sdk-33.0-x86_64-linux/bin/clang --target=wasm32-wasip1 -o modulo.wasm modulo.c` | Compilar C para WASM |
| `wasm2c modulo.wasm -o modulo.c` | Lifting WASM para C (WABT 1.0.41) |

## Propriedades Detectaveis no WASM

| Propriedade | Detectavel? | Mecanismo |
|:------------|:------------|:----------|
| no-overflow (CWE-190) | Sim | OverflowPass: opera em operacoes binarias |
| coverage-error-call | Sim | TargetPass: busca reach_error() por nome |
| valid-free (CWE-415) | Parcial (heap) | dlmalloc interception: w2c_*_dlfree |
| valid-memtrack (CWE-401) | Parcial (heap) | dlmalloc interception: w2c_*_dlmalloc |
| valid-deref (CWE-119/787) | Parcial (heap) | Per-allocation bounds check |
| valid-memsafety (stack) | Nao | Linear memory gap: stack usa alloca, nao dlmalloc |

## CI Debug (wasm-rt.h)

| Erro | Causa | Correcao |
|:-----|:------|:---------|
| fatal error: wasm-rt.h not found | WABT nao instalado no CI | apt install wabt; find_path busca /usr/include |
| NO_DEFAULT_PATH bloqueia find_path | flag NO_DEFAULT_PATH em find_path | Remover NO_DEFAULT_PATH do CMakeLists.txt |
| WasmRuntimeStubs.bc falha | add_custom_target ALL forcou build | Remover ALL, tornar target opcional |
