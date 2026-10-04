# Backlog

Itens decididos como "não agora", cada um com o motivo e onde está a evidência.

## `--add-invariants`: rodada dedicada de estudo

- **Estado:** a opção é opcional e fica desligada no build padrão (`-DENABLE_CLAM=OFF`).
  O usuário ainda desconfia da conclusão "não ajuda".
- **O que já se sabe** (log de experimentos, INV-1 e R25):
  - Na v7.3 os invariantes saíam como `llvm.assume` e ficavam inertes. Até out/2018 eles
    chegavam ao KLEE (`--crab-track=arr --crab-add-invariants=after-load`).
  - Numa sonda só com o KLEE, o ganho veio do pré-processamento em SSA, não dos
    invariantes.
  - No híbrido (71 tarefas de Cover-Error e 50 de MemSafety), os perfis do Clam não
    ganharam e o MemSafety teve mais FALSE errados (4 contra 2). Parte dos ERROR vinha do
    Clam sem limite de tempo, já corrigido.
- **O que a rodada precisa responder:**
  - efeito só com o KLEE (`--nondet-generator symex`), onde os invariantes têm mais
    chance de pesar, separado do efeito no híbrido;
  - por que os FALSE errados de MemSafety aumentam: um invariante incorreto ou o
    caminho de compilação do Clam (`-m 64`, pré-processamento próprio);
  - os domínios que o motor antigo tinha e o nosso Clam não compila: Apron (`oct`,
    `pk`) e LDD (`boxes`);
  - o gate: nenhuma detecção perdida no CASTLE e numa amostra do Juliet.

## Limite de memória do KLEE nas rodadas paralelas

- **Sintoma:** com 8 contêineres, e às vezes com 4, a memória da máquina (23 GB) chega a
  ficar crítica. O KLEE usa até `--max-memory` = 2000 MB por processo. Um KLEE morto
  pelo OOM killer sai como "did not finish its run", ou seja UNKNOWN, e contamina a
  medição. Os vigias do Claude Code também foram encerrados por isso.
- **Opções:**
  - passar `--max-memory` por execução, proporcional às vagas;
  - limitar a memória por contêiner (`docker run --memory`);
  - registrar no resultado quando o KLEE morreu por falta de memória, para descartar e
    repetir essas tarefas.
- **Por que não agora:** não afeta o produto (uma execução de competição tem uma tarefa
  por máquina), só a infraestrutura de medição.

## Harness de campanha fora do repositório

- Os escalonadores das rodadas R19 a R26 vivem em `../tacas-results/` e não são
  versionados, por decisão: campanhas não pertencem ao repositório do produto.
- Dois defeitos deles contaminaram rodadas, e quem escrever o próximo precisa evitá-los:
  - `IFS=$'\t' read` junta tabs consecutivos, e uma coluna vazia desloca as seguintes;
    usar `-` nos campos vazios;
  - os filhos herdavam o manifest pelo descritor 3. Corrigido no harness do repositório
    (`3<&-`).

## Validação de witness (fase D, item 4.2): estudo, a discutir com o orientador

- **Estado atual:**
  - O Map2Check gera só witness **GraphML** (`--generate-witness`, formato 1.0), gravado
    em `../witness.graphml` a partir do scratch.
  - Não gera o formato **2.0 (YAML)**, adotado pelo SV-COMP. É preciso confirmar no
    call do ano quais formatos ainda são aceitos.
  - Nenhum teste valida os witnesses gerados.
  - A Test-Comp não usa witness; a suíte é validada pelo TestCov. O item só pesa para o
    SV-COMP.
- **Caminho barato já esboçado (validação por execução):** compilar o programa original
  (com ASan, para as propriedades de memória), alimentá-lo com o vetor que viola a
  propriedade (que o Map2Check já emite na suíte de Cover-Error) e confirmar o
  `reach_error` ou o crash. É parecido com o que o TestCov faz e com os validadores por
  execução do SV-COMP.
- **Não começar a implementação** antes da conversa com o orientador: definir se o
  objetivo é o formato 2.0, a validação por execução, os dois ou nenhum.

## `--debug` com a alternância dá segfault

- Achado em 2026-10-02: `map2check --debug --check-overflow --timeout 60` sobre um caso do
  Juliet (CWE190 `char_fscanf_square_01`, good) termina com segfault depois das fases. O
  scratch aparece aninhado (`<hash>.map2check/<hash>.map2check-…`). Sem `--debug` a
  execução termina normalmente. Só afeta a depuração.
