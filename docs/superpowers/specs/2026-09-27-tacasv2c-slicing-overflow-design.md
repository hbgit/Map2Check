# tacasv2c — Slicing para overflow

**Data:** 2026-09-27
**Branch prevista:** `feat/tacas-slicing-overflow` (a partir de `feat/tacas-slicing-mem`)
**Baseline:** tacasv1, mesmo build com e sem `--slice`
**Status:** rascunho de estudo — aguardando revisão
**Registro:** `docs/reports/tacas-experiment-log.md` (R7 = sondagem deste design)

---

## 1. Objetivo

Estender o `--slice` para `--check-overflow`, fechando o slicing por propriedade da
tacasv2 (2a: reach/assert; 2b: memória; 2c: overflow).

## 2. Estudo

- **Como o overflow é instrumentado.** O `OverflowPass` troca cada operação aritmética
  com sinal (add, sub, mul, sdiv, srem, shl…) por uma chamada de runtime
  `map2check_binop_*` (`modules/backend/pass/OperationsFunctions.hpp`). Essas chamadas
  recebem os operandos e registram a violação.
- **Consequência:** o critério de overflow é o mesmo do 2b, ou seja, toda chamada
  `map2check_*` do módulo instrumentado mais as nondets. `Caller::sliceInstrumented()`
  já faz exatamente isso. Nenhuma lógica de slicing nova é necessária.
- **O que o Symbiotic faz** (levantado na 2a): instrumenta as checagens de overflow e
  fatia em relação à chamada de erro (`__VERIFIER_error`). O nosso critério é mais
  conservador: fatiar em relação às próprias checagens mantém todas elas, e não só as
  que já levam a erro.
- **Sondagem (R7):** busybox `chgrp-incomplete-2.i` 927 → 487 instruções (−47%);
  bitvector −2%; nla-digbench −14%. Nos programas grandes, a redução é maior que a de
  memória (R6: −20%).

## 3. Mudanças

1. **Gating** (`map2check.cpp`): `OVERFLOW_MODE` entra no gancho pós-`callPass` junto com
   memtrack/memcleanup. A mensagem de recusa passa a citar só coverage.
2. **Harness:** `build_corpus.py` ganha `--property overflow` (`no-overflow.prp`), com
   categorias por diretório espelhando o NoOverflows do SV-COMP:
   - Main: `bitvector`, `nla-digbench-scaling`, `recursive-simple`, `loop-zilu`,
     `signedintegeroverflow-regression`, `goblint-regression`;
   - BusyBox: `busybox-1.22.0`;
   - Juliet: `Juliet_Test`.

   Ficam excluídos `pthread*`, `weaver` e `termination-*`.
   `run_memsafety_evaluation.sh` aceita `PROPERTY=overflow` (modo `--check-overflow`),
   e o classificador mapeia a subpropriedade `no-overflow` para `FALSE-OVERFLOW`.
3. **Testes:**
   - overflow com slice mantém o FALSE-OVERFLOW;
   - programa sem overflow não vira FALSE;
   - a recusa em cover-branches continua;
   - linhas novas na tabela do classificador (`no-overflow`).

## 4. Avaliação (mini-rodadas, registradas no log)

- CASTLE CWE-190 (os 12 casos de overflow) e Juliet CWE-190, com e sem `--slice`. Os
  runners já aceitam `EXTRA_FLAGS`.
- NoOverflows do SV-COMP: 10 por categoria, com e sem `--slice`.
- Métricas: acertos, **wrong-true e wrong-false** e redução de instruções.

## 5. Riscos

| Risco | Mitigação |
|---|---|
| Operação sem instrumentação (sem sinal, por exemplo) sai da fatia e deixa de alimentar uma checagem | O critério é a checagem; o dg mantém as dependências de dados de cada operando |
| Redução nula em programas pequenos (CASTLE/Juliet) | Critério de sucesso = não piorar; o ganho aparece no SV-COMP (R7) |
