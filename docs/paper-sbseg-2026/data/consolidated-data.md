# Dados Consolidados — Artigo SBSeg 2026

## TestComp 2026 — C.coverage-error-call.Heap

| Resultado | Count | Porcentagem |
|:----------|:------|:------------|
| TRUE | 264 | 44.4% |
| UNKNOWN | 271 | 45.6% |
| FALSE | 56 | 9.4% |
| FALSE_FREE | 1 | 0.2% |
| TIMEOUT | 2 | 0.3% |
| **Total** | **594** | **100%** |
| **Score** | **57** | — |

### Por Set

| Set | Tasks | FALSE | TRUE | UNKNOWN | TIMEOUT |
|:----|:------|:------|:-----|:--------|:--------|
| Heap | 428 | 39 | 203 | 184 | 2 |
| LinkedLists | 166 | 18 | 61 | 87 | 0 |

### Configuracao Experimental

| Parametro | Valor |
|:----------|:------|
| LLVM | 16.0 |
| KLEE | 3.x (LLVM 16) |
| SMT Solver | Z3 |
| Timeout por task | 300s |
| Target function | reach_error |
| Property | coverage-error-call.prp |
| Benchmarks | sv-benchmarks tag testcomp26 |
| Ambiente | Docker (map2check-dev), single-container |
| Duracao total | ~20h (interrompida, retomada automaticamente) |

---

## Smoke Test E2E

| Benchmark | Esperado | Resultado | Status |
|:----------|:---------|:----------|:-------|
| loops/array-1.c | TRUE | VERIFICATION SUCCEEDED | OK |
| loops/array-2.c | FALSE | VERIFICATION FAILED + counter-example | OK |

### Counter-example (array-2.c)

```
State 0: __VERIFIER_nondet_int() -> 31763, Line 19, Scope: main
State 1: __VERIFIER_nondet_int() -> 31763, Line 22, Scope: main
Violated property: file map2check_property line 7 function __VERIFIER_assert
FALSE: Target Reached
```

---

## Estatisticas de Modernizacao

| Metrica | Valor |
|:--------|:------|
| Commits na branch feat-update (vs develop) | 35 |
| Arquivos alterados | 84 |
| Linhas adicionadas | +5.785 |
| Linhas removidas | −657 |
| Passes migrados (Legacy PM -> New PM) | 9 |
| Unit tests | 7/7 passando |
| Pass plugin load tests | 9/9 carregando no opt-16 |
| Bugs criticos encontrados e corrigidos | 3 |

### Componentes Atualizados

| Componente | Versao Anterior | Versao Nova |
|:-----------|:----------------|:------------|
| LLVM/Clang | 6.0 | 16.0 |
| KLEE | 2.x (fork custom) | 3.x |
| C++ Standard | C++11 | C++17 |
| CMake | >= 3.5 | >= 3.20 |
| Docker Base | Ubuntu 16.04 | Ubuntu 22.04 LTS |
| Pass Manager | Legacy (FunctionPass) | New PM (PassInfoMixin) |

---

## Bugs Criticos Corrigidos

| # | Bug | Sintoma | Causa Root | Commit |
|:--|:----|:--------|:-----------|:-------|
| 1 | KLEE 3.x CLI flags | Unknown command line argument | Flags removidas na v3.x | 284aef1ac |
| 2 | isRequired() ausente | Passes pulados silenciosamente | optnone + New PM behavior | 0deb84e01 |
| 3 | Target function nao propagada | __VERIFIER_error em vez de reach_error | env var vs cl::opt mismatch | 0deb84e01 |

---

## Mapeamento CWE <-> Propriedade SV-COMP

| Propriedade SV-COMP | CWE | Descricao | Pass Principal |
|:--------------------|:----|:----------|:---------------|
| valid-free | CWE-415 | Double-free | MemoryTrackPass |
| valid-deref | CWE-119/787 | Buffer overflow / Invalid dereference | MemoryTrackPass + OverflowPass |
| valid-memtrack | CWE-401 | Memory leak | MemoryTrackPass |
| valid-memcleanup | CWE-401 | Memory leak (cleanup) | MemoryTrackPass |
| no-overflow | CWE-190 | Integer overflow | OverflowPass |

---

## Checklist de Artefato CTA

| Criterio | Estado | Evidencia |
|:---------|:-------|:----------|
| Disponibilidade | OK | Repo publico + Docker image GHCR |
| Funcionalidade | OK | 7/7 unit tests + 9/9 passes |
| Reprodutibilidade | OK | Dockerfile.dev + scripts benchmark |
| Sustentabilidade | OK | CI/CD + static analysis + sanitizers + docs |
