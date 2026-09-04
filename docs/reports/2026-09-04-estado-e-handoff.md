# Estado do Map2Check e handoff — 2026-09-04

Documento de partida para uma sessão limpa. Consolida o que está implementado, o
que já foi medido, e as três frentes em aberto: **slicing**, **smart seeds** e
**WASM**. Nada aqui pede implementação; é registro do terreno.

---

## 1. Onde o repositório está

| | |
|---|---|
| Branch | `fix/slice-target-stub` — **idêntica a `origin/develop`** |
| PR #65 | **merged** — https://github.com/hbgit/Map2Check/pull/65 |
| Binário de referência | `install_v15/` |
| Relatório da campanha | `docs/Map2Check-Relatorio-v15.docx` (6448 palavras) |

A PR entregou seis correções no motor e a campanha de 13 220 tarefas que as mede.
Os CSVs de campanha **não são versionados** — `.gitignore` cobre
`tests/testcomp/results_*/`, `tests/juliet/results*/` e `tests/castle/results_v*/`.

### Onde os dados vivem (só no disco desta máquina)

```
tests/testcomp/results_v15_ce-control_s{0,1,2}    cover-error, braço controle
tests/testcomp/results_v15_ce-slice_s{0,1,2}      cover-error, braço slice
tests/testcomp/results_v15_cb-control_s{0..5}     cover-branches
tests/juliet/results_v15_{a,b,c,d}                Juliet, 4 shards
tests/castle/results_v15                          CASTLE
```

Baselines anteriores preservados para comparação: `results_v9*` (Juliet/CASTLE),
`results_v12*`. Os resultados do **v14 foram deletados** — aquela campanha rodou o
binário do `--max-time` fracionário e nenhum número dela significa nada.

---

## 2. As seis correções do v15

Todas com causa localizada. Nenhuma é explosão de caminhos.

1. **`--max-time` fracionário matava o KLEE.** O parser de duração rejeita fração,
   sai com 256, zero instruções executadas. Colapsou 464 TN do Juliet para 1.
   Corrigido com `static_cast<unsigned>`.
2. **O slicer apagava o corpo da função-critério.** `reach_error` é onde a fatia
   termina; o sbt-slicer mantém o call-site e descarta a definição. O link nativo do
   LibFuzzer falha com `undefined reference`, nenhum `*-fuzzed.out` é produzido e o
   estágio do fuzzer não faz nada — silenciosamente. Corrigido com um stub `weak`
   religado via `llvm-link`.
3. **O log nunca chegava ao disco.** `stdout` é bufferizado em bloco quando
   redirecionado; um run morto pelo timeout externo perdia o log inteiro, inclusive
   um veredito já impresso. Corrigido com `std::flush` nos cinco níveis.
4. **A compilação do LibFuzzer não tinha limite.** Dois `clang -O2` sobre o módulo
   instrumentado inteiro, sem timeout.
5. **Os motores se dimensionavam pelo orçamento nominal** (0,2× + 0,8× = 100%), sem
   sobrar nada para os dois ciclos de compilar-instrumentar-linkar. Agora medem
   contra `remainingSeconds()`.
6. **FAILED sem prova.** Um veredito de violação agora exige que o vetor seja
   recuperável, checando as mesmas duas fontes que `emitTestSuite` consulta.

**Não-regressão:** CASTLE 217/217 idênticos ao v9. Juliet com TP, FP e FN exatamente
iguais na interseção de 1054 casos, com um TN a mais.

---

## 3. Números da campanha v15

Orçamento de competição, 300 s por tarefa, tudo em `install_v15`.

| bloco | tarefas | resultado |
|---|---|---|
| Test-Comp cover-error | 1087 × 2 braços | controle **470 cobertas = 43,2%**; confirmação de veredito 91,9%; 9 ERROR; **zero NO_SUITE** (eram 41 no v11) |
| Test-Comp cover-branches | 2765 | **cobertura média 53,0%** (referência 46,9%) |
| Juliet | 8060 | TP 1154, TN 3290, FP 120, FN 1145, ERROR 380 |
| CASTLE | 221 | TP 54, TN 44, FN 14, FP 1 |
| **Total** | **13 220** | |

O corpus de cover-error é o **pool inteiro aplicável**, não amostra — a cota de 400
por categoria só limita ECA. Os dois braços rodaram as mesmas 1087 tarefas, então
toda comparação é pareada.

**Cuidado com a palavra "precisão":** no cover-error, 1086 das 1087 tarefas têm bug
alcançável. Não existe verdadeiro negativo nem falso positivo possível, logo o
percentual é **recall**, não precisão.

### Defeitos conhecidos e não corrigidos

- **CWE-369 nunca foi medida.** Declarada no escopo do harness, não pertence a
  nenhum shard. 9 das 10 CWEs foram efetivamente cobertas.
- **380 detecções bem-sucedidas classificadas como falha de ferramenta.** Todas em
  variantes vulneráveis, nenhuma em corrigidas. O classificador lê segfault de
  worker do LibFuzzer como crash da ferramenta; em CWE-121 é o fuzzer achando o bug.
  Bug do harness, não do binário.
- **CWE-416 tem zero TN** com 70 FP.
- **Overflow (CWE-190/191)** com razão TP:FN de ~1:2 e 29% de timeouts.
- **85 tarefas pontuam apesar do veredito ser UNKNOWN** — 18% da pontuação vem de
  casos em que a ferramenta gerou o vetor certo e se declarou incapaz de decidir.

---

## 4. Slicing — o que está implementado e o que **não** está

### De onde vem

**sbt-slicer** + **DG**, do grupo *staticafi* (Masaryk University Brno) — os mesmos
componentes que o **Symbiotic** usa. Instalado pelo `Dockerfile.dev` em
`/opt/sbt-slicer`, resolvido por `Map2Check::slicerBinary()`
(`modules/frontend/utils/tools.hpp:70`), sobrescrevível por `$SBT_SLICER`.

Não vem do FuSeBMC. **O FuSeBMC contribui só a ideia do motor híbrido** (fuzzer +
execução simbólica alternando), não o slicing.

Algoritmicamente é **slicing estático backward sobre SDG**, na tradição
Weiser / Horwitz–Reps–Binkley.

### Onde é chamado

`Caller::sliceWithRespectToTarget()` — `modules/frontend/caller.cpp:179`.
Disparado por `--slice` em `map2check.cpp:519`, **antes** de `callPass()`: fatiar
depois da instrumentação remove a própria instrumentação, e a entrada ainda é
`main` nesse ponto (depois vira `__map2check_main__` e o slicer não a encontraria).

Restrito a reachability. Não há critério a dar quando o objetivo é cobertura ou
propriedade de memória — `--slice` com `--cover-branches` é recusado.

### O comando exato que emitimos

```
timeout -k 10 <budget> /opt/sbt-slicer/bin/sbt-slicer \
    -c reach_error --entry=main -o <hash>-sliced.bc <hash>-compiled.bc
```

`budget = max(5, min(0.2 × timeout, remainingSeconds() − 5))`.

Depois: compila `void __attribute__((weak)) reach_error(void) {}`, faz `llvm-link`
com a fatia, e renomeia por cima do `-compiled.bc`. Se a fatia falhar ou estourar
o orçamento, cai de volta no programa inteiro — que é exatamente o que o controle faz.

### **O buraco: passamos três flags de vinte e sete**

O sbt-slicer expõe um conjunto grande de opções e **não configuramos nenhuma**.
Rodamos inteiramente nos defaults. O Symbiotic, de onde a ferramenta vem, ajusta
várias delas pelo tipo de propriedade. Lista real, extraída do `--help` na imagem
`ghcr.io/hbgit/map2check-dev:latest`:

| opção | default | o que faria por nós |
|---|---|---|
| `--cutoff-diverging` | **true** | **chama `abort()` nos caminhos que podem não alcançar o critério** — ver hipótese abaixo |
| `--pta` | `fi` (flow-insensitive) | `fs` = flow-sensitive, `inv` = com nós de invalidação. Fatia menor, custo maior |
| `--cda` | `standard` (Ferrante) | `ntscd`, `ntscd-ranganath`, `scc` — sensíveis a não-terminação |
| `--cda-icfg` / `--cda-per-inst` | false / por bloco | dependências de controle interprocedurais, por instrução |
| `--undefined-funs` | `read-args` | `rw-any` é conservador; `pure` é agressivo. Muda tudo em programa com muita função externa |
| `--preserved-functions` | — | **manteria o corpo de `reach_error` sem o hack do stub weak** |
| `--remove-slicing-criteria` | false | ligá-lo é o algoritmo de Weiser puro |
| `--memsafety` | false | trata critérios como instruções potencialmente inseguras — é o modo que o Symbiotic usa para memsafety |
| `--allocation-funs` | — | ensina alocadores customizados ao PTA |
| `--statistics` / `--annotate` | false | **quanto foi removido e por quê — hoje só medimos bytes de bitcode** |

### O resultado, e por que ele não conclui nada sobre a técnica

Controle 470 contra slice 412. McNemar χ² = 36,10 (cresceu monotonicamente ao longo
da campanha: 19,56 → 21,12 → 22,72 → 36,10).

Mas **57 das 58 tarefas perdidas estão em ECA**, onde a taxa despenca de 14,5% para
0,2%. Fora de ECA o braço empata ou ganha ligeiramente.

O relatório se recusa deliberadamente a concluir "slicing não funciona". A leitura
honesta é que o número diz mais sobre **a ausência de configuração** do que sobre a
técnica: comparamos os defaults de uma biblioteca contra um controle bem afinado.

### Hipótese principal para a sessão nova — `--cutoff-diverging`

O default é **true**: o slicer injeta `abort()` nos caminhos que podem não alcançar
o critério. E o Map2Check invoca o KLEE com **`--exit-on-error-type=Abort`**
(`caller.cpp`, `stopPolicy`). Ou seja, um `abort()` de caminho divergente é
indistinguível, para o nosso stop policy, de um `abort()` de violação real.

O mesmo vale para o LibFuzzer, que trata `abort()` como crash.

Se isso for o mecanismo, o braço slice está parando cedo em abortos que o próprio
slicer plantou — e ECA, um autômato grande onde a maioria dos caminhos diverge do
critério, é exatamente onde isso doeria mais. Casa com a geografia do dano.

**Ressalva importante e não resolvida:** numa checagem anterior, sobre **um** programa,
alternar `--cutoff-diverging` produziu bitcode idêntico. Isso enfraquece a hipótese
mas não a mata — uma amostra de um, e não no corpus ECA. É a primeira coisa a testar,
não um fato estabelecido.

---

## 5. Smart seeds — o que está implementado e o que **não** está

### O que existe

Flag `--seed-exchange`, **desligada por default**. Troca bidirecional entre os dois
motores, em `modules/frontend/caller.cpp`:

- **KLEE → fuzzer**: `exportKleeVectorsAsSeeds()` (linha 130). Varre `klee-out*/`,
  converte cada `.ktest` em bytes de corpus via `ktestToFuzzerBytes()`, grava em
  `Caller::seedDirectory`. Escrito **depois** do KLEE, porque `cleanGarbage()`
  remove o scratch.
- **fuzzer → KLEE**: `exportFuzzerVectorAsKtest()` (linha 163). Lê `klee_log.csv`,
  escreve **um** `.ktest`, passado como `--seed-file=`.
- **Persistência do corpus**: com `--seed-exchange`, o LibFuzzer recebe um
  *diretório* de corpus. Sem ele, o LibFuzzer mantém o corpus em memória e joga
  fora ao terminar — tudo que descobriu era descartado, todo run.

### Os buracos

- **Nunca foi medido com o binário corrigido.** O único dado que existe vem da
  campanha v14, invalidada. Fechar isso custa dois braços de cover-error, ~10 h.
- **Assimetria: KLEE → fuzzer manda N vetores, fuzzer → KLEE manda um só.** O lado
  do fuzzer lê só `klee_log.csv`, que guarda o último vetor, não o corpus.
  O LibFuzzer produz um diretório inteiro de entradas interessantes que nunca
  chegam ao KLEE. `--seed-file` aceita repetição.
- **Sem priorização.** Todo `.ktest` vira semente, sem ordenar por cobertura nova,
  profundidade ou novidade. O FuSeBMC seleciona sementes por valor.
- **Sem alternância.** O ciclo é uma passagem só: fuzzer, depois KLEE, fim. A ideia
  do FuSeBMC é iterar — cada motor realimenta o outro por várias rodadas dentro do
  orçamento. Os comentários no código já preveem "uma alternância posterior", mas
  o laço não existe.
- **`--use_value_profile=1` está ligado**, o que ajuda; mas nenhum dicionário,
  nenhum `-max_len` derivado do programa, nenhum `-focus_function`.

Mesma forma do slicing: **a técnica está plugada, não sintonizada.**

---

## 6. WASM — proposta contra implementação

### A proposta

`docs/proposta WASM-final.md` — Map2Check-WASM para segurança de IoT/Edge. Quatro
fases: lifter ciente de segurança, modelagem de memória linear, mapeamento de
vulnerabilidades binárias (taint até funções importadas do host), e refinamento do
motor SMT.

### O que existe hoje

`modules/frontend/wasm_lifter.{hpp,cpp}` — 369 linhas. Flag `--wasm`.

Pipeline implementado: **`.wasm` → `wasm2c` → `.c`/`.h` → `clang-16` → `.bc`**,
mais a geração de um wrapper que fornece `main()` chamando o entry point do módulo.
Runtime do WABT procurado em `/opt/wabt-1.0.41/include`, depois `/opt/wabt/include`,
depois `/usr/include`.

Validação existente: `docs/reports/wasm-juliet-validation.csv` — casos do Juliet
compilados para WASM, rodados com `--wasm --memtrack --smt-solver z3`, ~135–180 s
cada, veredito FALSE (bug encontrado) nas variantes vulneráveis.

### A distância entre as duas

Só a **Fase 1 está parcialmente feita**, e na forma mais simples: lifting por
`wasm2c`, sem as anotações que a proposta pede.

- **Sem injeção de metadados** preservando limites de objeto e chamadas de
  importação/exportação do host — que é justamente o ponto de inovação declarado.
- **Fase 2 (memória linear) não existe.** Não há modelagem de ponteiro como offset
  em array contíguo, nem asserção automática `ptr < MEM_SIZE`. O que temos é o
  memtrack genérico rodando sobre o C gerado pelo `wasm2c` — que já embute os
  próprios checks de bound do WABT, e isso confunde a atribuição do resultado.
- **Fase 3 (taint até funções do host) não existe.**
- **Fase 4** — o slicing que ela cita é o mesmo da seção 4, com os mesmos buracos.
- **Preservação semântica nunca foi medida**: a comparação C-direto contra
  WASM-lifted, que é a métrica que provaria que o lifting não degrada a detecção,
  não foi feita. O CSV tem só o lado WASM.

---

## 7. Fio condutor

As três frentes têm a mesma forma, e vale registrar porque muda o que a próxima
sessão deve perguntar:

> Cada uma delas está **integrada mas não configurada**. O sbt-slicer roda nos
> defaults. A troca de sementes acontece uma vez, sem seleção. O lifter de WASM
> traduz sem anotar. Nos três casos, o número medido compara *a configuração
> padrão de uma biblioteca* contra um controle afinado — e essa é uma comparação
> diferente de "a técnica ajuda ou não".

Isso não invalida os resultados do v15; invalida uma leitura específica deles.

---

## 8. Fatos de infraestrutura que custaram tempo

Registrados para não serem redescobertos:

- **`run_isolated` escreve em ARQUIVO, não em pipe.** Netos órfãos do KLEE e do
  LibFuzzer seguram a ponta de escrita para sempre — o harness travou 18 minutos.
- **Amostragem aninhada / prefixo-estável** com manifesto round-robin: quota-40 ⊂
  quota-150 ⊂ quota-400. Run truncado continua estratificado.
- **Sharding por módulo** (`i mod SHARDS == SHARD`), CSV-como-estado para retomar.
- **O veredito do TestCov é a linha `Result:`**, não o código de saída.
- **Nunca use `pkill`/`pgrep`** aqui — mata o próprio shell (saída 144). Mate por PID.
- **Confira containers pelas variáveis `MANIFEST`**, não pelo nome: uma campanha
  inteira rodou com o manifesto errado e o nome do container não denunciava.
- **Rode a varredura de regressão ANTES de armar a campanha.** O bug do `--max-time`
  era intermitente a 300 s (0,875 × 240 = 210, inteiro por acaso) e determinístico a
  60 s. Hoje isso é um portão nos launchers.
