# TestComp 2026 Checkpoint — Manual de Reprodução

Este documento explica como reproduzir os testes do checkpoint do Map2Check v8
contra os benchmarks do TestComp 2026.

## Pré-requisitos

- Docker instalado e funcionando
- Imagem `map2check-dev` já construída (via `docker build`)
- Repositório Map2Check clonado e compilado dentro do container

## Passo 1: Build da imagem Docker (se ainda não existir)

```bash
cd /home/guilherme/github/Map2Check
docker build -t map2check-dev -f docker/Dockerfile .
```

## Passo 2: Compilar o Map2Check v8

```bash
docker run --rm -v $(pwd):/workspace map2check-dev bash -c "
    cd /workspace/build && cmake .. && make -j\$(nproc)
"
```

Verificar que os testes unitários passam:

```bash
docker run --rm -v $(pwd):/workspace map2check-dev bash -c "
    cd /workspace/build && ctest --output-on-failure
"
```

## Passo 3: Clonar os sv-benchmarks (se ainda não existir)

```bash
cd test-comp2026/simulation

# Clone com sparse checkout (apenas as categorias necessárias)
git clone --no-checkout --filter=blob:none \
    https://gitlab.com/sosy-lab/benchmarking/sv-benchmarks.git
cd sv-benchmarks
git checkout testcomp26

# Sparse checkout — apenas as categorias de interesse
git sparse-checkout init --cone
git sparse-checkout set c/loops c/loop-acceleration c/loop-crafted \
    c/loop-invgen c/loop-lit c/loop-new c/loop-industry-pattern \
    c/loops-crafted-1 c/loop-invariants c/loop-invariants64 \
    c/loop-simple c/loop-zilu c/nla-digbench c/nla-digbench-scaling \
    c/array-examples c/array-industry-pattern c/array-cav19 \
    c/array-crafted c/array-fpi c/array-lossy c/array-memsafety \
    c/array-multidimensional c/array-patterns c/array-programs \
    c/array-tiling c/array-unsafe \
    c/eca-rers2018 c/psyco c/ntdrivers c/ntdrivers-simplified \
    c/ssh-simplified c/locks c/systemc c/product-lines c/eca-rers2012

cd ../..
```

> **Nota**: Os `.set` files (ex: `Loops.set`, `Arrays.set`, `ControlFlow.set`)
> e os `.prp` property files já estão no root dos benchmarks.

## Passo 4: Preparar o diretório release

O release directory contém todos os binários e bibliotecas necessárias para o
Map2Check executar de forma autocontida:

```bash
docker run --rm -v $(pwd):/workspace map2check-dev bash -c '
    MAP2CHECK_PATH=/workspace/test-comp2026/simulation/release

    # 1. Copiar o binário compilado
    cp /workspace/build/modules/frontend/map2check $MAP2CHECK_PATH/map2check

    # 2. Copiar os plugins dos passes (.so)
    cp /workspace/build/modules/backend/pass/lib*.so $MAP2CHECK_PATH/lib/

    # 3. Copiar as bibliotecas KLEE runtime (.bca)
    mkdir -p $MAP2CHECK_PATH/lib/klee/runtime
    cp /opt/klee/lib/klee/runtime/*.bca $MAP2CHECK_PATH/lib/klee/runtime/

    # 4. Verificar
    echo "=== Estrutura do release ==="
    echo "Binário:   $(file $MAP2CHECK_PATH/map2check | cut -d: -f2)"
    echo "Passes:    $(ls $MAP2CHECK_PATH/lib/*.so | wc -l) plugins"
    echo "BC libs:   $(ls $MAP2CHECK_PATH/lib/*.bc | wc -l) bitcode files"
    echo "KLEE runtime: $(ls $MAP2CHECK_PATH/lib/klee/runtime/*.bca | wc -l) .bca files"
'
```

## Passo 5: Testar um benchmark individual

Para rodar **um único teste** manualmente:

```bash
docker run --rm -v $(pwd):/workspace \
    -e KLEE_RUNTIME_LIBRARY_PATH=/workspace/test-comp2026/simulation/release/lib/klee/runtime \
    map2check-dev bash -c '
        cd /tmp && mkdir -p test && cd test
        /workspace/test-comp2026/simulation/release/map2check \
            --timeout 290 \
            --smt-solver z3 \
            --target-function \
            --target-function-name reach_error \
            /workspace/test-comp2026/simulation/sv-benchmarks/c/loops/array-2.c
    '
```

Resultado esperado: `VERIFICATION FAILED` (com counter-example).

## Passo 6: Rodar o Smoke Test (5 testes por categoria)

```bash
docker run --rm -v $(pwd):/workspace \
    -e SMOKE_TEST=true \
    map2check-dev bash /workspace/test-comp2026/simulation/run_all_inside.sh
```

Resultado: CSV em `test-comp2026/simulation/resultados_de_testes/summary_report_v8.csv`.

## Passo 7: Rodar o Suite Completo

> ⚠️ **Atenção**: O suite completo pode levar **várias horas** dependendo do
> número de benchmarks. Recomenda-se usar `nohup` ou `tmux`.

### Opção A: tmux (recomendado)

```bash
tmux new -s testcomp

docker run --rm -v $(pwd):/workspace \
    map2check-dev bash /workspace/test-comp2026/simulation/run_all_inside.sh

# Ctrl+B, D para detach
# tmux attach -t testcomp para reconectar
```

### Opção B: nohup

```bash
nohup docker run --rm -v $(pwd):/workspace \
    map2check-dev bash /workspace/test-comp2026/simulation/run_all_inside.sh \
    > testcomp_output.log 2>&1 &

# Monitorar
tail -f testcomp_output.log
```

## Estrutura dos resultados

Após a execução, os resultados ficam em:

```
test-comp2026/simulation/resultados_de_testes/
├── summary_report_v8.csv     # CSV com todos os resultados
├── Loops/
│   └── tempos.txt            # Tempos por task
├── Arrays/
│   └── tempos.txt
└── ControlFlow/
    └── tempos.txt
```

### Formato do CSV

```csv
category,file,time_s,result,expected
Loops,array-1.c,60,TRUE,TRUE
Loops,array-2.c,1,FALSE,FALSE
Arrays,data_structures_set_multi_proc_ground-1.i,2,FALSE,FALSE
```

## Variáveis de ambiente

| Variável | Descrição | Padrão |
|:---------|:----------|:-------|
| `SMOKE_TEST` | Se `true`, limita a 5 testes por categoria | `false` |
| `KLEE_RUNTIME_LIBRARY_PATH` | Path para klee-uclibc.bca | auto-detectado |
| `MAP2CHECK_PATH` | Path do release dir | auto-detectado pelo binário |

## Troubleshooting

### `klee: Loading file klee-uclibc.bca failed`
Certifique-se de que `KLEE_RUNTIME_LIBRARY_PATH` está setado ou que os `.bca`
foram copiados para `release/lib/klee/runtime/`.

### `Skipping pass ... due to optnone attribute`
O binário não foi recompilado com o fix `isRequired()`. Recompile com o último
código da branch `feat-update`.

### `symbol multiply defined: main`
Mesmo caso acima — os passes não estão instrumentando (faltando `isRequired()`).

### `Unknown command line argument '-suppress-external-warnings'`
KLEE flags obsoletas. Use o binário compilado com o commit `284aef1ac` ou posterior.
