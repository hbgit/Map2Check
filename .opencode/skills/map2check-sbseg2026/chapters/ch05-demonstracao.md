# Chapter 5: Demonstração Planejada

## Core Idea
A demonstração ao vivo (obrigatória para o SBSeg SF) executará o pipeline completo do Map2Check em Docker — compilação → instrumentação pelos 9 passes → execução com KLEE → veredito + counter-example — com benchmark `array-2.c`. Um vídeo técnico de 5-7 minutos complementará com instalação e resultados TestComp 2026.

## Estrutura da Demonstração Ao Vivo

1. **Build com Docker**: `docker build -f Dockerfile.dev -t map2check-dev .` (ou pull da imagem pré-construída)
2. **Compilação**: `clang-16 -emit-llvm -c array-2.c`
3. **Instrumentação**: `opt` carrega 9 passes New PM como plugins
4. **Execução**: KLEE com timeout 300s, target `reach_error`
5. **Veredito**: FALSE (reachable) + counter-example mostrando o caminho do erro
6. **Framework TestComp 2026**: subset de benchmarks mostrando throughput

## Key Concepts
- **Equipamentos**: laptop com Docker + imagem `map2check-dev` + benchmarks `testcomp26`
- **Pré-construção**: Dockerfile.dev baseado em Ubuntu 22.04 + LLVM 16 + KLEE 3.x — build local ~30 min
- **Vídeo técnico**: obrigatório pelo edital — 5-7 minutos cobrindo instalação, build, execução E2E, resultados
- **Roteiro do vídeo**: documentado em `video/roteiro-gravacao.md` — passo a passo completo
- **Inscrição**: pelo menos 1 autor deve estar inscrito no evento (01-04/set, Búzios-RJ)

## Worked Example
**Demonstração ao vivo — benchmark `loops/array-2.c`:**

```c
// array-2.c: contém acesso fora dos limites
extern int __VERIFIER_nondet_int();
void reach_error() { __VERIFIER_error(); }

int main() {
    int a[10];
    int i = __VERIFIER_nondet_int();  // valor simbólico
    a[i] = 42;                         // i pode ser >= 10 → buffer overflow
    return 0;
}
```

Pipeline:
```bash
# Compilar
clang-16 -emit-llvm -g -c array-2.c -o array-2.bc

# Instrumentar (9 passes)
opt -load-pass-plugin=build/lib/libAssertPass.so \
    -load-pass-plugin=build/lib/libTargetPass.so \
    ... \
    array-2.bc -o array-2-instr.bc

# Executar
klee --target-function=reach_error array-2-instr.bc

# Resultado esperado:
# KLEE: ERROR: array-2-instr.bc Memory error: out of bound pointer
# Counter-example: __VERIFIER_nondet_int() → 31763
```

## Key Takeaways
1. Demonstração ao vivo é condição de aceitação — SBSeg SF exige presença + execução E2E
2. Vídeo de 5-7 minutos cobre instalação, build, execução e resultados — obrigatório no edital
3. Docker garante reprodutibilidade — mesmo ambiente (Ubuntu 22.04 + LLVM 16) em qualquer máquina
4. `array-2.c` é o benchmark canônico: demonstra detecção de buffer overflow com counter-example preciso
5. Acesso à internet necessário apenas para pull da imagem Docker (se não pré-construída localmente)

## Connects To
- **Ch 3**: Pipeline de verificação documentado na seção 3.1
- **Ch 4**: Smoke test validado — os mesmos benchmarks da demonstração
- **Ch 6**: Vídeo técnico e demonstração são pré-requisitos para publicação no SF
- **SBSeg 2026 SF**: edital com requisitos de demonstração (ao vivo + vídeo + presença)
