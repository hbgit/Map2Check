# 4. Features de Ciberseguranca

**Estado:** Rascunho nao iniciado.  
**Fontes:** docs/migration/1.4.3-checkpoint-testcomp2026.md, docs/migration/1.4-checkpoint-smoke-test.md.  
**Figuras:** Figura 4 (resultados TestComp 2026).

---

## 4.1 Mapeamento de memory safety para CWEs

[PARAGRAFO A REDIGIR]
- CWE-401: Memory Leak
- CWE-416: Use-After-Free
- CWE-415: Double-Free
- CWE-119 / CWE-787: Buffer Overflow
- Tabela: Propriedade SV-COMP -> CWE -> pass responsavel

## 4.2 TestComp 2026 — Execucao Heap

[PARAGRAFO A REDIGIR]
- Contexto: TestComp 2026, sub-categoria C.coverage-error-call.Heap
- Configuracao experimental
- Tabela de resultados consolidados
- Tabela por set (Heap, LinkedLists)
- Figura 4: Grafico dos resultados

## 4.3 Smoke test e validacao de vereditos

[PARAGRAFO A REDIGIR]
- Execucao E2E manual em benchmarks loops/array-1.c e loops/array-2.c
- Counter-example validado
- Framework de checkpoint automatizado

## 4.4 Bugs criticos encontrados e corrigidos

[PARAGRAFO A REDIGIR]
- Tabela dos 3 bugs (KLEE flags, isRequired, target function)
- Destacar: nenhum detectavel pelos unit tests existentes
- Implicacao: necessidade de testes de integracao E2E

## 4.5 Suporte a WebAssembly (WASM) — Em Desenvolvimento

[PARAGRAFO A REDIGIR]
- Extensao do pipeline LLVM 16 existente para analise de modulos WASM
- Backend LLVM-WASM: compilacao C -> LLVM IR -> WASM -> instrumentacao -> verificacao
- Vetor de ataque: memory safety em runtimes WASM (Wasmtime, Wasmer)
- Testes preliminares em benchmarks SV-COMP adaptados para WASM
- Status: implementacao em andamento na branch feat-update (pos-merge)

---

*Nota: Aguardando redacao na Sprint 2. WASM sera integrado conforme resultados dos testes SV-COMP.*
