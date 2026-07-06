# Plano de Implementação — WASM Pipeline (Map2Check)

**Branch:** `feat-wasm-verification`  
**Deadline MVP:** 13/jul/2026  
**Deadline submissão SBSeg:** 20/jul/2026

---

## Contexto atual

| Componente | Estado |
|------------|--------|
| WasmLifter (`wasm_lifter.{hpp,cpp}`) | ✅ Compila, integrado ao CMake, NÃO invocado pelo CLI |
| Docker (WABT 1.0.41 + wasi-sdk 33.0) | ✅ Dockerfile.dev atualizado |
| Scripts de validação (`test_wasm_pipeline.sh`) | ✅ Pipeline C→WASM→LLVM IR validado |
| Paper SBSeg (LaTeX) | ✅ 6 seções escritas, falta seção WASM |
| Integração CLI (`map2check --wasm`) | 🔴 Não existe |
| PoC E2E (detectar bug real em WASM) | 🔴 Não testado |
| MemoryTrackPass adaptado | 🔴 Não iniciado |

---

## Arquitetura Proposta

O WasmLifter já produz `.bc` (LLVM bitcode). O pipeline existente:

```
C (.c) → clang → .bc → opt (passes) → llvm-link (runtime) → KLEE → veredito
```

Para WASM, o step de compilação é substituído pelo lifter:

```
WASM (.wasm) → wasm2c → .c → clang-16 → .bc → opt (passes) → llvm-link (runtime) → KLEE → veredito
                └─────────── WasmLifter ──────────┘    └─────── pipeline existente (inalterado) ───────┘
```

**Vantagem**: do `.bc` pra frente, tudo é reutilizado — passes, runtime, KLEE, leitura de veredito.

---

## Sub-etapas

### 1.7.3 — PoC E2E manual (2 dias)

**Objetivo**: Provar que o pipeline detecta um bug de memory safety em WASM.

**Tarefas**:
1. Criar caso de teste canônico: `test_overflow.c` com buffer overflow intencional
2. Compilar para WASM: `wasi-sdk clang --target=wasm32-wasip1 test_overflow.c -o test.wasm`
3. Liftar para LLVM IR: `WasmLifter::lift("test.wasm")` → `test.bc`
4. Inspecionar o IR: verificar se o wasm2c gera `load`/`store`/`call` que o MemoryTrackPass reconhece
5. Rodar passes: `opt -load-pass-plugin=libMemoryTrackPass.so -passes=memory-track test.bc -o test-instr.bc`
6. Linkar com runtime: `llvm-link test-instr.bc Map2CheckFunctions.bc AnalysisModeMemtrack.bc ...`
7. Executar KLEE: verificar se o overflow é detectado (FALSE + counter-example)
8. Documentar descobertas: padrões de IR do wasm2c que diferem do C nativo

**Riscos**:
- `wasm2c` não gera `malloc`/`free` → usa `wasm_rt_allocate_memory()` → MemoryTrackPass não captura
- Memória linear WASM é array contíguo acessado via offset, não ponteiro → bounds check precisa ser adaptado
- Entry point é `w2c__start`, não `main` → Map2CheckLibrary pass não injeta init/exit

---

### 1.7.4 — Adaptação MemoryTrackPass para WASM (3 dias)

| Componente | Problema | Solução |
|------------|----------|---------|
| **Map2CheckLibrary** | Não reconhece `w2c__start` como entry point | Adicionar flag `--entry-function` ou detectar `w2c_*` automaticamente |
| **MemoryTrackPass** | `malloc`/`free` não existem no IR wasm2c — usa `wasm_rt_allocate_memory` | Adicionar handlers para `wasm_rt_*` calls; instrumentar `wasm_rt_allocate_memory` como `map2check_malloc` |
| **MemoryTrackPass** | Ponteiros são offsets i32 dentro da memória linear, não endereços reais | Tratar stores/loads em `wasm_rt_memory_t.data` como acessos à memória; validar offset < `memory.size` |
| **OverflowPass** | Buffer overflow em WASM = acesso à memória linear além do bounds | Adicionar bounds check: `assert(offset + size <= memory_pages * 64KB)` antes de cada store/load |
| **Runtime** | `map2check_malloc` espera endereço real, WASM usa offset | Criar modo `WASM_MODE` na runtime que trabalha com offsets relativos à base da memória linear |

---

### 1.7.5 — CLI `--wasm` + WasmBackend (2 dias)

1. Adicionar flag `--wasm` no `map2check.cpp` (via `boost::program_options`)
2. Modificar `map2check_execution()`:
   ```cpp
   if (args.wasmMode) {
       WasmLifter lifter(config);
       WasmLifterResult result = lifter.lift(args.inputFile);
       args.inputFile = result.bitcodePath; // substitui .wasm por .bc
   }
   // Resto do pipeline inalterado
   ```
3. Passar `--wasm-mode` para o `callPass()` quando em modo WASM
4. Ajustar `Caller::compileCFile()` para ser no-op quando input já é `.bc`

**Arquivos afetados**:
- `modules/frontend/map2check.cpp` — adicionar flag + WasmLifter call
- `modules/frontend/map2check.hpp` — adicionar campos ao `map2check_args`
- `modules/frontend/caller.cpp` — opcional: skip compileCFile para .bc

---

### 1.7.6 — Benchmarks Juliet WASM (2 dias)

1. Selecionar subset da Juliet Test Suite: CWE-119 (Buffer Overflow), CWE-416 (Use-After-Free)
2. Compilar para WASM com wasi-sdk-33
3. Rodar pipeline em cada benchmark
4. Tabular: total, bugs detectados (FALSE), safe confirmados (TRUE), inconclusivos (UNKNOWN)
5. Comparar taxa de detecção: WASM vs C nativo (via pipeline normal)

---

### 1.7.7 — Documentação no artigo (2 dias)

1. Escrever seção WASM (~1 página) no `main.tex`: arquitetura do lifter, modelagem, pipeline, resultados Juliet
2. Adicionar figura do pipeline WASM
3. Atualizar abstract/conclusão com menção ao suporte WASM

---

## Cronograma

| Data | Marco |
|------|-------|
| 04-05/jul | 1.7.3 PoC E2E |
| 06-08/jul | 1.7.4 Passes adaptados |
| 09-10/jul | 1.7.5 CLI integrado |
| 11-12/jul | 1.7.6 Juliet benchmarks |
| 13/jul | 1.7.7 Seção WASM no artigo |

**Dependência crítica**: 1.7.3 é o gate — se o PoC falhar, reavaliar abordagem.

---

## Riscos (atualizado)

| Risco | Impacto | Prob | Mitigação |
|:---|:---|:---|:---|
| wasm2c gera código C incompreensível para passes | Alto | Média | Validar exemplos simples primeiro; ajustar passes para `wasm_rt_*` |
| KLEE não resolve memória linear levantada | Alto | Média | Injetar asserts de bounds manualmente; `klee_make_symbolic` para offsets |
| Juliet compilado para WASM perde semântica de malloc/free | Médio | Alta | Validar preservação; mapear para `wasm_rt_malloc` se necessário |
| IR levantado explode após lifting | Médio | Média | Usar slicing antes da instrumentação; limitar a benchmarks pequenos |
