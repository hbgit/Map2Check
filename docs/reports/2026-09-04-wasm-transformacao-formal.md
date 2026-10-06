# A transformação WASM → LLVM-IR no Map2Check: formalização

Documento técnico. Descreve, passo a passo e com formalismo, a cadeia de
transformações que leva um binário `.wasm` até o programa instrumentado que o
Map2Check analisa. O objetivo não é só documentar: é tornar **enunciável** o que a
cadeia preserva e o que ela perde, porque essa distinção decide quais resultados
experimentais são atribuíveis ao Map2Check.

Referências de base: a semântica do WebAssembly é a de Haas et al., *Bringing the
Web up to Speed with WebAssembly* (PLDI 2017), e a especificação normativa dela
derivada. O tradutor é o `wasm2c` do WABT 1.0.41, verificado na imagem
`ghcr.io/hbgit/map2check-dev`.

---

## 1. Notação

Seja $\mathbb{B} = \{0,\dots,255\}$ o conjunto dos bytes e $\mathbb{B}^{*}$ as
sequências finitas de bytes. Escrevemos $[a,b)$ para o intervalo inteiro
semiaberto. Para uma função parcial $f$, $\mathrm{dom}(f)$ é seu domínio.

O tamanho de página do WASM é a constante

$$P = 65\,536 .$$

Os tipos de valor são $\mathit{valtype} = \{\mathtt{i32}, \mathtt{i64},
\mathtt{f32}, \mathtt{f64}\}$, e $\mathit{Val}$ é o conjunto dos valores
correspondentes.

---

## 2. Passo 0 — o objeto de entrada

Um módulo WebAssembly é uma tupla

$$M = (\mathit{types}, \mathit{funcs}, \mathit{tables}, \mathit{mems},
\mathit{globals}, \mathit{elem}, \mathit{data}, \mathit{start},
\mathit{imports}, \mathit{exports}).$$

Interessam aqui três componentes:

- $\mathit{mems}$, que na versão 1.0 do WASM tem **cardinalidade no máximo 1**.
  Essa é a hipótese estrutural que sustenta todo o resto: **existe uma única
  memória linear**.
- $\mathit{exports} : \mathit{Name} \rightharpoonup \mathit{ExternIdx}$, as
  funções visíveis de fora do módulo.
- $\mathit{imports}$, as funções que o *host* fornece — em contexto IoT, o
  acesso a sensores, rede e banco.

O **estado de execução** é o par $(s, F)$, com store

$$s = (\mathit{funcs}, \mathit{tables}, \mathit{mems}, \mathit{globals})$$

e frame $F$ contendo os locais e a instância corrente. Uma configuração é
$s; F; e^{*}$, onde $e^{*}$ é a pilha de instruções, e a semântica é dada por uma
relação de redução em passos pequenos

$$s; F; e^{*} \hookrightarrow s'; F'; e'^{*}.$$

O termo distinguido $\mathtt{trap}$ é absorvente: nenhuma regra o reduz.

### 2.1 A memória linear

Uma instância de memória é

$$\mu = (d, \ell), \qquad d : [0,\ell) \to \mathbb{B}, \qquad \ell = n \cdot P$$

para algum $n \in \mathbb{N}$. Ou seja: **um vetor plano de bytes, contíguo,
endereçado por inteiros de 32 bits**, sem qualquer estrutura interna. Não há
noção de objeto, de alocação, nem de fronteira entre variáveis. Esse fato é o
centro de tudo que segue.

A regra de leitura, na forma que nos importa, é

$$
\frac{\quad ea = i + \mathit{offset} \qquad ea + N/8 \le \ell \quad}
     {s; F;\ (\mathtt{i32.const}\ i)\ (t.\mathtt{load}\ N\ \mathit{offset})
      \hookrightarrow s; F;\ (t.\mathtt{const}\ c)}
$$

e, quando a condição lateral falha,

$$
\frac{\quad ea + N/8 > \ell \quad}
     {s; F;\ (\mathtt{i32.const}\ i)\ (t.\mathtt{load}\ N\ \mathit{offset})
      \hookrightarrow s; F;\ \mathtt{trap}} .
$$

Guarde a forma da condição lateral: $ea + N/8 \le \ell$. Ela vai reaparecer
literalmente, como código C, no passo 1.

---

## 3. Passo 1 — $\tau$: `wasm2c`

Definimos

$$\tau : \mathit{Wasm} \longrightarrow \mathit{C}$$

como a função implementada por `wasm2c` (WABT 1.0.41), invocada em
`WasmLifter::runWasm2c` (`modules/frontend/wasm_lifter.cpp:86`) por

```
wasm2c <entrada>.wasm -o <saída>.c
```

produzindo o par $(\texttt{.c}, \texttt{.h})$.

### 3.1 Representação do estado

$\tau$ realiza o store $s$ como uma **struct de instância** $I$. A memória linear
$\mu = (d,\ell)$ vira o registro `wasm_rt_memory_t`, cujos campos relevantes são

$$\texttt{data} \mapsto d, \qquad \texttt{size} \mapsto \ell,$$

com `data` obtido de uma **única** chamada de alocação em
`wasm_rt_allocate_memory`. Registre-se o quantificador: *uma* alocação, para
*toda* a memória do módulo.

Cada função $f$ do módulo vira uma função C

$$\tau(f) : \texttt{(w2c\_}\langle mod\rangle\texttt{*)} \times
\overline{\mathit{Val}} \to \mathit{Val},$$

nomeada por mangling `w2c_<módulo>_<função>`. O primeiro parâmetro é o ponteiro
de instância — é assim que o estado global do WASM vira estado explícito de
primeira classe no C.

### 3.2 A condição lateral vira código

O ponto técnico central: $\tau$ **compila a condição lateral da regra de redução
em uma verificação dinâmica explícita**. No C gerado (verificado na saída real do
WABT 1.0.41):

```c
#define MEM_ADDR(mem, addr, n)  &((mem)->data[addr])

#define RANGE_CHECK(mem, offset, len)              \
  do {                                             \
    uint64_t res;                                  \
    if (UNLIKELY(add_overflow(offset, len, &res))) \
      TRAP(OOB);                                   \
    if (UNLIKELY(res > (mem)->size))               \
      TRAP(OOB);                                   \
  } while (0);

#define TRAP(x) (wasm_rt_trap(WASM_RT_TRAP_##x), 0)
```

Comparando com §2.1: `res > mem->size` é exatamente a negação de
$ea + N/8 \le \ell$, e `TRAP(OOB)` é exatamente $\mathtt{trap}$. A tradução é
literal.

Logo, definindo o predicado

$$\mathrm{Safe}_{\mathrm{wasm}}(\mu, ea, N) \;\equiv\; ea + N/8 \le \ell,$$

o programa C avalia $\mathrm{Safe}_{\mathrm{wasm}}$ *em tempo de execução* antes
de cada acesso, e desvia para $\texttt{wasm\_rt\_trap}$ quando ele é falso.

O conjunto de traps que o WABT distingue é

$$
\mathcal{T} = \{\mathtt{OOB}, \mathtt{INT\_OVERFLOW}, \mathtt{DIV\_BY\_ZERO},
\mathtt{INVALID\_CONVERSION}, \mathtt{UNREACHABLE},
$$
$$
\mathtt{CALL\_INDIRECT}, \mathtt{NULL\_REF},
\mathtt{UNCAUGHT\_EXCEPTION}, \mathtt{UNALIGNED}, \mathtt{EXHAUSTION}\}.
$$

### 3.3 Correção de $\tau$

Seja $\approx$ a relação entre configurações WASM e estados C induzida pela
representação de §3.1 (memória por igualdade byte a byte, globais e locais por
igualdade de valor).

> **Hipótese 1 (correção do `wasm2c`).** Para todo módulo $M$ bem tipado e toda
> execução, $\tau$ é uma **simulação forte** módulo passos administrativos: se
> $s;F;e^{*} \approx \sigma$ e $s;F;e^{*} \hookrightarrow s';F';e'^{*}$, então
> existe $\sigma'$ com $\sigma \to^{+} \sigma'$ e $s';F';e'^{*} \approx \sigma'$;
> e $e^{*} = \mathtt{trap}$ se e somente se $\sigma$ chama $\texttt{wasm\_rt\_trap}$.

Isto é **hipótese, não teorema nosso**. Assumimos a correção do WABT como
assumimos a do Clang. Vale explicitá-la porque toda a soundness da cadeia repousa
sobre ela, e porque ela é falsificável: uma divergência de `wasm2c` apareceria
como discordância entre executar o `.wasm` num runtime e executar o C gerado.

---

## 4. Passo 2 — $\kappa$: Clang para LLVM-IR

$$\kappa : \mathit{C} \longrightarrow \mathit{LLVM}$$

implementada em `WasmLifter::runClang` (linha 102) e `runClangEmitLLVM` (linha 122):

```
clang-16 -c -emit-llvm -I<wasm-rt include> -I<dir do .h>  <arquivo>.c -o <arquivo>.bc
clang-16 -S -emit-llvm -I<wasm-rt include> -I<dir do .h>  <arquivo>.c -o <arquivo>.ll
```

O `.ll` textual não é redundante: ele é a entrada de $\epsilon$, a extração de
metadados de §5.

> **Hipótese 2 (correção do Clang).** $\kappa$ preserva a semântica de programas C
> **livres de comportamento indefinido**.

A ressalva importa mais do que o normal aqui. O C emitido pelo `wasm2c` é gerado,
não escrito, e usa construções deliberadamente próximas do limite — aritmética de
ponteiro sobre `data`, `setjmp`/`sigsetjmp` para o mecanismo de trap, e o
`__seg_gs` do modo *segue* quando habilitado. Não auditamos essa fronteira.

---

## 5. Passo 3 — $\epsilon$: extração do ponto de entrada

$$\epsilon : \mathit{LLVM}_{\text{texto}} \longrightarrow
\mathit{Name} \times \mathcal{P}(\mathit{Name})$$

implementada em `WasmLifter::extractMetadata` (linha 145). Ela varre o `.ll` linha
a linha com a expressão regular

$$\texttt{define.*@w2c\_([A-Za-z0-9\_]+)\textbackslash(}$$

e devolve $(\eta, E)$: o ponto de entrada $\eta$, reconhecido pelo sufixo mangled
`_0x5Fstart`, e $E$ o conjunto das demais funções exportadas.

**Esta é uma análise sintática sobre texto, não sobre o módulo LLVM.** Vale
enunciar suas condições de falha, porque não são hipotéticas:

1. Se nenhuma função casa `_0x5Fstart` — o caso de um módulo *reactor*, sem
   `_start`, que é a forma usual de um handler WASM de IoT — o código cai no
   default `w2c__start` com um aviso. **Esse símbolo pode não existir.**
2. $E$ e $\eta$ são disjuntos por construção, logo um módulo cujo único ponto de
   interesse é uma função exportada (de novo: o caso IoT) tem $\eta$ mal definido.

Isto é: $\epsilon$ é total, mas **não é correta** para a classe de módulos que a
proposta declara como alvo. Registrado aqui como propriedade da implementação
atual, não como defeito a corrigir neste documento.

---

## 6. Passo 4 — $\omega$: o harness

O programa liftado não tem `main`. $\tau$ traduziu funções de módulo; nenhuma
delas é ponto de entrada de um processo. Definimos então

$$\omega : \mathit{Name} \longrightarrow \mathit{LLVM}$$

(`Caller::generateWasmWrapperStatic`, `caller.cpp:567`, chamada em
`map2check.cpp:465`), que sintetiza literalmente

```c
int main() {
    w2c_<mod> instance;
    wasm2c_<mod>_instantiate(&instance, NULL);
    <entrada>(&instance);
    wasm2c_<mod>_free(&instance);
    return 0;
}
```

e o resultado é religado por

$$\Lambda_0 = \texttt{llvm-link}(\kappa(\tau(M)),\ \omega(\eta)).$$

### 6.1 O que $\omega$ quantifica: nada

Formalmente, o módulo denota uma **família** de execuções indexada pelos
argumentos das funções exportadas e pelo comportamento das importações:

$$\llbracket M \rrbracket : \overline{\mathit{Val}} \times \mathit{Host}
\longrightarrow \mathit{Outcome}.$$

O harness atual **não instancia essa família: ele a evita.** A chamada
$\eta(\&I)$ é nulária — a única entrada aceita é o ponteiro de instância, porque
$\eta$ é o `_start`, cuja assinatura no WASM é
$[\;] \rightarrow [\;]$. Nenhum $\nu$ é escolhido porque não há onde colocá-lo.

A consequência é precisa. Escrevendo $\mathit{Reach}(p)$ para o conjunto de
estados alcançáveis de um programa $p$:

$$\mathit{Reach}(\Lambda_0) \;=\;
\bigcup_{h \in \mathit{Host}} \mathit{Reach}\big(\llbracket M \rrbracket(\bot, h)\big)
\;\subseteq\;
\bigcup_{\nu, h} \mathit{Reach}\big(\llbracket M \rrbracket(\nu, h)\big),$$

e a inclusão é **estrita** sempre que o módulo tem alguma função exportada com
aridade positiva. O símbolo $\bot$ não é um valor: é a ausência da chamada.

Duas leituras disso, e as duas importam:

1. **Para um módulo com `_start`** — um programa compilado para WASI, que é o
   formato dos casos do Juliet na CSV de validação — a entrada chega por
   `argv`/stdin *dentro* do `_start`, e o `NonDetPass` pode amarrá-la ali. A
   cadeia funciona. Este é o caso medido até hoje.
2. **Para um módulo *reactor*** — o handler de IoT que a proposta declara como
   alvo, sem `_start`, cujo comportamento inteiro está nas funções exportadas em
   $E$ — o wrapper não chama nada de interesse. Combinado com §5, onde $\epsilon$
   já devolve $\eta$ mal definido para essa classe, o resultado é que **o alvo
   declarado da proposta não é alcançável pela implementação atual**, e não por
   um motivo profundo: falta gerar uma chamada por função exportada, com
   `__VERIFIER_nondet_<t>()` em cada parâmetro.

O tratamento das importações $h$ é o buraco restante: sem modelo de host, as
funções importadas são símbolos externos, e o que se assume sobre elas é o default
de quem estiver rodando, não uma escolha nossa. A Fase 3 da proposta — taint do
dado controlado até a função importada — vive inteiramente aí.

## 7. Passo 5 — $\pi$: instrumentação do Map2Check

Daqui em diante o pipeline é o comum: `map2check.cpp:497` chama
`caller->compileCFile(is_llvmir_in)` com `is_llvmir_in = true`, e a partir daí
$\Lambda_0$ é tratado como qualquer bitcode.

$\pi$ é a transformação de programa dos passes LLVM. Ela é um par
$(\pi_{\text{code}}, \Sigma)$: instrumentação e **estado sombra**. Para o modo
`--memtrack`, $\Sigma$ mantém o conjunto de alocações vivas

$$\Sigma \subseteq \{(\mathit{base}, \mathit{size}, \mathit{alive})\},$$

e $\pi_{\text{code}}$ insere, antes de cada acesso a $(p, n)$, a verificação

$$\mathrm{Safe}_{\mathrm{C}}(\Sigma, p, n) \;\equiv\;
\exists (b,z,\mathit{alive}) \in \Sigma :\;
\mathit{alive} \;\wedge\; b \le p \;\wedge\; p + n \le b + z .$$

**Compare com $\mathrm{Safe}_{\mathrm{wasm}}$ de §3.2.** As duas verificações não
falam da mesma coisa, e a §9 é inteiramente sobre a distância entre elas.

A cadeia completa é

$$\boxed{\;\Lambda \;=\; \pi \;\circ\; \mathrm{link}\big(\omega \circ \epsilon,\ \cdot\big)
\;\circ\; \kappa \;\circ\; \tau \;}$$

seguida do motor de geração de entradas (KLEE ou LibFuzzer) sobre $\Lambda(M)$.

---

## 8. O que a cadeia preserva

> **Proposição 1 (soundness para alcançabilidade de trap).**
> Sob as Hipóteses 1 e 2, se a execução de $\Lambda(M)$ sob a valoração $\nu$
> alcança $\texttt{wasm\_rt\_trap}$, então $\llbracket M \rrbracket(\nu, h)$
> reduz a $\mathtt{trap}$.

*Esboço.* Por Hipótese 2, a execução de $\Lambda_0$ corresponde à do C gerado.
Por Hipótese 1, alcançar $\texttt{wasm\_rt\_trap}$ no C corresponde a alcançar
$\mathtt{trap}$ no WASM. $\pi$ só acrescenta código sobre um estado sombra
disjunto do estado do programa, logo preserva alcançabilidade. $\square$

Isso é real e é útil: **as dez classes de trap de $\mathcal{T}$ — OOB, divisão por
zero, overflow de conversão, `unreachable`, `call_indirect` inválido — são
detectáveis pela cadeia atual, sem nenhum trabalho adicional.** O que
`wasm_rt_trap` faz é abortar, e abort é precisamente o sinal que o modo
reachability do Map2Check já consome.

---

## 9. O que a cadeia **não** preserva

Aqui está a consequência formal que motiva a Fase 2 da proposta.

> **Proposição 2 (incompletude para segurança de memória).**
> Existem $M$ e $\nu$ tais que a execução de $M$ sob $\nu$ **sobrescreve um objeto
> que não é o alvo pretendido** — uma violação genuína de CWE-787 — e ainda assim
> $\Lambda(M)$ sob $\nu$ não viola $\mathrm{Safe}_{\mathrm{C}}$ em nenhum acesso,
> nem alcança $\texttt{wasm\_rt\_trap}$.

*Prova (por construção).* Tome o módulo compilado de

```c
int  buf[4];
char token[16];
int f(int i) { buf[i] = 7; return buf[0]; }
```

com $\nu = (i = 4)$ e $\mathit{token}$ posicionado logo após $\mathit{buf}$ na
memória linear. Sejam $a_{\mathit{buf}}$ e $a_{\mathit{token}}$ seus offsets.

1. **No WASM.** O acesso tem endereço efetivo $ea = a_{\mathit{buf}} + 16$, que é
   $a_{\mathit{token}}$. Como $ea + 4 \le \ell$ — a memória linear tem pelo menos
   uma página, $\ell \ge 65\,536$, e os objetos estão bem no começo — a condição
   lateral de §2.1 **é satisfeita**. A redução não produz $\mathtt{trap}$: produz
   uma escrita bem-definida. O token foi sobrescrito e a execução segue
   normalmente.
2. **No C gerado.** `RANGE_CHECK` testa `res > mem->size`, isto é
   $ea + 4 > \ell$, que é falso. Nenhum trap.
3. **Sob $\pi$.** Por §3.1, `data` é **uma única alocação** cobrindo $[0,\ell)$.
   Logo $\Sigma$ contém o registro $(\texttt{data}, \ell, \mathrm{true})$, e para
   $p = \texttt{data} + ea$, $n = 4$, vale
   $\texttt{data} \le p$ e $p + 4 \le \texttt{data} + \ell$. Portanto
   $\mathrm{Safe}_{\mathrm{C}}(\Sigma, p, 4)$ **é verdadeiro**. Nenhuma violação
   é reportada. $\square$

O ponto não é uma limitação do memtrack. É que **$\tau$ apaga a informação
necessária**: a partição da memória linear em objetos existe no fonte C original,
sobrevive até o compilador para WASM, e é descartada na emissão do módulo, porque
o WASM não tem onde guardá-la. O que chega ao Map2Check é um vetor de bytes com
uma única fronteira — a de fora.

Note ainda o **quantificador invertido**: quanto *maior* a memória linear, *menos*
detectável a violação, porque mais longe fica a única fronteira que sobrou. Um
módulo com uma página tem $\ell = 65\,536$; um overflow de dezesseis bytes está a
sessenta e cinco mil bytes de ser visível.

### 9.1 Consequência para a atribuição dos resultados

Isso muda como ler `docs/reports/wasm-juliet-validation.csv`. Todos os casos ali
retornam `FALSE`, isto é, bug encontrado. Mas a Proposição 1 e a Proposição 2
juntas dizem que há **dois mecanismos possíveis** para esse `FALSE`:

- **(a)** o overflow foi grande o bastante para sair de $[0,\ell)$, o `RANGE_CHECK`
  do WABT disparou, e o que detectamos foi o *check do WABT*; ou
- **(b)** a instrumentação do Map2Check detectou algo por conta própria.

**A CSV não distingue os dois casos**, e a Proposição 2 diz que (b) é impossível
para violação intra-memória. Enquanto essa separação não for feita, a validação
WASM não sustenta a alegação de que *o Map2Check* encontrou os bugs: ela é
consistente com o WABT tê-los encontrado e o Map2Check ter apenas reportado o
abort resultante.

Separar é barato: basta instrumentar `wasm_rt_trap` e registrar qual dos dois
caminhos produziu o abort. É a primeira medição que a frente WASM deveria fazer.

---

## 10. O que a Fase 2 tem de adicionar, formalmente

A proposta pede "modelagem da memória linear". Em termos deste documento, o que
falta é exatamente um objeto: um **mapa de objetos**

$$O : [0,\ell) \rightharpoonup \mathit{ObjID},$$

com o predicado refinado

$$\mathrm{Safe}_{\mathrm{lin}}(O, ea, N) \;\equiv\;
\mathrm{Safe}_{\mathrm{wasm}}(\mu, ea, N)
\;\wedge\;
\exists o \in \mathit{ObjID} :\;
\forall k \in [ea,\, ea+N/8) :\; O(k) = o .$$

Em palavras: além de estar dentro da memória, o acesso inteiro tem de cair
**dentro de um único objeto**. Com $\mathrm{Safe}_{\mathrm{lin}}$ no lugar de
$\mathrm{Safe}_{\mathrm{C}}$, o contraexemplo da Proposição 2 passa a ser
detectado: $O(a_{\mathit{buf}} + 16) = \mathit{token} \ne \mathit{buf}$.

E isso torna preciso o requisito sobre o lifter. $O$ **não é recuperável do
`.wasm`** — foi apagado antes. As três fontes possíveis são:

1. **Seções de nome e de DWARF** do módulo, quando presentes ($\mathtt{clang}$
   `-g` para `wasm32` as emite). Recupera objetos estáticos e o layout de frame.
2. **Reconstrução do alocador**: reconhecer as funções de `malloc`/`free` do
   módulo, quando ele carrega uma libc, e derivar $O$ dinamicamente delas. É o
   análogo, aqui, do `--allocation-funs` do sbt-slicer.
3. **Anotação no momento do lifting**, que é o que a proposta chama de "lifter
   ciente de segurança": $\tau'$ que emita, junto do C, os metadados de fronteira.

As três são trabalho de $\tau$ ou de um passo novo entre $\tau$ e $\kappa$.
**Nenhuma delas é trabalho do backend do Map2Check** — o que confirma a
afirmação de viabilidade da proposta ("não exige reescrever o motor de
verificação"), e ao mesmo tempo localiza o custo real onde ele está.

---

## 11. Resumo da cadeia

| passo | função | implementação | preserva | perde |
|---|---|---|---|---|
| 0 | $M$ | entrada `.wasm` | — | (o layout de objetos já foi perdido na compilação **para** WASM) |
| 1 | $\tau$ | `wasm2c`, WABT 1.0.41 | semântica, traps, bytes da memória | nada além do que já faltava |
| 2 | $\kappa$ | `clang-16 -emit-llvm` | semântica C (módulo UB) | — |
| 3 | $\epsilon$ | regex sobre o `.ll` | ponto de entrada, quando há `_start` | módulos *reactor*, o caso IoT |
| 4 | $\omega$ | wrapper + `llvm-link` | a execução de `_start` | toda função exportada com aridade > 0, e o host |
| 5 | $\pi$ | passes do Map2Check | alcançabilidade | — |

E os dois enunciados que a tabela resume:

- **Proposição 1.** A cadeia é *sound* para as dez classes de trap do WASM. Isso
  já funciona hoje e é um resultado publicável por si.
- **Proposição 2.** A cadeia é *incompleta* para segurança de memória
  intra-sandbox — a classe de vulnerabilidade que a proposta declara como alvo
  principal. A causa é estrutural, não um bug: a memória linear chega como uma
  alocação única, e nenhuma análise sobre ela pode distinguir objetos que já não
  estão lá.
