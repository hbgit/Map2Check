# Crab-LLVM no Map2Check: revisão sistemática e decisão

**Data:** 2026-08-16
**Motivo:** `--add-invariants` é aceito pela CLI e não faz nada. A issue #54 registra
o sintoma; esta revisão estabelece a causa, o que a capacidade valia, e o que fazer
com ela.

O escopo é o que foi pedido: onde o Crab-LLVM atua no Map2Check, se ele suporta a
versão atual do LLVM, que outras ferramentas resolvem o mesmo problema, e qual
alternativa serve melhor à saúde do projeto.

---

## 1. Onde o Crab-LLVM atua

O acoplamento é maior do que uma flag solta. São cinco pontos, em três camadas:

| # | Ponto | Arquivo | O que faz |
|---|---|---|---|
| 1 | Flag da CLI | `modules/frontend/map2check.cpp:356` | `--add-invariants` → `args.invCrabLlvm` |
| 2 | Despacho | `modules/frontend/map2check.cpp:227-245` | Troca `compileCFile()` por `compileToCrabLlvm()` |
| 3 | Invocação | `modules/frontend/caller.cpp:574-656` | Chama `crabllvm.py`, exporta `LD_LIBRARY_PATH`, produz o `.bc` instrumentado |
| 4 | Caminho do binário | `modules/frontend/utils/tools.hpp:37` | `${MAP2CHECK_PATH}/bin/crabllvm/bin/crabllvm.py` |
| 5 | **Reconhecimento do símbolo** | `modules/backend/pass/NonDetPass.cpp:95` | `verifier.assume` → `map2check_crab_assume` |

O ponto 5 é o que importa e o que quase se perde numa leitura rápida. O Crab não
"analisa e reporta": ele **reescreve o bitcode**, inserindo chamadas a
`verifier.assume(inv)` nos pontos onde provou um invariante. O `NonDetPass` reconhece
esse símbolo e o mapeia para `map2check_crab_assume`, que a runtime encadeia assim:

```
verifier.assume            (inserido pelo Crab)
  → map2check_crab_assume  (NonDetPass.cpp:135)
  → map2check_assume       (Map2CheckFunctions.c:46)
  → nondet_assume          (por gerador)
      ├── KLEE:      klee_assume(expr)      → restringe o path condition
      └── LibFuzzer: nondet_cancel()        → pthread_exit se expr for falso
```

Ou seja: a infraestrutura de consumo de invariantes **existe e está correta**. O que
está quebrado é apenas o produtor. Essa assimetria é o argumento mais forte contra
simplesmente apagar a capacidade.

### A invocação atual, literalmente

`caller.cpp:650-654`:

```
crabllvm.py -o <out.bc> -m 64 -g --crab-disable-warnings --disable-lower-gv
            --llvm-pp-loops --crab-promote-assume --crab-inter
            --crab-track=num --crab-add-invariants=block-entry
```

---

## 2. Por que está morto

Duas causas independentes, e as duas precisam ser ditas porque só uma foi registrada
na issue.

**Causa A — a dependência aponta para o mundo LLVM 6.**
`utils/build_crabllvm.py` clona `https://github.com/hbgit/crab-llvm.git`, faz
`git checkout dev-llvm-6.0`, e fixa `llvm_dir = "/llvm/release/llvm600/lib/cmake/llvm/"`.
Também exige `g++-5`. Nada disso existe no ambiente atual (Ubuntu 22.04 + LLVM 16).

O fork `hbgit/crab-llvm` teve seu último push em **2019-03-14**. Sete anos parado.

**Causa B — o aviso de fallback não cobre o caminho que usamos.**
O aviso em `map2check.cpp:239` ("crabllvm is not built — ignoring --add-invariants")
está dentro de `if (!is_llvmir_in)`. Com entrada `.c` ele aparece. Com entrada `.bc`
— que é exatamente como todo o baseline v5 invocou a ferramenta — o bloco inteiro é
pulado e **nem o aviso sai**. A flag é aceita, ignorada, e não deixa rastro.

É por isso que isso passou despercebido por tanto tempo: no modo em que rodamos, o
silêncio é total.

---

## 3. O Crab-LLVM suporta LLVM 16?

**Sim, e o projeto está mais vivo do que o nosso fork sugere.**

O `crab-llvm` foi **renomeado para Clam**. `github.com/seahorn/crab-llvm` e
`github.com/seahorn/clam` retornam metadados idênticos pela API do GitHub (mesmo
`pushed_at`, mesmas 286 estrelas) — é o mesmo repositório sob dois nomes. Não houve
abandono; houve rename, e nosso fork ficou preso ao nome antigo e à versão antiga.

Evidência coletada em 2026-08-16:

| Repositório | Último push | Licença | Situação |
|---|---|---|---|
| `seahorn/clam` (= `seahorn/crab-llvm`) | 2026-08-08 | Apache-2.0 | ativo |
| `seahorn/crab` (a biblioteca) | 2026-08-16 | Apache-2.0 | ativo (hoje) |
| `hbgit/crab-llvm` (nosso fork) | 2019-03-14 | Apache-2.0 | parado |

Branches do Clam: `dev10` … `dev15`, **`dev16`**, `dev17`, `dev18`. O branch `dev16`
tem como último commit `302bf49`, de **2026-08-07**, com a mensagem
`test(llvm16): mark test-mem-global-init-offset as XFAIL` — manutenção ativa
especificamente contra LLVM 16.

O `CMakeLists.txt` do `dev16` fixa `set(CLAM_LLVM_VERSION 16 ...)` e o README manda
`apt-get install clang-16`. É a nossa versão exata, sem adaptação.

**Dependências de sistema** (além do que já temos): `libgmp-dev`, `libmpfr-dev`,
`libflint-dev`. Boost ≥ 1.65 já é requisito nosso.

### O que quebra na migração

Comparei as flags que usamos hoje com a superfície de opções do `dev16`:

| Flag atual | Estado no Clam dev16 |
|---|---|
| `--crab-track=num` | inalterada (`choices=['num','sing-mem','mem']`) |
| `--crab-inter` | inalterada |
| `--crab-promote-assume` | inalterada |
| `--llvm-pp-loops` | inalterada |
| `-o <file>`, `-m 64`, `-g` | inalteradas |
| **`--crab-add-invariants=block-entry`** | **renomeada** → `--crab-opt=add-invariants --crab-opt-invariants-loc=block-entry` |
| `--disable-lower-gv`, `--crab-disable-warnings` | não confirmadas; verificar no build |

Uma flag renomeada, e o símbolo `verifier.assume` **preservado** (aparece em
`lib/Clam/Optimizer/Optimizer.cc:667` e `lib/Transforms/PromoteAssume.cc:51`). Nosso
`NonDetPass.cpp:95` continuaria funcionando sem tocar numa linha.

O Clam ainda ganhou capacidades que não existiam na versão de 2019:
`--crab-opt=dce` e `--crab-opt=replace-with-constants`, que encolhem o IR usando os
invariantes. Isso se sobrepõe em espírito ao ganho que o plano espera do slicing
(F6), a um custo de integração muito menor.

---

## 4. O que os invariantes realmente comprariam — e o risco

Vale separar o que é ganho real do que é entusiasmo.

**Onde ajuda.** Para o KLEE, `klee_assume(inv)` acrescenta fatos ao path condition.
Um invariante é, por definição, verdadeiro em todo estado alcançável — então ele não
elimina estados legítimos. O ganho é de duas naturezas: consultas ao solver ficam mais
fáceis, e ramos infactíveis são cortados mais cedo, antes de gerar estados. Contra os
45,6%/50,7% de UNKNOWN por explosão de caminhos, é uma alavanca legítima. É também
uma forma barata da "poda" que o plano de desenvolvimento pede em H2.

**Onde não ajuda.** Test-Comp não pontua invariantes. Eles só valem pelo que reduzem
de UNKNOWN. Se a redução for pequena, o esforço não se paga — e isso é mensurável
antes de comprometer o caminho default.

**O risco, que é sério e específico.** Um invariante *incorreto* não produz um erro
visível: ele produz um veredito TRUE errado.

- No KLEE, `klee_assume` de algo falso poda um estado alcançável — a violação
  simplesmente deixa de ser encontrada.
- No LibFuzzer é pior: `nondet_assume(0)` chama `nondet_cancel()` → `pthread_exit`.
  A execução morre em silêncio e o fuzzer segue como se aquele input fosse
  inofensivo.

Em ambos os casos a saída é "programa correto", sem aviso. Para uma ferramenta cuja
precisão medida no CASTLE é 98,2%, trocar detecções por silêncio é o pior modo de
falha possível.

Isso não é hipotético no nosso contexto: o baseline v5 mostrou que **887 de 2.525
casos (35%) recebem entrada por funções externas não resolvidas**. Como o Crab modela
essas externas determina se o invariante é sound. Se ele as tratar de forma mais
estreita do que a realidade, o invariante é unsound — e o efeito é invisível.

**Consequência de projeto:** invariantes não podem entrar ligados por default sem um
teste diferencial que prove que nenhuma detecção foi perdida.

---

## 5. Alternativas

| Alternativa | LLVM 16 | Injeta invariante no IR | Licença | Manutenção | Veredito |
|---|---|---|---|---|---|
| **Clam `dev16`** | sim, explícito | sim (`--crab-opt=add-invariants`) | Apache-2.0 | ativa (2026-08-07) | **candidato natural** |
| `hbgit/crab-llvm` (atual) | não (LLVM 6) | sim | Apache-2.0 | parada em 2019 | morto |
| Biblioteca `crab` direto, passe próprio | sim (é lib C++) | teríamos que escrever | Apache-2.0 | ativa (2026-08-16) | reimplementa o Clam; custo alto sem ganho |
| **IKOS** (NASA) | **não** (LLVM 14) | **não** (analisador standalone, grava `output.db`) | NASA OSA 1.3 | ativa | não serve: versão errada, saída errada, licença atípica |
| `llvm.assume` nativo (SCEV/InstCombine) | sim | parcial e muito fraco | Apache-2.0 | n/a | insuficiente para o problema |
| Não fazer invariantes; investir em slicing (`dg`) | sim | n/a | MIT | ativa | é a aposta do Symbiotic — **complementar, não substituto** |

Duas leituras dessa tabela merecem destaque.

O **IKOS** parece atraente pelo tamanho (3.160 estrelas, NASA) e não serve por três
motivos independentes: trava em LLVM 14, é um analisador terminal que escreve um banco
de resultados em vez de reescrever bitcode, e a NASA Open Source Agreement 1.3 não é
uma licença OSI convencional — combiná-la com nosso GPL-2.0 exigiria análise jurídica
que o ganho não justifica.

Escrever nosso próprio passe sobre a **biblioteca `crab`** é tecnicamente viável e
seria reconstruir o Clam. O Clam *é* o front-end LLVM do Crab, mantido pelos mesmos
autores.

---

## 6. Quem mais resolve o mesmo problema

Como o plano de desenvolvimento manda seguir convenções de outras ferramentas e
competições, vale ver como o campo trata "reduzir espaço de estados antes/durante a
exploração":

| Ferramenta | Abordagem | Observação |
|---|---|---|
| **SeaHorn** | usa o próprio Clam | mesma família, mesmos autores (Gurfinkel, Navas) |
| **CPAchecker** | geração própria de invariantes para k-indução | invariantes são cidadãos de primeira classe |
| **ESBMC** | interval analysis + k-indução | invariantes internos, não injetados no IR |
| **2LS** | síntese de invariantes por templates | a tese inteira é invariantes |
| **UAutomizer** | interpretação abstrata dentro de trace abstraction | idem |
| **Symbiotic** | **slicing**, não invariantes | mesmo engine que nós (KLEE); venceu MemSafety |
| **FuSeBMC** | seeds + BMC; sem invariantes | ouro em Cover-Error 2026 |

O recorte relevante: as duas ferramentas mais próximas do Map2Check em arquitetura
—Symbiotic (KLEE + LLVM) e FuSeBMC (fuzzer + BMC) — **não usam invariantes injetados**.
Symbiotic ataca o mesmo gargalo com slicing; FuSeBMC com orquestração e seeds.

Isso não invalida os invariantes, mas reordena a prioridade: se o objetivo é reduzir
UNKNOWN, a evidência competitiva mais forte aponta para slicing (F6) e orquestração
(H2.2/H2.3) antes de invariantes. Os invariantes têm a vantagem de que **metade do
trabalho já está feita** no nosso código (o ponto 5 da seção 1) — o que muda o cálculo
de custo, não o de evidência.

---

## 7. Recomendação

**Duas fases, separadas por um gate de medição.**

### Fase A — parar o silêncio (independe de tudo o mais)

`--add-invariants` nunca mais pode ser aceito e ignorado sem rastro. Corrigir o
alcance do aviso para cobrir também entrada `.bc`, e decidir entre avisar ou falhar.
Recomendo **falhar com código de saída distinto**: numa competição, uma flag que não
faz nada é uma configuração silenciosamente errada, e um aviso em stderr se perde no
log do BenchExec.

Isso é pequeno, testável em CI, e vale para qualquer decisão tomada na Fase B.

### Fase B — Clam `dev16` atrás de `-DENABLE_CLAM=ON`, desligado por default

Justificativa de cada parte:

- **Clam e não outra coisa**: é o mesmo projeto que já usávamos, ativo, na nossa
  versão exata do LLVM, com o símbolo de integração preservado e uma única flag
  renomeada.
- **Atrás de um flag de CMake**: as dependências (`gmp`, `mpfr`, `flint`) e o tempo de
  build não podem entrar no caminho crítico do CI enquanto o ganho não estiver medido.
- **Desligado por default**: por causa da seção 4. Um invariante unsound converte
  detecção em silêncio, e nossa precisão atual é alta demais para arriscar isso sem
  evidência.
- **Gate de promoção**: rodar CASTLE e uma amostra do Juliet com e sem `--add-invariants`.
  Promover a default só se **nenhuma detecção for perdida** e a redução de UNKNOWN for
  material. Se perder detecção, o invariante é unsound e a investigação vira o produto.

### O que rejeitei, e por quê

- **Apagar a capacidade** (`build_crabllvm.py`, `compileToCrabLlvm`, o hook do
  `NonDetPass`): descarta infraestrutura correta e já paga, para eliminar código que
  não custa nada enquanto está desligado. O `verifier.assume → klee_assume` é
  exatamente o mecanismo que qualquer produtor de invariantes futuro precisaria.
- **Clam ligado por default**: sem medição prévia, é apostar precisão contra recall
  no escuro.
- **IKOS**: LLVM 14, saída incompatível, licença atípica.
- **Passe próprio sobre `crab`**: reconstruir o Clam.

### Sequenciamento em relação ao resto

A Fase A pode ir junto com o trabalho de conformidade (H1) — não compete por nada.

A Fase B **não deve** vir antes da emissão de test suite XML nem do gate TestCov. O
plano de desenvolvimento é explícito: conformidade antes de eficácia, porque hoje o
Map2Check não produz o artefato que a competição avalia. Invariantes que reduzem
UNKNOWN em uma ferramenta que pontua zero continuam pontuando zero.

---

## Apêndice: como a evidência foi coletada

Tudo verificável e datado de 2026-08-16:

- `git ls-remote --heads` em `hbgit/crab-llvm`, `seahorn/crab-llvm` e `seahorn/clam`
- API do GitHub (`/repos/...`) para `archived`, `pushed_at`, licença
- Clone raso de `seahorn/clam` no branch `dev16`; leitura de `CMakeLists.txt`,
  `README.md`, `py/clam.py` e `lib/Clam/Optimizer/Optimizer.cc`
- Leitura direta dos cinco pontos de acoplamento no Map2Check, citados por
  arquivo e linha
- Baseline v5 para os números de externas não resolvidas e de precisão
