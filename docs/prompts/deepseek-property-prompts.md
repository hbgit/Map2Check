# Prompts de verificação Map2Check ↔ DeepSeek

Oito prompts, um por **veredicto de violação** que o Map2Check é capaz de
escrever. O recorte é por veredicto, e não por flag, porque `FALSE-FREE`,
`FALSE-DEREF` e `FALSE-MEMTRACK` compartilham `--memtrack` mas são defeitos
distintos — agrupá-los mediria três coisas com uma nota só.

Referência das propriedades: [`../Map2Check-Propriedades-e-CWEs.pdf`](../Map2Check-Propriedades-e-CWEs.pdf).

## Regras de simetria

Para que a comparação meça capacidade de detecção, e não desenho de
experimento:

1. **Ambos os lados sabem qual propriedade checar.** O Map2Check recebe a
   propriedade pela flag; o DeepSeek recebe pelo prompt. Um prompt genérico
   ("ache bugs neste código") daria ao Map2Check uma vantagem artificial.
2. **O prompt não revela se há defeito.** Toda formulação diz explicitamente
   que o programa pode ou não violar a propriedade. Sem isso o modelo é
   empurrado para `FALSE`, e a taxa de falso-positivo deixa de ser medível.
3. **O vocabulário de saída é o do Map2Check.** Mesmos tokens, mesmo
   `UNKNOWN` disponível dos dois lados.
4. **A definição da propriedade é a do SV-COMP / CWE, não a do Map2Check.**
   A verdade de referência são os rótulos do Juliet e do CASTLE. Onde o
   Map2Check é estruturalmente cego (ver "Assimetrias conhecidas"), isso é um
   resultado a registrar, não algo a esconder no prompt.
5. **Um prompt por programa.** Sem histórico entre programas, para que um
   caso não vaze no seguinte.

Os prompts estão em inglês: os benchmarks, os identificadores e as definições
do SV-COMP são em inglês, e traduzir a definição da propriedade introduz
ambiguidade que não existe no original. O relatório em volta segue em
português.

## Assimetrias conhecidas (registrar, não corrigir no prompt)

| Situação | Efeito na comparação |
|:---|:---|
| `Shl`/`LShr`/`AShr`/`And`/`Or`/`Xor` não são instrumentados | Overflow por deslocamento: o Map2Check responde `TRUE` por construção. Contabilizar à parte. |
| Aritmética sem sinal fora da propriedade `no-overflow` | Vale para os dois lados — o prompt de overflow exclui `unsigned` explicitamente. |
| Ponto flutuante é *no-op* | Idem: excluído no prompt. |
| Sob `--wasm`, acesso fora dos limites vira `FALSE-OVERFLOW` | Só afeta a trilha WASM. Aceitar `FALSE-OVERFLOW` como acerto de *bounds* ali. |
| `--generate-witness` produz *witness* de corretude para `ASSERT`, `FALSE-MEMCLEANUP` e `FALSE-DIVBYZERO` | Pontuar pelo stdout, nunca por validação de *witness*, nessas três trilhas. |
| Orçamento | O Map2Check tem *timeout* explícito; o DeepSeek não. Registrar o *timeout* usado e o custo em tokens. |

Duas armadilhas do lado do Map2Check que corrompem a coleta em silêncio:

- **O veredicto de overflow é gravado como `OVERFLOW`, não `FALSE-OVERFLOW`.**
  `PropertyGenerator.c:97` escreve essa string, e é ela que `tools.cpp:228` lê.
  Um coletor que faça `grep FALSE-OVERFLOW` em `map2check_property` pontua
  zero na trilha P5 inteira. O token `FALSE-OVERFLOW` do prompt é o nome do
  *enum* e vale para o lado do DeepSeek; **não** use o mesmo padrão para os
  dois lados.
- **Use apenas `--smt-solver z3` (padrão) ou `stp`.** `btor` e `yices2` passam
  na validação da CLI e depois são entregues ao KLEE como
  `--solver-backend=metasmt`, que a imagem de referência não compila.

---

## System prompt (compartilhado pelos oito)

````text
You are a program-verification engine. You receive one C translation unit and
one safety property. You decide whether some execution of the program can
violate that property, and you answer in a fixed format.

Assume the SV-COMP execution model:
  - `main` is the entry point.
  - `__VERIFIER_nondet_<type>()` returns an arbitrary value of that type; every
    call is independent. Reason over all possible return values.
  - `__VERIFIER_assume(c)` and `assume_abort_if_not(c)` discard executions in
    which `c` is false. Executions they discard cannot violate anything.
  - The machine model is 64-bit little-endian (LP64), signed integers are
    two's complement, `int` is 32 bits, `long` and pointers are 64 bits.
  - Library functions behave as the C standard specifies. `malloc` may return
    NULL.

Judge ONLY the property you are given. A program may contain other defects;
they are irrelevant and must not change your verdict.

The program may or may not violate the property. Both outcomes are equally
likely a priori. Do not assume a defect is present because you were asked to
look for one.

Answer with exactly three lines and nothing else:

VERDICT: <one token from the allowed list you are given>
LINE: <1-based line number of the violating operation, or - if not applicable>
REASON: <one sentence, at most 30 words>

`UNKNOWN` is a legitimate answer and is scored separately from a wrong one.
Use it when you cannot decide. Never answer with prose outside the three lines,
never hedge inside VERDICT, never emit more than one VERDICT line.
````

---

## P1 — `FALSE-DEREF` (dereferência inválida)

- **Map2Check:** `map2check --memtrack --nondet-generator symex <file.c>`
- **CWEs no benchmark:** 121, 122, 125, 476, 787, 822, 843 — e 416, que
  também aparece em P2 (`use-after-free` é a dereferência de memória liberada)
- **CWEs da propriedade (referência, fora dos benchmarks):** 124, 126, 127, 824
- **Tokens permitidos:** `FALSE-DEREF`, `TRUE`, `UNKNOWN`

````text
PROPERTY — valid-deref

Every memory dereference the program performs must be valid: the address must
point inside a live object, at an offset within that object's bounds, for the
full width of the access.

A violation is any of:
  - reading or writing through a NULL pointer;
  - reading or writing outside the bounds of an allocated object (heap, stack
    or global), including off-by-one past the end;
  - dereferencing a pointer to memory that has already been freed;
  - dereferencing a pointer to a local variable whose scope has ended;
  - dereferencing an uninitialised or otherwise indeterminate pointer.

Not a violation for this property:
  - failing to free memory (that is a different property);
  - passing an invalid pointer to free() without dereferencing it;
  - integer overflow, division by zero, or a failing assertion.

Answer FALSE-DEREF if some execution performs an invalid dereference.
Answer TRUE if no execution can.
Answer UNKNOWN if you cannot decide.

PROGRAM:
```c
{{PROGRAM}}
```
````

---

## P2 — `FALSE-FREE` (liberação inválida)

- **Map2Check:** `map2check --memtrack --nondet-generator symex <file.c>`
- **CWEs no benchmark:** 415, 416, 761
- **CWEs da propriedade (referência, fora dos benchmarks):** 590
- **Tokens permitidos:** `FALSE-FREE`, `TRUE`, `UNKNOWN`

````text
PROPERTY — valid-free

Every call to free() (and to realloc() with a non-null pointer) must receive
either NULL or a pointer that was returned by a previous allocation and has not
been freed since — and it must point at the START of that allocation.

A violation is any of:
  - freeing the same pointer twice;
  - freeing a pointer that no allocation returned (a stack address, a global, a
    string literal, an arbitrary integer cast to a pointer);
  - freeing a pointer that has been advanced away from the start of its block
    (e.g. `p++` then `free(p)`);
  - freeing a pointer whose value is indeterminate.

`free(NULL)` is well defined and is NOT a violation.

Not a violation for this property:
  - dereferencing a freed pointer without freeing it again (that is
    valid-deref);
  - never freeing an allocation (that is valid-memtrack).

Answer FALSE-FREE if some execution performs an invalid free.
Answer TRUE if no execution can.
Answer UNKNOWN if you cannot decide.

PROGRAM:
```c
{{PROGRAM}}
```
````

---

## P3 — `FALSE-MEMTRACK` (memória alocada e perdida)

- **Map2Check:** `map2check --memtrack --nondet-generator symex <file.c>`
- **CWEs no benchmark:** *nenhum* — os dois runners roteiam CWE-401 para
  `--memcleanup-property` (P4), então esta trilha não tem caso rotulado no
  repositório e precisa de um corpus próprio
- **CWEs da propriedade (referência):** 401, 771, 772
- **Tokens permitidos:** `FALSE-MEMTRACK`, `TRUE`, `UNKNOWN`

````text
PROPERTY — valid-memtrack

The program must not lose track of allocated memory. A block is lost when the
last pointer to it is overwritten, goes out of scope, or is otherwise destroyed
while the block is still allocated — after that no execution could free it even
in principle.

A violation is any of:
  - overwriting the only pointer to a block that is still allocated;
  - returning from a function while the only pointer to a block is a local
    variable of that function;
  - losing a block on an early-return or error path while the normal path frees
    it.

Not a violation for this property:
  - a block that is still reachable through some live pointer when main
    returns — that is valid-memcleanup, a different property;
  - freeing a block twice, or dereferencing it after free.

Note the distinction carefully: this property is about UNREACHABLE memory, not
about unfreed memory.

Answer FALSE-MEMTRACK if some execution makes an allocated block unreachable.
Answer TRUE if no execution can.
Answer UNKNOWN if you cannot decide.

PROGRAM:
```c
{{PROGRAM}}
```
````

---

## P4 — `FALSE-MEMCLEANUP` (memória viva no fim do `main`)

- **Map2Check:** `map2check --memcleanup-property --nondet-generator symex <file.c>`
- **CWEs no benchmark:** 401
- **CWEs da propriedade (referência):** 401
- **Tokens permitidos:** `FALSE-MEMCLEANUP`, `TRUE`, `UNKNOWN`

````text
PROPERTY — valid-memcleanup

All memory allocated by the program must have been deallocated by the time main
returns (or exit() is called). Whether a pointer to the block still exists is
irrelevant: the question is only whether the block was freed.

A violation is any execution that terminates normally with at least one
allocated, unfreed block.

Not a violation for this property:
  - an unfreed block in an execution that does not terminate;
  - freeing a block twice, dereferencing it after free, or making it
    unreachable while still freeing it later.

Note the distinction from valid-memtrack: a block kept in a global pointer and
never freed satisfies valid-memtrack (it is still reachable) but violates
valid-memcleanup.

Answer FALSE-MEMCLEANUP if some terminating execution leaves memory allocated.
Answer TRUE if every terminating execution frees everything.
Answer UNKNOWN if you cannot decide.

PROGRAM:
```c
{{PROGRAM}}
```
````

---

## P5 — `FALSE-OVERFLOW` (overflow de inteiro com sinal)

- **Map2Check:** `map2check --check-overflow --nondet-generator symex <file.c>`
- **CWEs no benchmark:** 190, 191
- **CWEs da propriedade (referência, fora dos benchmarks):** 194, 197
- **Tokens permitidos:** `FALSE-OVERFLOW`, `TRUE`, `UNKNOWN`

````text
PROPERTY — no-overflow

No arithmetic operation on a SIGNED integer type may produce a result outside
the range of the operation's resulting type. Evaluate the property on the type
that C's usual arithmetic conversions give the operation, not on the type of
the variable the result is stored into.

A violation is any of:
  - a signed addition, subtraction or multiplication whose mathematical result
    falls outside the range of its resulting type;
  - the division or remainder `INT_MIN / -1` and `INT_MIN % -1`.

NOT a violation for this property:
  - arithmetic on unsigned types — unsigned wraparound is defined behaviour and
    is explicitly outside this property;
  - floating-point arithmetic of any kind;
  - conversions and truncations that merely discard bits (e.g. assigning a
    long to an int);
  - division or remainder by zero — that is a different property;
  - out-of-bounds accesses, invalid frees, or failing assertions.

Remember that operands narrower than int are promoted to int before the
operation, so arithmetic on two chars is evaluated in int and rarely overflows.

Answer FALSE-OVERFLOW if some execution overflows a signed integer operation.
Answer TRUE if no execution can.
Answer UNKNOWN if you cannot decide.

PROGRAM:
```c
{{PROGRAM}}
```
````

---

## P6 — `FALSE-DIVBYZERO` (divisão ou resto por zero)

- **Map2Check:** `map2check --check-overflow --nondet-generator symex <file.c>`
  — não há flag própria; a instrumentação de `SDiv`/`SRem` é a mesma.
- **CWEs no benchmark:** 369
- **CWEs da propriedade (referência):** 369
- **Tokens permitidos:** `FALSE-DIVBYZERO`, `TRUE`, `UNKNOWN`

````text
PROPERTY — no division by zero

No execution may evaluate an integer division or remainder whose right operand
is zero.

A violation is any execution reaching `a / b` or `a % b` on integer types with
b == 0, including the compound forms `a /= b` and `a %= b`.

Not a violation for this property:
  - floating-point division by zero, which is defined and yields an infinity or
    a NaN;
  - signed overflow such as INT_MIN / -1 — that is the no-overflow property;
  - a division by zero that appears only in code no execution reaches, or only
    on a path excluded by __VERIFIER_assume / assume_abort_if_not.

Answer FALSE-DIVBYZERO if some execution divides or takes a remainder by zero.
Answer TRUE if no execution can.
Answer UNKNOWN if you cannot decide.

PROGRAM:
```c
{{PROGRAM}}
```
````

---

## P7 — `ASSERT` (assertiva alcançável e falha)

- **Map2Check:** `map2check --check-asserts --nondet-generator symex <file.c>`
- **CWEs no benchmark:** 617
- **CWEs da propriedade (referência):** 617
- **Tokens permitidos:** `ASSERT`, `TRUE`, `UNKNOWN`

````text
PROPERTY — assertion validity

Every assertion in the program must hold whenever it is evaluated. Both
`assert(c)` (which expands to a call to __assert_fail when c is false) and
`__VERIFIER_assert(c)` count.

A violation is any execution that reaches an assertion whose condition
evaluates to false.

Not a violation for this property:
  - an assertion whose condition is false only under inputs excluded by
    __VERIFIER_assume or assume_abort_if_not;
  - an assertion inside code no execution reaches;
  - any memory error, overflow or division by zero that occurs on a path that
    never reaches a failing assertion. If such an error would occur BEFORE the
    assertion on the same path, the assertion is not reached and this property
    is not violated by that path.

Answer ASSERT if some execution reaches an assertion that evaluates to false.
Answer TRUE if no execution can.
Answer UNKNOWN if you cannot decide.

PROGRAM:
```c
{{PROGRAM}}
```
````

---

## P8 — `TARGET-REACHED` (alcançabilidade da função-alvo)

- **Map2Check:** `map2check --target-function --target-function-name <NAME> --nondet-generator symex <file.c>`
- **CWEs no benchmark:** *nenhum* — `test_cwe_mode_mapping.sh` proíbe mapear
  qualquer CWE para modo de alcançabilidade (o alvo `main` era degenerado)
- **Propriedade:** `unreach-call` do SV-COMP, usada como codificação genérica de
  "este ponto é alcançável"
- **Tokens permitidos:** `TARGET-REACHED`, `TRUE`, `UNKNOWN`
- **Placeholder extra:** `{{TARGET}}` — o nome da função-alvo, o mesmo passado
  em `--target-function-name`.

````text
PROPERTY — unreach-call

The function `{{TARGET}}` must never be called. Determine whether some
execution of the program reaches a call to it.

A violation is any execution in which `{{TARGET}}` is called, directly or
through any chain of calls, including through a function pointer.

Not a violation for this property:
  - a call to `{{TARGET}}` that appears in the source but that no execution
    reaches;
  - a call reachable only on a path excluded by __VERIFIER_assume or
    assume_abort_if_not;
  - any memory error, overflow or division by zero on a path that never reaches
    the call.

Answer TARGET-REACHED if some execution calls `{{TARGET}}`.
Answer TRUE if no execution can.
Answer UNKNOWN if you cannot decide.

PROGRAM:
```c
{{PROGRAM}}
```
````

---

## Pontuação

Por trilha, contra o rótulo do benchmark:

| Resposta \ Rótulo | vulnerável | seguro |
|:---|:---|:---|
| veredicto de violação correto | **TP** | **FP** |
| `TRUE` | **FN** | **TN** |
| `UNKNOWN` | *inconclusivo* | *inconclusivo* |
| veredicto de violação de outra trilha | erro de formato | erro de formato |

`UNKNOWN` fica numa coluna própria e não entra em precisão/recall — é o que
torna a comparação com uma ferramenta que também responde `UNKNOWN` honesta.
Um `FALSE-DEREF` na trilha de overflow é erro de formato, não acerto parcial:
o prompt restringe os tokens permitidos.

O campo `LINE` é métrica secundária (localização), medida só sobre os TPs.

**Duas trilhas não têm corpus rotulado no repositório:** P3
(`FALSE-MEMTRACK`) porque os dois runners roteiam CWE-401 para
`--memcleanup-property`, e P8 (`TARGET-REACHED`) porque
`test_cwe_mode_mapping.sh` proíbe mapear qualquer CWE para alcançabilidade.
Rodar essas duas exige montar um corpus antes — ou declará-las fora da
comparação. As outras seis saem direto de Juliet e CASTLE.

## Uso

```sh
python3 build_prompts_json.py   # regenera prompts.json a partir deste .md
```

O markdown é a fonte de verdade; `prompts.json` é derivado. Cada entrada traz
`system_prompt`, `user_prompt` com os placeholders `{{PROGRAM}}` (e
`{{TARGET}}` em P8), `allowed_tokens` para validar a resposta, e
`map2check_command` — a invocação pareada, para que os dois lados da comparação
fiquem no mesmo arquivo.
