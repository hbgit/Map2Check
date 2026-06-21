**Plano de Trabalho — Submissão ao Salão de Ferramentas (SF) do SBSeg 2026![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.001.png)**

**Ferramenta:** Map2Check — Finding Software Vulnerabilities using Symbolic Execution and Fuzzing **Modalidade sugerida:** Código Aberto (repositório público + vídeo técnico → elegível a prêmio e à avaliação de artefatos pelo CTA) **Coordenação técnica:** PD&I / Engenharia de Sistemas Computacionais

Antes do plano em si, deixo registradas três restrições do edital que moldam todo o trabalho, porque é nelas que submissões ao SF costumam tropeçar.

**Restrições do edital que precisam ser respeitadas desde já![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.002.png)**

O **limite é de 10 páginas: 8 de corpo (incluindo figuras e tabelas) + até 2 de referências e apêndices** — O modelo obrigatório é o template SBC, o que exige reformatação do material histórico.

O artigo deve conter obrigatoriamente, segundo o edital: descrição e motivação do problema; arquitetura e principais funcionalidades; descrição da demonstração planejada (com equipamentos necessários); e URL de vídeo de instalação/uso. A premiação depende de avaliação de artefato (disponibilidade, funcionalidade, reprodutibilidade, sustentabilidade) — e os commits recentes de CI/CD, Dockerfile e hardening são exatamente o que sustenta uma boa nota de artefato.

**Datas:** submissão **20/jul/2026**

Considerando hoje (17/jun/2026), há **~33 dias** até a submissão.

**Posicionamento e narrativa do artigo![ref1]**

A oportunidade aqui é tratar este artigo como o **marco de "renascimento" do Map2Check**. As publicações anteriores (TACAS 2016, 2018, 2020) eram *competition contributions* de 4–5 páginas, focadas em SV-COMP. Desde 2020 a ferramenta ficou essencialmente estagnada (LLVM 6.0, último release de nov/2019). Os commits de maio–junho/2026 mostram um esforço sistemático de modernização: migração para **LLVM 16 / Ubuntu 22.04**, conversão completa para o **New Pass Manager**, modernização para **C++17**, introdução de **CI/CD via GitHub Actions**, **análise estática (clang-tidy, cppcheck)**, **sanitizers**, busca pelo **OpenSSF Best Practices badge** e um framework de checkpoint para **TestComp 2026**.

A tese central que sugiro para o artigo: *o Map2Check evoluiu de um verificador de C orientado a competição para uma plataforma de detecção de vulnerabilidades de memória, modernizada em engenharia e estendida para alvos emergentes (WebAssembly), com foco renovado em memory safety (memory leak, use-after-free, invalid-free/deref).*

Sobre as features que vamos destacar:

- As features de **memory safety/overflow** já são histórico consolidado e funcional da ferramenta —  --memtrack , e as propriedades  valid-free ,  valid-deref ,  valid-memtrack , ![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.004.png)![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.005.png)![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.006.png)![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.007.png)valid-memcleanup do SV-COMP. Aqui o trabalho é de *fortalecimento e reposicionamento* sob a ótica de cibersegurança (mapear para CWEs: CWE-401 memory leak, CWE-416 use-after-free, CWE-415 double-free, CWE-119/787 buffer overflow). É terreno seguro.
- A feature de **WebAssembly (WASM)** é a novidade de maior impacto para um público de cibersegurança, mas **não aparece iniciamos nos commits**. Para isso temos duas saídas honestas: (a) implementar um pipeline mínimo viável de WASM antes de 20/jul (escopo abaixo), ou (b) posicionar WASM como *roadmap / trabalho em andamento* numa seção de perspectivas. Recomendo tentar (a) com escopo enxuto e ter (b) como plano de contingência.

**Estrutura proposta do artigo (8 páginas de corpo)![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.008.png)**

A distribuição abaixo equilibra história, arquitetura e novidade, dando peso às features que você quer destacar sem inflar o que ainda não existe.

A introdução e motivação (~1 pág) abrem com o problema de vulnerabilidades de memória em C/C++ como classe persistente e crítica de falhas de segurança, ancorando em CWEs e citando que memory-safety bugs seguem entre as principais causas de CVEs exploráveis. Posiciona o Map2Check nesse contexto e enuncia as contribuições da versão 2026.

A história e evolução (~1 a 1,5 pág) traça a linha 2016→2018→2020→2026: de *Hunting Memory Bugs* (TACAS 2016, BMC), para LLVM+KLEE (TACAS 2018), para a combinação fuzzing + execução simbólica + invariantes indutivos com LibFuzzer/KLEE/Crab-LLVM (TACAS 2020), até a modernização atual. Aqui entra uma figura de timeline.

A arquitetura e funcionalidades (~2 a 2,5 pág) é o núcleo técnico: instrumentação de código- fonte sobre LLVM, monitoramento de ponteiros e atribuições, geração de casos de teste por execução simbólica (KLEE) e fuzzing (LibFuzzer), invariantes indutivos (Crab-LLVM), e a abordagem de *iterative deepening*. Documenta a modernização de engenharia (LLVM 16, New Pass Manager, C++17, CI/CD, análise estática, sanitizers) como pilar de **sustentabilidade e qualidade de artefato** — que pontua diretamente no CTA. Figura de arquitetura/pipeline aqui. As novas features de cibersegurança (~1,5 pág) detalham a detecção de memory leak e violações de memory safety mapeadas para CWEs, com a integração ao TestComp 2026 (geração/validação de testes, execução de heap + verificação de veredito) como evidência de capacidade atual. É a seção que conecta a ferramenta ao escopo "cibersegurança" do SBSeg.

WebAssembly (~1 pág) apresenta o suporte a análise de binários/módulos WASM — seja como feature entregue (MVP) ou como direção arquitetural via o pipeline LLVM existente, discutindo o vetor de ataque de memory safety em runtimes WASM.

A demonstração planejada e o vídeo (~~0,5 pág) descrevem o que será mostrado ao vivo e os equipamentos (item obrigatório do edital). Encerra com avaliação preliminar, perspectivas e conclusão (~~0,5 pág). Referências e apêndice de reprodutibilidade vão nas 2 páginas extras.

**Cronograma de execução (17/jun → 20/jul)![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.009.png)**

Trabalhando para a submissão de 20 de julho, proponho quatro sprints.

**Sprint 0 — Fundação e decisão de escopo (17–22/jun, ~5 dias).** Baixar o template SBC e montar o esqueleto anonimizado; auditar o repositório para garantir que o estado de  master ![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.010.png)compila e roda os exemplos do README ( --memtrack ); definir o conjunto de benchmarks para a avaliação preliminar (subset SV-COMP MemSafety + casos próprios mapeados a CWEs). Entregável: outline aprovado + ambiente de build reprodutível (imagem Docker LLVM 16).![](Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.011.png)

**Sprint 1 — Núcleo técnico e história (23/jun–02/jul, ~10 dias).** Redigir história, arquitetura e funcionalidades; produzir as figuras de timeline e de pipeline; consolidar a narrativa de modernização (mapeando os commits de maio–junho para a seção de sustentabilidade). Em paralelo, executar os experimentos de memory safety e tabular resultados. Entregável: seções 1–4 em rascunho + tabela de resultados.

**Sprint 2 — Cibersegurança + WASM + demo (03–13/jul, ~10 dias).** Finalizar a seção de memory leak/CWE; implementar (ou documentar como roadmap) o caminho WASM; gravar e editar o **vídeo técnico** de instalação e uso (obrigatório); escrever a seção de demonstração com lista de equipamentos. Entregável: artigo completo em rascunho + vídeo publicado (link anonimizado) + checklist de artefato (disponibilidade/funcionalidade/reprodutibilidade/sustentabilidade).

**Sprint 3 — Revisão, anonimização e submissão (14–20/jul, ~7 dias).** Revisão técnica e de escrita; verificação rigorosa de anonimato (sem nomes, instituições, URLs identificáveis, metadados do PDF); checagem do limite de 8 páginas de corpo; revisão à luz do código de conduta da SBC sobre IA generativa; geração do PDF final e submissão no **JEMS**. Entregável: PDF submetido + pacote de artefato preparado para o eventual camera-ready/CTA.

**Riscos e recomendações![ref1]**

O maior risco é o **WASM não ficar funcional a tempo**; a mitigação é ter a versão "roadmap" pronta como contingência — não vale comprometer a nota de artefato apresentando algo irreprodutível. O segundo risco é **falha de anonimização**, que pode levar a *desk reject* num processo double-blind; por isso a verificação é uma etapa formal no Sprint 3, incluindo metadados do PDF. O terceiro é o **build não reprodutível** para o CTA — os commits recentes de Docker/CI ajudam, mas o ambiente precisa ser validado do zero por alguém que não seja o autor.

Uma nota sobre IA generativa: o edital remete explicitamente ao novo Código de Conduta da SBC quanto ao uso de IA generativa. Convém revisar essas regras antes de redigir e, se aplicável, declarar o uso conforme exigido.

[ref1]: Aspose.Words.20da6e99-f690-4638-80d8-9465634d56d0.003.png
