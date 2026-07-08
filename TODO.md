# TODO — OpenSSF Best Practices Badge

> Status atualizado em 2026-07-08 (branch `feat-valgrind`). Histórico completo
> dos achados originais: commit `eb006d27`; rastreio item a item em
> `docs/migration-schedule.md` (seção 2.0).

---

## `static_analysis_fixed` — ✅ Met (resolvido em 2026-07-08)

> All medium and higher severity exploitable vulnerabilities discovered with static code analysis MUST be fixed in a timely way after they are confirmed. [`static_analysis_fixed`]

Confirmados em 2026-06-14, corrigidos em ~3 semanas (referência usual do badge: ≤60 dias):

- [x] CWE-119 `strcpy` ×3 — `map2check.cpp` → `setenv()` (`f0d6a28a`)
- [x] Off-by-one OOB — `BTree.c` (create loop `588ba5f8`; dump loop `e5442766`)
- [x] VLA dangling return — `NonDetGeneratorKlee.c` **e** `NonDetGeneratorLibFuzzy.c` (cópia extra achada na verificação) (`f0d6a28a`)
- [x] Shift UB — `NonDetGeneratorLibFuzzy.c` (`f0d6a28a`)
- [x] Uninit vars — `AllocationLog.c`, `NonDetLog.c`, `ContainerBTree.c` (`ca2692c5`, `f0d6a28a`)
- [x] Null-deref CWE-476 — `AnalysisModeMemtrack.c`/`AnalysisModeMemcleanup.c` (checagens de NULL com corpo vazio) (`e5442766`)

**Verificação (imagem Docker do CI, clang-16):** cppcheck backend C com **zero**
achados error/warning (scan agora **bloqueante** no CI); cppcheck C++ enforced
exit 0; clang-tidy `clang-analyzer-security/core` sem achados.

---

## `dynamic_analysis_fixed` — ✅ Met (resolvido em 2026-07-08)

> All medium and higher severity exploitable vulnerabilities discovered with dynamic code analysis MUST be fixed in a timely way after they are confirmed. [`dynamic_analysis_fixed`]

- [x] UBSan: OOB write `BTree.c:276` (CWE-787), disparado em `BTreeTest`/`ContainerBTreeTest` (`588ba5f8`)
- [x] ASan: leaks em `AllocationLogTest` e `AllocationLog.c:89` (`dd7f3268`, `e9287b95`)
- [x] Valgrind memcheck (novo job): 8 defeitos em `BTreeTest`, double-free/leak em `AllocationLog`, uninit em `ContainerBTree` — todos corrigidos; falso positivo (pool eh da libstdc++) suprimido com justificativa em `valgrind.supp` (`ba4e3728`)
- [x] CI não tolera mais achados: `sanitizer-tests` estrito (`detect_leaks=1`, `halt_on_error=1`, `0a0e6c08`) e `valgrind-memcheck` bloqueante (`--error-exitcode=1`)

**Verificação:** suíte de 6 testes 100% verde sob ASan/UBSan estritos e
`ctest -T memcheck` com 0 defects (imagem Docker do CI).

---

## `dynamic_analysis_unsafe` — ❌ Not Met (metade do fuzzer segue faltando)

> It is SUGGESTED that ... at least one dynamic tool (e.g., a fuzzer or web application scanner) be routinely used in combination with a mechanism to detect memory safety problems such as buffer overwrites. [`dynamic_analysis_unsafe`]

O que existe (atualizado): mecanismos de memory-safety **duplicados** — ASan/UBSan
estritos + Valgrind memcheck bloqueante. O que falta (inalterado):

- [ ] Nenhum fuzzing do próprio Map2Check: `SKIP_LIB_FUZZER=ON` nos jobs de teste; o LibFuzzer embarcado é *feature do produto* (gera entradas para os programas C analisados), não self-fuzzing
- [ ] Sem harness `LLVMFuzzerTestOneInput`, corpus ou integração OSS-Fuzz
- [ ] Iniciativa real na roadmap: AFL++ (Phase 3, itens 3.1.1–3.1.4) — não iniciada

---

## `dynamic_analysis_enable_assertions` — ⚠️ Met apenas na forma (inalterado)

> It is SUGGESTED that the project use a configuration for at least some dynamic analysis (such as testing or fuzzing) which enables many assertions. [`dynamic_analysis_enable_assertions`]

- Configuração OK: nenhum job usa `-DNDEBUG` (o job Valgrind builda `Debug` explicitamente)
- [ ] Mas re-verificado em 2026-07-08: **0 ocorrências** de `assert()`/`BOOST_ASSERT()` em `modules/` — não há asserts para habilitar. Adotar `assert()` em invariantes internos (ex.: invariantes estruturais do BTree, checagem de tipo dos containers); o `.clang-tidy` já reconhece esses macros
