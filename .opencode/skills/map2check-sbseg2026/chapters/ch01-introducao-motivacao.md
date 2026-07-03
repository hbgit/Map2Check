# Chapter 1: Introdução

## Core Idea
Memory safety em C/C++ lidera o *2025 CWE Top 25* com Out-of-bounds Write (CWE-787), Use-After-Free (CWE-416) e Buffer Overflow (CWE-119). A verificação híbrida — combinando execução simbólica (KLEE) e fuzzing (AFL++) — é a abordagem mais promissora, e o Map2Check 2026 renasce como ferramenta competitiva após 7 anos de estagnação técnica.

## Frameworks Introduced
- **Verificação Híbrida**: combinação de execução simbólica (KLEE) + fuzzing guiado por cobertura (AFL++)
  - When to use: quando fuzzing puro não penetra guardas lógicas complexas, e execução simbólica pura sofre de path explosion
  - How: fases alternadas entre exploração rápida (fuzzer) e resolução de path constraints via SMT (execução simbólica)
- **Smart Seeds (FuSeBMC)**: injeção de sementes geradas por BMC no fuzzer, atingindo >81% de cobertura em categorias críticas da Test-Comp
  - When to use: quando o fuzzer estagna em branches com magic numbers
  - How: BMC resolve guardas lógicas e injeta entradas concretas de volta na fila do fuzzer
- **Program Slicing (Symbiotic)**: integração de slicing estático (DG Library) com KLEE para reduzir espaço de busca
  - When to use: programas grandes onde a análise simbólica sofre de path explosion
  - How: remove trechos de código que não afetam as propriedades de segurança antes da execução simbólica

## Key Concepts
- **CWE Top 25 (2025)**: CWE-787, CWE-416, CWE-119 entre as vulnerabilidades mais perigosas — fonte: `cwe2025`
- **SV-COMP / TestComp**: competições internacionais de verificação de software — fonte: `beyersvcomp2013`, `beyertestcomp2023`
- **KLEE**: motor de execução simbólica de referência para LLVM bitcode — fonte: `klee2008`
- **AFL++**: fuzzer guiado por cobertura, sucessor do AFL — fonte: `fioraldi2020aflpp`
- **FuSeBMC**: ferramenta com smart seeds + BMC, líder em cobertura na Test-Comp — fonte: `fusebmc2022`
- **Symbiotic**: ferramenta com program slicing + KLEE, destaque em MemSafety — fonte: `chalupa2021symbiotic`
- **ESBMC**: bounded model checker com múltiplos solvers SMT — fonte: `gadelha2021esbmc`
- **OpenSSF Best Practices**: certificação de boas práticas para projetos open source — fonte: `openssf2024`
- **Therac-25**: caso histórico de falha de software em sistema crítico médico — fonte: `leveson1993investigation`

## Mental Models
- Use **fuzzing como primeira linha** — rápido, cobre caminhos superficiais, detecta bugs fáceis
- Use **execução simbólica como solver de guardas** — preciso para branches com magic numbers, mas caro
- Pense em **orquestração como diferencial competitivo**: FuSeBMC ganhou com smart seeds, Symbiotic com slicing — o Map2Check 2026 precisa dessa camada
- Use **CWE como linguagem comum** entre verificação formal e times de AppSec — torna resultados acionáveis

## Anti-patterns
- **Ferramenta sem CI/CD**: sem reprodutibilidade, resultados não são validáveis por terceiros — pré-requisito para publicação científica
- **Stack EOL (LLVM 6, Ubuntu 16.04)**: inviabiliza participação em competições e perde compatibilidade com solvers modernos
- **Motores sem orquestração**: KLEE e LibFuzzer operando em fases estanques, sem compartilhamento de estado — perde eficiência vs. FuSeBMC

## Worked Example
**Exemplo de uso do Map2Check 2026:**

```c
// array-2.c — buffer overflow intencional
int main() {
    int a[10], i = __VERIFIER_nondet_int();
    a[i] = 42;  // i pode ser >= 10 → buffer overflow
    return 0;
}
```

Pipeline completo via CLI:
```bash
$ map2check --property-file coverage-error-call.prp array-2.c
VERIFICATION FAILED
Counter-example: nondet_int() -> 31763, line 3, scope main
```

O comando `map2check` compila com Clang-16, instrumenta com os passes de memory safety, executa o KLEE e reporta o veredito FALSE com contra-exemplo concreto.

## Key Takeaways
1. Memory safety bugs lideram o CWE Top 25 2025 — CWE-787, CWE-416, CWE-119
2. Verificação híbrida é o estado da arte — FuSeBMC (>81% cobertura), Symbiotic (slicing+KLEE), ESBMC (multi-solver BMC)
3. Map2Check estava estagnado desde 2019 (LLVM 6.0, Ubuntu 16.04 EOL, KLEE 2.0 fork)
4. 5 contribuições da versão 2026: (1) LLVM 6→16 + New PM + C++17, (2) CI/CD + Docker + sanitizers + OpenSSF, (3) TestComp 2026 score 57, (4) CWE mapping, (5) WASM suporte
5. Exemplo concreto: `map2check array-2.c` → VERIFICATION FAILED + counter-example `nondet_int() → 31763`

## Connects To
- **Ch 2**: História e evolução — como cada checkpoint do Map2Check se compara aos concorrentes
- **Ch 3**: Arquitetura — pipeline de 5 estágios com passes de instrumentação
- **Ch 4**: Análise Experimental — resultados TestComp 2026 e CWE mapping
- **CWE Top 25 2025**: referência do MITRE para classificação de vulnerabilidades
