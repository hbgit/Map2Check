# Chapter 4: Análise Experimental

## Core Idea
O Map2Check 2026 foi validado no TestComp 2026 (Heap: 594 tasks, score 57, 56 bugs reais). O mapeamento SV-COMP → CWE estabelece a ferramenta como ponte entre verificação formal e cibersegurança. O suporte a WASM estende o pipeline para análise de binários WebAssembly via wasm2c.

## Mapeamento SV-COMP → CWE

| Propriedade SV-COMP | CWE | Técnica |
|---------------------|-----|---------|
| `valid-free` | CWE-415 (Double-Free) | Análise de fluxo de dados (MemoryTrackPass) |
| `valid-deref` | CWE-416 (Use-After-Free) | Análise de fluxo de dados (MemoryTrackPass) |
| `valid-memtrack` | CWE-401 (Memory Leak) | Tracking de alocações (MemoryTrackPass) |
| `valid-memsafety` | CWE-119/787 (Buffer Overflow) | Análise de intervalos + fluxo de dados |
| `no-overflow` | CWE-190 (Integer Overflow) | Análise de intervalos (OverflowPass) |

## Frameworks Introduzidos
- **CWE Mapping Framework**: cada propriedade de memory safety do SV-COMP mapeia para um CWE do MITRE + técnica empregada
  - When to use: para comunicar resultados de verificação formal para times de AppSec
  - How: associar `propriedade → CWE → pass/técnica` em tabela de referência

## Key Concepts
- **TestComp 2026 Heap**: sub-categoria `C.coverage-error-call.Heap`, 594 tasks, timeout 300s, solver Z3, target `reach_error`
- **Resultados consolidados**: TRUE 264 (44.4%), UNKNOWN 271 (45.6%), FALSE 56 bugs reais (9.4%), TIMEOUT 2 (0.3%)
- **Heap set**: 428 tasks — 39 FALSE, 203 TRUE, 184 UNKNOWN, 2 TIMEOUT
- **LinkedLists set**: 166 tasks — 18 FALSE, 61 TRUE, 87 UNKNOWN, 0 TIMEOUT
- **Smoke test validado**: `array-1.c` → TRUE (VERIFICATION SUCCEEDED), `array-2.c` → FALSE + counter-example `nondet_int → 31763`
- **WASM pipeline**: `.wasm → wasm2c (WABT 1.0.41) → .c → clang-16 → .bc → passes → KLEE`
- **wasi-sdk 33.0**: toolchain para compilar C → WASM com target `wasm32-wasip1`
- **Per-allocation bounds check WASM**: MemoryTrackPass intercepta `w2c_*_dlmalloc`/`dlfree` e injeta `map2check_wasm_check_access` em loads/stores
- **Juliet WASM**: 15 casos validados (CWE-121/122/124/126/127); 15/15 FALSE com timeout 290s
- **Linear memory gap**: heap detectável via dlmalloc; stack/globals sem alocador visível → inviável no MVP (mas Juliet stack/underread também foram detectados)

## Mental Models
- Use **UNKNOWN como KPI de engenharia**: 45.6% não é fracasso, é direcionamento — timeout tuning, estratégias de solver, path exploration heuristics
- Pense em **CWE como ponte academia-indústria**: o mapeamento torna resultados de verificação formal interpretáveis por profissionais de AppSec
- Use **WASM como novo vetor de memory safety**: mesmo pipeline LLVM, binários de terceiros (Wasmtime, Wasmer), sem reimplementar lógica de verificação
- Pense em **smoke test como sanity check**: 2 benchmarks canônicos (TRUE + FALSE) validam o pipeline completo em segundos

## Anti-patterns
- **Confiar apenas em unit tests**: 7/7 passando e ainda assim 3 inconsistências NPM escaparam → testes E2E são indispensáveis
- **Usar gráfico de pizza sem baseline**: comparar TRUE/FALSE/UNKNOWN sem baseline de concorrente não agrega — usar tabela numérica
- **Tratar WASM como "trabalho futuro"**: pipeline funcional (lifting validado), feature em desenvolvimento ativo — reportar como contribuição em andamento
- **Fazer o CI passar sem executar KLEE**: 8 commits consecutivos adaptando include paths e tornando WasmRuntimeStubs opcional deram green checkmark, mas não validaram o pipeline real — job Docker E2E necessário

## 3 Inconsistências Corrigidas na Migração

| Problema | Causa | Impacto |
|:---------|:------|:--------|
| KLEE 3.1 flags | Flags removidas entre KLEE 2.0 → 3.1 | `Unknown command line argument` — ferramenta não executava |
| Passes ignorados | `optnone` + New PM: passes não executam sem `isRequired()` | Instrumentação silenciosamente ausente — falsos TRUE |
| Target function | `__VERIFIER_error` vs `reach_error` — env var vs `cl::opt` | KLEE procurava função errada — veredito incorreto |

Nenhum desses problemas era detectável pelos 7/7 unit tests, que cobrem apenas a biblioteca C de runtime.

## Worked Example
**Validação E2E — smoke test:**

```bash
# TRUE case: sem bug
$ map2check --property-file coverage-error-call.prp array-1.c
VERIFICATION SUCCEEDED

# FALSE case: buffer overflow detectado
$ map2check --property-file coverage-error-call.prp array-2.c
VERIFICATION FAILED
Counter-example: nondet_int() → 31763, line 19, main
```

## Key Takeaways
1. TestComp 2026 Heap: 594 tasks, score 57, 56 bugs reais (9.4% FALSE), 45.6% UNKNOWN
2. Mapeamento CWE: 5 propriedades SV-COMP → 5 CWEs + técnica empregada — acionável por times AppSec
3. 3 inconsistências NPM corrigidas: KLEE flags, passes ignorados, target function — indetectáveis por unit tests
4. WASM: pipeline funcional (wasm2c 1.0.41 + wasi-sdk 33.0), lifting validado, per-allocation bounds check implementado
5. Juliet WASM: 15 casos (CWE-121/122/124/126/127), 15/15 FALSE com timeout 290s
6. Timeout é KPI de engenharia: 110s → UNKNOWN, 290s → FALSE nos casos Juliet WASM
7. Smoke tests canônicos (`array-1.c` TRUE, `array-2.c` FALSE) validam pipeline E2E em segundos

## Connects To
- **Ch 3**: Arquitetura — cada pass da seção 3 mapeia para CWE e resultado TestComp
- **Ch 6**: Conclusão — limitações (UNKNOWN 45.6%) e perspectivas (WASM, E2E tests)
- **Ch 5**: (removida do paper — Demonstração ao vivo era requisito do edital, não conteúdo)
- **MITRE CWE Top 25**: taxonomia padrão da indústria — `cwe2025`
