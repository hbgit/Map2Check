# Roteiro — Vídeo Técnico Map2Check 2026 (SBSeg SF — 5-7 min)

## Cena 1: Apresentação (0:00–0:45)
**Slide:** Título + autores + UFRN/UFRR
- "Map2Check 2026 é uma ferramenta de verificação híbrida para C que combina execução simbólica e fuzzing"
- "SBSeg 2026 — Salão de Ferramentas"
- Stack: LLVM 16, KLEE 3.x, C++17, Docker

## Cena 2: Demonstração E2E — Buffer Overflow (0:45–2:00)
**Terminal ao vivo:**
```bash
$ cat array-2.c
$ map2check --property-file coverage-error-call.prp array-2.c
VERIFICATION FAILED
Counter-example: nondet_int() -> 31763
```
- Mostrar: compilação → instrumentação → KLEE → contra-exemplo

## Cena 3: Pipeline de Verificação (2:00–2:30)
**Slide:** Diagrama do pipeline (fig2-pipeline.png)
- 5 estágios: compilação, instrumentação, linking, análise, veredito
- 9 passes LLVM New PM (MemoryTrackPass, OverflowPass, etc.)

## Cena 4: Resultados TestComp 2026 (2:30–3:30)
**Slide:** Tabela de resultados Heap + ControlFlow
- Heap: 594 tasks, score 57, 56 bugs reais encontrados
- ControlFlow: 138 tasks, score 38, 38 bugs
- KPI: 45.6% UNKNOWN → direcionamento para próximos ciclos

## Cena 5: WASM Pipeline (3:30–4:30)
**Slide:** Diagrama WASM pipeline (fig6-wasm-pipeline.png)
- `.wasm → wasm2c → LLVM IR → Passes → KLEE`
- `map2check --wasm modulo.wasm` — mesmo pipeline, binários de terceiros

## Cena 6: Demo WASM — Heap Overflow detectado (4:30–5:30)
**Terminal ao vivo:**
```bash
$ cat test_heap_overflow.c  # malloc(10*sizeof(int)); p[10] = 999;
$ wasi-sdk clang --target=wasm32-wasip1 test_heap_overflow.c -o overflow.wasm
$ map2check --wasm --memtrack overflow.wasm
VERIFICATION FAILED
Violated property: OVERFLOW
```
- Juliet: 15/15 casos FALSE

## Cena 7: Infraestrutura de Engenharia (5:30–6:15)
**Slide:** CI/CD + Docker + OpenSSF
- GitHub Actions: build, test, static analysis, sanitizers, E2E Docker
- Dockerfile.dev: ambiente reproduzível
- OpenSSF Best Practices badge

## Cena 8: Conclusão + Links (6:15–7:00)
**Slide:** Repositório + DOI + contato
- `github.com/hbgit/Map2Check`
- Código aberto (GPL-2.0), Docker image pública (GHCR)
- Contribuições bem-vindas
