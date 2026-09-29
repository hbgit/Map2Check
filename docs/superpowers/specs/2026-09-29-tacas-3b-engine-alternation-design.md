# tacas 3b — alternância de engines com detecção de estagnação (design)

**Data:** 2026-09-29
**Branch:** `feat/tacas-3b-alternation`, a partir de `feat/tacas-2d-slicing`
**Aprovação:** o usuário delegou as decisões e pediu a abordagem recomendada no
brainstorming de 2026-09-29, a opção A: sinais nativos de estagnação com polling.

## Objetivo

Hoje o híbrido com `--seed-exchange` reparte o orçamento de forma fixa:
- AFL++ com 0,2T;
- KLEE com 0,6T;
- AFL++ com 0,2T.

Isso tem dois custos:
- uma engine estagnada segura o orçamento até o fim da sua cota;
- uma engine que ainda progride é cortada no meio.

O 3b troca de engine **quando a atual estagna**, em rodadas, até:
- achar a violação;
- provar a propriedade;
- acabar o tempo.

## Desenho

### Laço de rodadas (em `main`)

A flag nova é `--alternate-engines`. Ela implica `--seed-exchange` e só vale no híbrido.
O laço alterna AFL++ e KLEE, começando pelo AFL++, com `args.phase = 1, 2, 3, …`:

```
janela(engine, rodada r) = min(restante, base(engine) * 2^(r-1))
    base(AFL++) = 0,1T      base(KLEE) = 0,3T
para cada fase:
    roda a engine com teto = janela e parada por estagnação (abaixo)
    pára o laço se: violação | provedSafe | restante < max(10 s, 0,05T)
```

**Por que as janelas dobram:** a mesma engine volta com mais tempo quando a alternância
não resolveu. Uma engine que estagna cedo devolve o tempo que sobrou na hora.

**Quem alimenta o KLEE:** toda fase AFL++ seguida de uma fase KLEE alimenta o KLEE
(`feedsKleePhase`). O corte é que, se o que resta não comporta outra fase, ela não
alimenta.

**O que o KLEE recebe de semente:**
- o corpus do AFL++, como hoje;
- os `.ktest` da rodada KLEE anterior. Assim o KLEE reconstrói depressa a fronteira
  que já tinha alcançado, em vez de redescobri-la.

**O que o AFL++ recebe:** o `store/afl` já acumula a fila do AFL++ e os vetores do KLEE,
então nada muda.

**Seleção das entradas da fila (`selectQueueEntries`):** passa a **pular as sementes com
que o fuzzer começou** (entradas `,orig:`): são vetores do KLEE ou de rodadas anteriores,
que o KLEE já tem. O `store/ktest` é limpo a cada exportação, porque os `.ktest` da rodada
KLEE anterior (`kleeprev/`) carregam o que ela já explorou. (A primeira versão deste spec
previa um `store/exported.txt`; o efeito é o mesmo com menos estado.)

**Sem limite na troca KLEE → AFL++:** todos os vetores do KLEE vão para o fuzzer.
Completados com zeros depois do fim, alguns caminhos parciais do KLEE chegam ao erro no
dry run do AFL++, e é assim que o híbrido fixo cobre tarefas eca-* que o próprio KLEE deixa
UNKNOWN. Uma primeira versão limitava a exportação aos 64 vetores mais recentes (e o corpus
a 256). Isso custou 5 tarefas eca-* na R19 e foi removido. A calibração ficou barata depois
que o gerador do AFL++ passou a devolver zeros no fim da entrada.

**Binários do AFL++ compilados uma vez por execução** (cache `<hash>.build/`, chaveado
pelo conteúdo dos módulos): em eca-* cada fase do fuzzer gastava ~24 s recompilando.

**Paciência do AFL++ também dobra por rodada:** com o corte fixo de 15 s, os turnos do
fuzzer em eca-* acabavam em ~17 s, onde o híbrido fixo dava 60 s.

### Estagnação

- **AFL++:** `AFL_EXIT_ON_TIME=S`. O próprio fuzzer sai com 0 quando passa S segundos sem
  cobertura nova. O `timeout` externo continua sendo o teto da janela.
- **KLEE:** o frontend lança o KLEE em uma thread (`system`, como hoje, com o PID do
  `timeout` gravado em `klee.pid`) e faz polling do `klee-out-0/run.stats` a cada 2 s,
  lendo `CoveredInstructions` da última linha pela API do SQLite, em modo somente leitura.
  - Se o valor fica S segundos sem subir, o frontend manda SIGINT ao `timeout`, que o
    repassa ao KLEE.
  - O KLEE para de forma limpa: grava os testes dos estados que já terminaram.
  - A fase é marcada como **incompleta** (`gotTimeout`). A execução nunca vira TRUE
    por isso.
- **Valor de S:** `max(10 s, 0,05T)`, que dá 15 s em T = 300, para o AFL++. Para o KLEE,
  S dobra a cada rodada dele: a cobertura de instruções para de subir bem antes de o KLEE
  terminar os caminhos que formam uma prova, e um corte fixo nunca o deixaria chegar lá.
- **Sem SQLite:** o SQLite é uma dependência opcional, via `find_package(SQLite3)`. Sem
  ele, a estagnação do KLEE fica desligada e vale só o teto da janela. O CI, que não
  instala `libsqlite3-dev`, continua compilando.

### Correção necessária: numeração da suíte

`TestSuiteWriter` numera os casos a partir de 1 em cada fase (`testcase-1.xml`, …). Com
mais de uma fase KLEE em Cover-Branches, **uma rodada sobrescreveria a anterior**. O
contador passa a começar depois do maior `testcase-N.xml` que já existir no diretório.

### Revisão (2026-09-29), incorporada

- **Orçamento:** cada fase mede o próprio tempo de preparação (compilar, instrumentar,
  linkar), que nenhuma janela conta. Uma fase só começa se sobrar `S + preparação`, e a
  janela desconta a preparação. O AFL++ guarda 5 s de reserva, como o KLEE.
- **SIGINT:** vai direto ao PID do KLEE, filho do `timeout`. Mandado ao `timeout`, ele
  repassava ao filho e ao grupo, o KLEE recebia dois sinais e perdia o despejo dos estados
  vivos.
- **Suíte de Cover-Branches:** o limite de 50 casos vale para a suíte, não para cada fase,
  e vetores repetidos entre fases são pulados. A suíte começa vazia a cada execução, o que
  evita somar casos de uma execução anterior no mesmo diretório.
- **Flag:** sem `--timeout`, ou com `--nondet-generator`, `--alternate-engines` avisa e
  não se aplica.

## Fora do escopo

- **Transformar o corpus do AFL++ em casos de teste de Cover-Branches.** Hoje a suíte de
  CB sai só dos `.ktest` do KLEE. É um bom candidato, porque o replay tipado já existe,
  mas é uma mudança de outra natureza e fica para depois.
- **Priorização das sementes (3c).**

## Testes

- **Unitários:**
  - `SeedStoreTest`: a seleção pula nomes já exportados;
  - `alternationWindow(engine, rodada, T, restante)`: janelas dobram e respeitam o
    restante;
  - `stagnationSeconds(T)`;
  - `TestSuiteWriter`: o contador continua a partir de um diretório existente.
- **Integração, seções novas:**
  - **Várias fases numa execução só:** um programa em que o AFL++ estagna (guarda de
    igualdade de 32 bits que o CmpLog não resolve), rodado com `--alternate-engines`,
    registra mais de uma fase e termina dentro do orçamento, sem TRUE.
  - **Numeração da suíte:** um diretório com `testcase-1.xml` recebe o próximo caso como
    `testcase-2.xml`.
  - **Estagnação do KLEE:** um programa com laço infinito de estados, a 60 s, mostra o
    aviso "KLEE stagnated" antes do teto e o veredito não é TRUE.

## Medição (R19, a mesma rodada do 2d)

Braços a comparar, todos com o build final:

| braço | flags |
|---|---|
| controle | — |
| seeds | `--seed-exchange` |
| alternate | `--alternate-engines` |

- **Amostras:** Cover-Error em `r15-ce.tsv`; Cover-Branches em `r15-cb.tsv` (seeds e
  alternate).
- **Gate:** TRUE errado = 0.
- **Critério de promoção:** mais tarefas cobertas que seeds em Cover-Error e cobertura
  média de CB não pior.
