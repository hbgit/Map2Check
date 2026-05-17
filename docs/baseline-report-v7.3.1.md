# Map2Check v7.3.1 — Relatório de Baseline Pré-Migração

**Data:** 2026-05-17  
**Autor:** Guilherme Bernardo  
**Objetivo:** Documentar o estado operacional da versão atual antes da migração para Map2Check 2.0  
**Versão testada:** `v7.3.1-Flock` (Wed Nov 27 20:38:14 UTC 2019)

---

## 1. Resumo Executivo

Este relatório documenta o processo completo de build, instalação e execução de testes do Map2Check v7.3.1 em seu ambiente original (Docker com LLVM 6.0). O objetivo é estabelecer uma baseline de referência antes de iniciar a migração para a nova stack (LLVM 16, KLEE 3.2, AFL++, DG Library).

| Suite | Resultado | Taxa |
|:------|:----------|:-----|
| **Build do Release** | 50/50 targets | ✅ 100% |
| **Testes Unitários** | 7/7 | ✅ 100% |
| **Testes de Regressão** | 5/9 | ⚠️ 55.6% |

---

## 2. Ambiente de Build

### 2.1 Cadeia de Imagens Docker

O Map2Check utiliza uma cadeia de 3 camadas Docker:

| Camada | Imagem | Base | Conteúdo Principal |
|:-------|:-------|:-----|:-------------------|
| 1 | `herberthb/dev-llvm_6.0:first` | Ubuntu 16.04 | LLVM 6.0 compilado do source |
| 2 | `herberthb/base-image-map2check:v2` | Camada 1 | KLEE, Z3 4.8.4, STP 2.1.2, MiniSat, LibFuzzer, MetaSMT, Crab-LLVM |
| 3 | `hrocha/mapdevel` | Camada 2 | Ambiente de desenvolvimento + Clang 6.0 pre-built |

### 2.2 Dependências Efetivas (imagem Docker)

| Componente | Versão no cmake | Versão na Imagem Docker | Observação |
|:-----------|:----------------|:------------------------|:-----------|
| LLVM/Clang | 6.0 | 6.0.0 | Compilado do source na camada 1 |
| KLEE | fork `map2check_svcomp2018` | master (oficial) | Na imagem base, usa KLEE oficial |
| Z3 | `z3-4.4.1` (tag cmake) | **4.8.4** (na imagem) | cmake é para build from-scratch; imagem usa 4.8.4 |
| STP | 2.1.2 | 2.1.2 | Consistente |
| MiniSat | releases/2.2.1 | releases/2.2.1 | Consistente |
| LibFuzzer | SVN trunk | SVN trunk | SVN deprecado, mas funcional na imagem |
| MetaSMT | v4.rc2 (fork hbgit) | v4.rc2 | Boolector 2.2.0, Lingeling, Yices 2.5.1 |
| Crab-LLVM | dev-llvm-6.0 (fork hbgit) | dev-llvm-6.0 | Invariantes indutivas |
| Boost | sistema | 1.58.0 | program_options, system, filesystem |
| C++ Standard | C++11 | C++11 | — |
| CMake | ≥ 3.5 | 3.5.1 | — |
| klee-uclibc | klee_0_9_29 | klee_0_9_29 | Runtime C para KLEE |

---

## 3. Processo de Build

### 3.1 Passo 1 — Inicialização dos Submodules

```bash
git submodule update --init --recursive
```

3 submodules inicializados:
- `utils/base_image_map2check` — Dockerfile da imagem base
- `utils/benchexecrun` — Dockerfile do runner de benchmarks
- `utils/dev-llvm_6.0` — Dockerfile do ambiente LLVM 6.0

### 3.2 Passo 2 — Pull da Imagem Base

```bash
docker pull herberthb/base-image-map2check:v2
```

Imagem baixada com sucesso do Docker Hub (~3.4 GB).

### 3.3 Passo 3 — Build da Imagem de Desenvolvimento

```bash
docker build -t hrocha/mapdevel --no-cache -f Dockerfile .
```

Resultado: ✅ Sucesso. Imagem construída com Clang 6.0 pre-built.

### 3.4 Passo 4 — Compilação do Release

```bash
docker run --rm \
  -v $(pwd):/home/map2check/devel_tool/mygitclone:Z \
  --user $(id -u):$(id -g) hrocha/mapdevel \
  /bin/bash -c "cd /home/map2check/devel_tool/mygitclone; ./make-release.sh"
```

Resultado: ✅ **50/50 targets compilados com sucesso.**

#### Artefatos Gerados

**Passes LLVM (shared libraries):**

| Pass | Arquivo | Função |
|:-----|:--------|:-------|
| NonDetPass | `libNonDetPass.so` | Instrumentação de funções não-determinísticas |
| MemoryTrackPass | `libMemoryTrackPass.so` | Rastreamento de operações de memória |
| Map2CheckLibrary | `libMap2CheckLibrary.so` | Biblioteca auxiliar de runtime |
| TargetPass | `libTargetPass.so` | Busca por `__VERIFIER_error()` |
| TrackBasicBlockPass | `libTrackBasicBlockPass.so` | Rastreamento de blocos básicos |
| GenerateAutomataTruePass | `libGenerateAutomataTruePass.so` | Geração de autômatos para witness TRUE |
| AssertPass | `libAssertPass.so` | Instrumentação de asserts |
| OverflowPass | `libOverflowPass.so` | Detecção de overflow |
| LoopPredAssumePass | `libLoopPredAssumePass.so` | Predicados de loop e assume |

**Binários instalados:** `map2check`, `clang`, `opt`, `llvm-link`, `klee`, `klee-replay`, `klee-stats`, `ktest-tool`, `ld`, `crabllvm/`

**Runtime libraries:** 21 bitcode files (`.bc`) para instrumentação, Z3, STP, MiniSat, MetaSMT, Boolector, Lingeling, Yices.

---

## 4. Testes Unitários — 7/7 (100%)

### 4.1 Correção Necessária: GoogleTest

O GoogleTest renomeou a branch padrão de `master` para `main`, e as versões recentes exigem CMake ≥ 3.16 (o container tem 3.5.1).

**Arquivo:** `cmake/CMakeLists.txt.googletest`

```diff
-  GIT_TAG           master
+  GIT_TAG           release-1.12.1
```

> `release-1.12.1` é a última versão do GoogleTest compatível com CMake < 3.13.

### 4.2 Execução

```bash
docker run --rm \
  -v $(pwd):/home/map2check/devel_tool/mygitclone:Z \
  --user $(id -u):$(id -g) hrocha/mapdevel \
  /bin/bash -c "cd /home/map2check/devel_tool/mygitclone; ./make-unit-test.sh"
```

### 4.3 Resultados

```
Test project /home/map2check/devel_tool/mygitclone/build
    Start 1: HelloGTest
1/7 Test #1: HelloGTest .......................   Passed    0.00 sec
    Start 2: AllocationLogTest
2/7 Test #2: AllocationLogTest ................   Passed    0.00 sec
    Start 3: HeapLogTest
3/7 Test #3: HeapLogTest ......................   Passed    0.00 sec
    Start 4: ContainerReallocTest
4/7 Test #4: ContainerReallocTest .............   Passed    0.00 sec
    Start 5: BTreeTest
5/7 Test #5: BTreeTest ........................   Passed    0.15 sec
    Start 6: ContainerBTreeTest
6/7 Test #6: ContainerBTreeTest ...............   Passed    0.16 sec
    Start 7: MemTrackTest
7/7 Test #7: MemTrackTest .....................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 7

Total Test time (real) =   0.33 sec
```

---

## 5. Testes de Regressão — 5/9 (55.6%)

### 5.1 Correção Necessária: BenchExec

O BenchExec runner precisou de duas correções para funcionar em ambientes modernos:

**Arquivo:** `utils/benchexecrun/Dockerfile`

```diff
- FROM ubuntu:18.04
+ FROM ubuntu:20.04

- RUN apt-get install -y sudo \
+ RUN DEBIAN_FRONTEND=noninteractive apt-get install -y sudo \
      python3-pip \
      ...
-     python-minimal \
      libc6-dev

- RUN pip3 install benchexec
+ RUN pip3 install benchexec==3.18

- RUN test -e /usr/local/lib/python3.6/dist-packages/benchexec/tools/map2check.py ...
+ RUN PYTHON_SITE=$(python3 -c "import site; print(site.getsitepackages()[0])") && \
+     test -e ${PYTHON_SITE}/benchexec/tools/map2check.py ...
```

**Motivos:**
- BenchExec 3.16+ exige Python 3.7+ (Ubuntu 18.04 tem apenas 3.6)
- BenchExec 3.18+ tem suporte a cgroups v2 (necessário para kernels modernos)
- `python-minimal` foi removido no Ubuntu 20.04

### 5.2 Nota sobre BenchExec e WSL2

O BenchExec não consegue rodar dentro de containers Docker em WSL2 porque os cgroups v2 não são delegados para dentro dos containers. Os testes de regressão foram executados **diretamente com o binário `map2check`** dentro do container de desenvolvimento.

### 5.3 Execução

Os testes foram executados diretamente dentro do container `hrocha/mapdevel`, invocando o binário `map2check` com timeout de 30 segundos por caso de teste:

```bash
docker run --rm \
  -v $(pwd):/home/map2check/devel_tool/mygitclone:Z \
  hrocha/mapdevel /bin/bash -c '
    cd /home/map2check/devel_tool/mygitclone/release
    timeout 30s ./map2check --timeout 25 --memtrack <arquivo.c>
  '
```

### 5.4 Resultados

| # | Modo | Arquivo | Esperado | Obtido | Status |
|:--|:-----|:--------|:---------|:-------|:-------|
| 1 | `--memtrack` | `test-0158_1-1.c` | TRUE | `std::bad_alloc` | ❌ FAIL |
| 2 | `--memtrack` | `test-0158_1-2.c` | TRUE | `std::bad_alloc` | ❌ FAIL |
| 3 | `--memtrack` | `test-memsafety-alloca-1.c` | TRUE | `std::bad_alloc` | ❌ FAIL |
| 4 | `--memtrack` | `add_last_unsafe.c` | FALSE | FALSE | ✅ PASS |
| 5 | `--memtrack` | `lis_unsafe.c` | FALSE | TRUE | ❌ FAIL |
| 6 | `--target-function` | `check_if_bug.c` | VERIFICATION | VERIFICATION | ✅ PASS |
| 7 | `--check-asserts` | `check_if_bug.c` | VERIFICATION | VERIFICATION | ✅ PASS |
| 8 | `--memtrack` | `standard_two_index_04.c` | VERIFICATION | VERIFICATION | ✅ PASS |
| 9 | `--memtrack` | `standard_two_index_05.c` | VERIFICATION | VERIFICATION | ✅ PASS |

### 5.5 Diagnóstico das Falhas

#### Falha 1-3: `std::bad_alloc` (Esgotamento de Memória)

Os programas de teste envolvem manipulação complexa de heap dinâmica (listas encadeadas, alocações recursivas). O KLEE esgota a memória disponível antes de concluir a análise.

```
Adopting z3 solver...
Started Map2Check
std::bad_alloc
```

**Causa raiz:** O KLEE analisa **todo o código** do programa, incluindo trechos irrelevantes para a propriedade verificada. Sem slicing estático, o espaço de estados explode exponencialmente.

**Solução no Map2Check 2.0:** Integração com a **DG Library** para slicing estático — remover código morto antes da análise simbólica, reduzindo drasticamente o footprint de memória.

#### Falha 5: Falso Negativo (`lis_unsafe.c`)

O programa contém um erro de memória (`out of bound pointer`) que o KLEE **detectou**, mas que o pipeline do Map2Check **não propagou** como falha de verificação.

Sequência observada:
1. **LibFuzzer** roda 8 workers, mas não encontra crash (exit code 31744)
2. **KLEE** identifica erros:
   ```
   KLEE: ERROR: concretized symbolic size
   KLEE: ERROR: memory error: out of bound pointer
   ```
3. **Map2Check** reporta incorretamente:
   ```
   VERIFICATION SUCCEEDED  ← Deveria ser FAILED
   ```

**Causa raiz:** Os motores (LibFuzzer e KLEE) operam em fases distintas, sem compartilhar aprendizado. Os achados do KLEE não são corretamente processados pelo frontend.

**Solução no Map2Check 2.0:** Implementação do **Coordenador Central** com **Smart Seeds** — orquestração bidirecional entre AFL++ e KLEE, com propagação correta de resultados.

---

## 6. Validação do Binário

O binário `map2check` está funcional com todas as opções de verificação disponíveis:

```
> > > 	  v7.3.1-Flock : Wed Nov 27 20:38:14 UTC 2019 	 < < <

Usage: map2check [options] file.[i|c|bc]

Options:
  --help                 show help
  --version              prints map2check version
  --debug                debug mode
  --input-file arg       specifies the files
  --smt-solver arg (=z3) specifies the smt-solver (stp, z3, btor, yices2)
  --timeout arg          timeout for map2check execution
  --target-function      searches for __VERIFIER_error is reachable
  --generate-testcase    creates c program with fail testcase (experimental)
  --memtrack             check for memory errors
  --print-counter        print counterexample
  --memcleanup-property  analyze program for memcleanup errors
  --check-overflow       analyze program for overflow failures
  --check-asserts        analyze program and verify assert failures
  --add-invariants       adding program invariants adopting Crab-LLVM
  --generate-witness     generates witness file
  --expected-result arg  specifies type of violation expected
  --btree                uses btree structure (experimental)
```

---

## 7. Arquivos Modificados

| Arquivo | Modificação | Motivo |
|:--------|:------------|:-------|
| `cmake/CMakeLists.txt.googletest` | `GIT_TAG master` → `release-1.12.1` | Google renomeou branch; versões novas exigem CMake ≥ 3.16 |
| `utils/benchexecrun/Dockerfile` | Ubuntu 18.04 → 20.04; benchexec==3.18; paths Python atualizados | Python 3.6 incompatível com BenchExec moderno; suporte cgroupsv2 |

---

## 8. Conclusão

Os resultados deste baseline **confirmam empiricamente os problemas arquiteturais** descritos no pré-projeto do Map2Check 2.0:

1. **Explosão de estados/memória** — Testes 1-3 falham por `std::bad_alloc` porque o KLEE analisa código irrelevante. A integração com **DG Library** para slicing estático é crítica.

2. **Falsos negativos por falta de orquestração** — Teste 5 mostra que o KLEE detecta o erro mas o pipeline não o propaga. O **Coordenador Central** com feedback loop entre motores é necessário.

3. **Obsolescência da infraestrutura** — GoogleTest, BenchExec e Ubuntu precisaram de correções para funcionar em ambientes modernos. A migração para **LLVM 16, Ubuntu 22.04, C++17** é urgente.

O baseline está estabelecido. A migração pode prosseguir com confiança de que temos uma referência quantitativa para medir o progresso.
