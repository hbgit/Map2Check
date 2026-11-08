# Previsões para o v9, registradas antes de rodar

Escritas **antes** do lançamento, para que um erro fique visível em vez de ser
racionalizado depois. No v8 eu previ 30–50% de confirmação no cover-error e deu
13,9% — o diagnóstico estava incompleto, e é por isso que isto existe.

## Desenho

Corpus de cota 40, que a amostragem aninhada garante ser **subconjunto exato**
da cota 150 usada no v8. A comparação é pareada nas mesmas tarefas.

| grupo | categorias | n | previsão |
|---|---|---|---|
| **tratamento** | Sequentialized, Floats, Arrays, Loops, ControlFlow | 158 | devem melhorar |
| **controle** | as outras sete | 214 | não devem mudar |

O grupo de controle é o que separa "a correção funcionou" de "a corrida variou".

## O que eu espero

**Cover-error, tratamento.** No v8: Sequentialized 0/40, Floats 0/33, Arrays
1/40. Se `assume_abort_if_not` é mesmo a causa, essas três saem do zero. Não
arrisco número: a hipótese sustenta a *direção*, não a magnitude — o que a
correção remove é um obstáculo, e nada garante que não haja outro atrás dele.
Um único caso confirmado em Sequentialized já falsifica "0% é estrutural".

**Cover-error, controle.** Sem mudança além de ruído de execução. Se o controle
se mover junto, a atribuição à correção do assume não se sustenta.

**Falsos positivos.** Devem CAIR. O `pals_lcr` dava FAILED com 25 inputs que
cobriam 0,0%; agora dá UNKNOWN. Contagem de `FAILED` menor com confirmação
maior é o resultado bom aqui — um `FAILED` a menos que era falso vale mais que
um a mais que não se sustenta.

**Juliet e CASTLE.** A instabilidade `UNKNOWN↔ERROR` (45–48 trocas em três
corridas) deve encolher, porque a causa provável era o campo não inicializado.
Se persistir no mesmo tamanho, a causa é outra e eu estava errado.

**CASTLE.** Com o achado B corrigido no harness (commit do opencode), os 98
casos em modo degenerado deixam de rodar. Menos tempo gasto, e os 12 TN falsos
que aquele modo rendia desaparecem — a precisão pode CAIR e isso é correto.
