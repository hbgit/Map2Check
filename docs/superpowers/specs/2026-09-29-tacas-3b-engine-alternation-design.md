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

**Seleção das entradas da fila (`selectQueueEntries`):** passa a **pular as entradas já
exportadas** em rodadas anteriores. Os nomes exportados ficam registrados em
`store/exported.txt`. Sem isso, toda rodada reenviaria as mesmas 64 primeiras.

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
- **Valor de S:** `max(10 s, 0,05T)`, que dá 15 s em T = 300.
- **Sem SQLite:** o SQLite é uma dependência opcional, via `find_package(SQLite3)`. Sem
  ele, a estagnação do KLEE fica desligada e vale só o teto da janela. O CI, que não
  instala `libsqlite3-dev`, continua compilando.

### Correção necessária: numeração da suíte

`TestSuiteWriter` numera os casos a partir de 1 em cada fase (`testcase-1.xml`, …). Com
mais de uma fase KLEE em Cover-Branches, **uma rodada sobrescreveria a anterior**. O
contador passa a começar depois do maior `testcase-N.xml` que já existir no diretório.

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
