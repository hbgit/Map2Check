# tacasv1 — Troca do LibFuzzer pelo AFL++ (modo persistente)

**Data:** 2026-09-25
**Branch:** `tacas/aflpp` (a partir de `develop`)
**Status:** ✅ Design aprovado — aguardando plano de implementação

---

## 1. Objetivo

Substituir **completamente** o LibFuzzer pelo **AFL++ 4.40c** como motor de fuzzing do
Map2Check, preservando o restante do pipeline (KLEE, smart seeding, slicing) byte a byte.
A medição da tacasv1 isola **apenas** a contribuição do motor de fuzzing, comparando
`tacas/aflpp` contra a baseline `v15` (= `develop`).

O entregável é uma versão em que o `--nondet-generator afl` e o loop híbrido default
(fuzzer → KLEE → seed-exchange opcional) rodam AFL++ em modo persistente, com paridade de
semântica nondet e de veredito com a v15.

> Linha de desenvolvimento: `decisions/tacas-afl-slicing-roadmap.md` (memória do projeto).
> Referências: `docs/map2check_migration_plan.md` (Fase 3), `docs/migration-schedule.md`.

---

## 2. Decisões fechadas

| Decisão | Valor |
|---|---|
| Branches das frentes | `tacas/aflpp`, `tacas/slicing`, `tacas/combined` (todas a partir de `develop`) |
| Numeração `tacasvN` | **global** (rodada de avaliação), não por braço |
| Ordem de construção | AFL++ (tacasv1) → slicing (tacasv2) → combined (tacasv3) |
| Escopo tacasv1 | **só a troca do fuzzer** — smart seeding fica exatamente como está |
| Versão AFL++ | **4.40c** (última da linha 4.x; ver §3) |
| Instrumentação | `afl-clang-fast` + `AFL_LLVM_INSTRUMENT=PCGUARD` (LLVM 16) |
| Modo de execução | **persistente** (`__AFL_FUZZ_INIT` + `__AFL_LOOP`) |
| Paralelismo | **1 instância** `afl-fuzz` na tacasv1; `-M/-S` em PR subjacente posterior |
| Coordenador | **permanece no Caller C++** (não vira módulo Python/pybind11) |
| Política de nomes | **rename completo** — sem alias de transição |

---

## 3. Verificação da versão do AFL++

- `v4.40c` é real: publicada em **2026-03-13**, última release da linha 4.x (madura).
  Existe `v5.03c` (2026-09-02), linha 5.x recém-saída — **não** adotada por
  reprodutibilidade do paper e consistência com o mapeamento da literatura (FuSeBMC,
  MEUZZ, Symbiotic usam 4.x).
- **Compatibilidade LLVM 16**: o `Dockerfile.dev` fixa `clang-16`/`llvm-16`
  (apt.llvm.org). O `afl-clang-fast` da 4.40c compila o pass `afl-llvm-pass.so` contra o
  LLVM presente no build; o modo **PCGUARD** (`-fsanitize-coverage=trace-pc-guard`) exige
  LLVM ≥ 14 — satisfeito pelo LLVM 16.

---

## 4. Escopo

### Dentro (tacasv1)
- `cmake/FindAFLPlusPlus.cmake` novo; remoção de `cmake/FindLibFuzzer.cmake`.
- `NonDetGeneratorLibFuzzy.c` → `NonDetGeneratorAFL.c` (driver persistente).
- `Caller` (`caller.cpp`/`caller.hpp`): compilação (`applyNonDetGenerator`), execução
  (`executeAnalysis`), link (`linkLLVM`) e enum.
- CLI: valor `--nondet-generator fuzzer` → `afl`.
- CMake root: `SKIP_LIB_FUZZER` → `SKIP_AFL_PLUS_PLUS`.
- `Dockerfile.dev`: seção de build do AFL++ 4.40c (pin de SHA).
- CI/release: `.github/workflows/{ci,release}.yml`, `scripts/{make-release,prepare-release}.sh`.
- Docs: `README.md`, `CLAUDE.md`, `CHANGELOG.md`, `TODO.md`, plano de migração (§3.2).

### Fora (adiado)
- Revamp de smart seeding (laço alternado, ranking SDG, múltiplos `--seed-file`) — tacasv2/v3.
- Calibração do slicing (`--cutoff-diverging` etc.) — tacasv2.
- Paralelismo `-M/-S` do `afl-fuzz` — PR subjacente posterior.
- Coordenador como processo/módulo separado — descartado (fica no Caller C++).

---

## 5. Build system

- **`cmake/FindAFLPlusPlus.cmake`** novo, análogo ao `FindKlee.cmake`: localiza/instala
  `afl-clang-fast`, `afl-fuzz`, `afl-cc` e expõe os caminhos. **Deleta**
  `cmake/FindLibFuzzer.cmake`.
- **`CMakeLists.txt`** (root): `option(SKIP_LIB_FUZZER ...)` →
  `option(SKIP_AFL_PLUS_PLUS ...)`; troca o `include(cmake/FindLibFuzzer.cmake)` pelo novo.
- **`modules/backend/library/lib/CMakeLists.txt`**: `NonDetGeneratorLibFuzzy` →
  `NonDetGeneratorAFL` (continua compilado a `.bc` via `clang -c -emit-llvm`).
- **`Dockerfile.dev`**: nova seção (após KLEE, no padrão das seções KLEE/DG) que clona e
  compila AFL++ **4.40c** com o toolchain LLVM 16, `ARG AFL_PLUS_PLUS_SHA` pinado, e
  `ENV AFL_PATH`/`PATH` apontando para o install. Remove a nota "LibFuzzer sem install
  extra" (§7 atual).
- **CI/release**: substituir `-DSKIP_LIB_FUZZER=ON` por `-DSKIP_AFL_PLUS_PLUS=ON` em
  `.github/workflows/ci.yml`, `.github/workflows/release.yml`, `scripts/make-release.sh`,
  `scripts/prepare-release.sh`, `make-unit-test.sh`.

---

## 6. Runtime — nondet generator e Caller

### 6.1 `NonDetGeneratorAFL.c` (substitui `NonDetGeneratorLibFuzzy.c`)
- Trampoline:
  ```c
  __AFL_FUZZ_INIT();
  int main(void) {
      while (__AFL_LOOP(10000)) {
          unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;
          int len = __AFL_FUZZ_TESTCASE_LEN;
          set_global_input(buf, len);
          __map2check_main__(0, NULL);
      }
  }
  ```
- `get_next_input_from_fuzzer()` / `get_bytes_from_fuzzer()` leem do buffer global
  (preenchido por `__AFL_FUZZ_TESTCASE_BUF`), preservando o contrato de largura
  `sizeof(type)` (o fix documentado em `NonDetGeneratorLibFuzzy.c`).
- `nondet_assume()` → `abort()` (AFL trata abort como crash; mesmo contrato que o
  LibFuzzer já assumia via `pthread_exit`).
- `NonDetLog.c` (gravação de `klee_log.csv`) fica **intocado** — a semântica do veredito
  `cover-error` depende dele.

### 6.2 `caller.cpp` — `applyNonDetGenerator()` (caso fuzzer)
Substituir as duas compilações:
```
clang -g -fsanitize=fuzzer -fsanitize-coverage=inline-8bit-counters -O2 -o <hash>-fuzzed.out <hash>-result.bc
clang -g -fsanitize=fuzzer -o <hash>-witness-fuzzed.out <hash>-witness-result.bc
```
por:
```
afl-clang-fast -O2 -o <hash>-fuzzed.out <hash>-result.bc
afl-clang-fast -O2 -o <hash>-witness-fuzzed.out <hash>-witness-result.bc
```
com `AFL_LLVM_INSTRUMENT=PCGUARD` exportado no ambiente (via `ENV` no `Dockerfile.dev`,
não inline no comando). Mantém o `timeout -k <grace> <compileBudget>` existente (o
orçamento de compilação continua valendo — é ele que evita estourar o budget em
programas grandes).

### 6.3 `caller.cpp` — `executeAnalysis()` (caso fuzzer)
Substituir a execução
```
./<hash>-fuzzed.out -jobs=8 -use_value_profile=1 [corpus] > fuzzer.output
```
por:
```
afl-fuzz -i seeds -o afl-out -V <seconds> -- ./<hash>-fuzzed.out
```
- O AFL++ **exige** diretório de entrada não vazio (diferente do LibFuzzer, que parte do
  vazio). O Caller garante ao menos 1 seed: se `seeds/` estiver vazio, grava um arquivo
  mínimo (1 byte) antes de invocar o `afl-fuzz`.
- `<seconds>` deriva de `remainingSeconds()` (análogo ao `kleeBudget`:
  `max(1, remainingSeconds() - 5)`), não de um literal.
- Crash-replay: varrer `afl-out/crashes/id:*` e re-executar cada um com
  `./<hash>-witness-fuzzed.out <arquivo>` (o `__AFL_FUZZ_INIT` em modo standalone lê
  `argv[1]` como arquivo de entrada), substituindo o replay de `crash-*` do LibFuzzer.

### 6.4 `caller.cpp` — `linkLLVM()` e enum
- `NonDetGeneratorLibFuzzy.bc` → `NonDetGeneratorAFL.bc`.
- Enum `NonDetGenerator::LibFuzzer` → `NonDetGenerator::AFLPlusPlus`
  (`caller.hpp:21-46`, `caller.cpp` casos, `map2check.cpp` captura do
  `--nondet-generator`).

### 6.5 `map2check.cpp` — CLI e loop híbrido
- Valor `fuzzer` do `--nondet-generator` → `afl`.
- Loop híbrido em `main()` (`map2check.cpp:949-982`) **inalterado estruturalmente**:
  fuzzer → KLEE → seed-exchange opcional. Só o passo fuzzer passa a rodar AFL++.

---

## 7. Semântica de execução

- **Budget**: `compileBudget` (compilação) inalterado; execução do `afl-fuzz` limitada
  por `-V <seconds>` (AFL++ "run N seconds"), com o `timeout -k <killGracePeriod>` do
  Caller como backstop.
- **Paralelismo**: 1 instância `afl-fuzz` na tacasv1. `-M master + -S slave` fica para
  PR subjacente posterior (mapeia o `-jobs=8` antigo).
- **Contrato a preservar**: `klee_log.csv` continua gravando por iteração; o fluxo
  fuzzer→KLEE de **1 vetor** (`readNonDetLogAsObjects` → `seeds/from-fuzzer.ktest` →
  `--seed-file`) permanece exatamente como está. O swap não pode quebrar o veredito
  `cover-error`.

---

## 8. Riscos e mitigação

| Risco | Mitigação |
|---|---|
| `afl-clang-fast` sobre o `<hash>-result.bc` **pré-linkado** pode não injetar cobertura (o pass do AFL roda no IR de entrada, mas o `.bc` é IR "pronto") | Smoke test (§9) confere se `afl-fuzz` **não** aborta com "no instrumentation". Fallback documentado: instrumentar na primeira compilação C→`.bc` com `afl-clang-fast` (`compileCFile()`), preservando o mesmo `.bc` para o KLEE. |
| `abort()` do slicing/`nondet_assume` vs tratamento de crash do AFL | AFL trata `abort` como crash; o replay com `-witness-fuzzed.out` confirma violação real antes do veredito. |
| `__AFL_LOOP` (persistente) acumular entradas em `klee_log.csv` ao longo das iterações | Comportamento já existente no LibFuzzer; `readNonDetLogAsObjects` usa o último vetor. Nenhuma mudança em tacasv1. |
| Atraso na primeira compilação do AFL++ no Docker (rebuild de imagem) | Build com cache por camada; SHA pinado para reprodutibilidade. |

---

## 9. Teste e avaliação

### Smoke test (bloqueante)
1. Compilar a imagem com AFL++ 4.40c; `map2check --nondet-generator afl <prog.c>` em um
   programa `reach_error` conhecido.
2. Confirmar: `afl-fuzz` instrumenta sem "no instrumentation"; acha o bug; emite veredito;
   crash-replay com `-witness-fuzzed.out` confirma; `--seed-exchange` injeta o vetor no KLEE.

### Unit / regressão
- `--nondet-generator afl` e o caminho híbrido default.
- `make-unit-test.sh` com `-DSKIP_AFL_PLUS_PLUS=ON -DSKIP_KLEE=ON` (paridade com o antigo
  `-DSKIP_LIB_FUZZER`).

### Harness tacasv1 (pareado com v15)
- `cover-error` (1087 tarefas), `cover-branches`, Juliet, CASTLE; teste de McNemar para a
  diferença de proporção de cobertas.

---

## 10. Documentação

- `README.md`, `CLAUDE.md`: trocar "LibFuzzer" por "AFL++" e `SKIP_LIB_FUZZER` por
  `SKIP_AFL_PLUS_PLUS`.
- `CHANGELOG.md`: entrada da tacasv1.
- `TODO.md`: remover nota do `SKIP_LIB_FUZZER` self-fuzzing e marcar o fuzzing embarcado
  como AFL++.
- `docs/map2check_migration_plan.md` §3.2: marcar que o Coordenador **permanece no Caller
  C++** (não vira `modules/coordinator/` Python/pybind11); §3.1 concluído.
