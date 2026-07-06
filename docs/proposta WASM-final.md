

Proposta de Pesquisa e Desenvolvimento:
Map2Check-WASM
- Contextualização e Motivação
A adoção do WebAssembly (WASM) expandiu-se rapidamente do navegador para o
ecossistema de Internet das Coisas (IoT) e Edge Computing. Sua portabilidade, eficiência e
isolamento nativo (sandbox) o tornam ideal para executar aplicações em dispositivos com
recursos restritos. No entanto, o paradigma WASM não elimina falhas de segurança inerentes
às linguagens de origem (como C/C++ e Rust).
Como destacado na literatura recente, vulnerabilidades binárias clássicas — como Buffer
Overflows e Use-After-Free (UAF) — sobrevivem no módulo WASM. Em contextos IoT, onde
módulos WASM frequentemente interagem com sensores, atuadores e bancos de dados locais,
essas falhas de memória linear podem ser exploradas para desencadear vulnerabilidades de
nível de aplicação (como injeções SQL, XS-Leaks e SSTIs), contornando mecanismos de
segurança estabelecidos.
O Map2Check é uma ferramenta de verificação de software consolidada que opera sobre a
Representação Intermediária do LLVM (LLVM-IR). Para garantir a segurança de módulos
WASM em IoT, propõe-se a extensão do Map2Check para suportar a verificação formal desses
binários, modelando sua execução e memória linear diretamente no LLVM-IR, antecipando e
mitigando vetores de ataque antes da implantação nos dispositivos.
- Relevância Técnico/Científica e Inovadora
Inovação em Verificação de Código WASM: A maioria das ferramentas de análise de
WASM foca em análise dinâmica ou fuzzing. Aplicar Bounded Model Checking (BMC) e
execução simbólica via Map2Check para provar a ausência (ou presença) de violações de
memória em binários WASM é uma abordagem de fronteira.
Ponte entre Baixo e Alto Nível (Desafio do LLVM-IR): O ecossistema atual (incluindo o
MLIR) demonstra que a compilação de linguagens de alto nível para WASM via LLVM-IR
sofre com a perda de abstrações semânticas. A inovação desta proposta reside na criação
de um modelo de memória no LLVM-IR que seja semanticamente ciente das restrições do
WASM (como a memória linear de 32-bits e a ausência de Stack Smashing Protection
nativa dentro do sandbox), permitindo que o Map2Check identifique precisamente como
falhas binárias se tornam vulnerabilidades de sistema/web.

Foco em Cibersegurança (Shift-Left Security em IoT): O projeto entrega um mecanismo
para analisar third-party WASM modules que serão embarcados em IoT, garantindo que
não carreguem exploits que possam comprometer a rede local ou vazar dados de
sensores.
- Viabilidade de Execução
A execução do projeto é altamente viável, fundamentada nos seguintes pilares técnicos:
- Maturidade do Map2Check: A ferramenta já possui suporte robusto para análise de
LLVM-IR. A adaptação não exige reescrever o motor de verificação (backend), mas sim
criar um frontend/middleware adequado.
- Disponibilidade de Ferramentas de Lifting: Transformar bytecode WASM em LLVM-IR
(processo de lifting) pode ser orquestrado combinando ferramentas open-source existentes
(como wasm2c seguido de compilação Clang para emissão de LLVM-IR, ou ferramentas
diretas como wasm2llvm).
- Modelagem de Memória Determinística: A memória do WASM é um array contíguo
(Linear Memory). Modelar um acesso a ponteiro no Map2Check se reduz a verificar se o
índice (um valor i32) está dentro dos limites desse array. Essa simplicidade estrutural do
WASM favorece a verificação baseada em restrições lógicas (SMT Solvers).
- Arquitetura da Solução e Metodologia
O R&D será conduzido em quatro fases técnicas:
Fase 1: Lifter de WASM para LLVM-IR Ciente de Segurança. Desenvolver/adaptar um
módulo de tradução que converta binários .wasm (ou código fonte direcionado a WASM)
para LLVM-IR. Para mitigar a perda de abstrações (problema apontado na literatura sobre
MLIR/LLVM), o lifter injetará metadados de depuração e anotações intrínsecas no LLVM-IR
para preservar informações sobre limites de objetos e chamadas de
importação/exportação do host IoT.
Fase 2: Modelagem da Memória Linear e Contexto IoT. Modificar a semântica
operacional do Map2Check para entender o ambiente WASM.
Ponteiros serão tratados como offsets numéricos.

Memory Bounds: Injeção de asserções automáticas (assert(ptr < MEM_SIZE)) antes de
instruções de load/store.
Fase 3: Mapeamento de Vulnerabilidades Binárias para Lógicas (Threat Modeling).
Implementar verificadores de propriedades (Property Checkers) focados em segurança
web/IoT. Por exemplo: rastrear se dados controlados pelo usuário no WASM (Taint
Analysis) alcançam funções importadas do Host (ex: uma função de query de banco de
dados, resultando em SQLi, ou APIs de rede do dispositivo IoT).
Fase 4: Integração e Refinamento do Motor. Otimização das fórmulas SMT geradas
para evitar a explosão de estados, utilizando técnicas de loop unrolling direcionado e
fatiamento de programa (program slicing).
- Plano de Avaliação Experimental
Para validar a eficácia do Map2Check-WASM, o plano experimental será rigoroso e focado em
métricas de Cibersegurança:
5.1. Benchmarks e Datasets
- Juliet Test Suite (C/C++): Compilado para WASM, focado em CWEs de corrupção de
memória (CWE-119: Buffer Errors, CWE-416: Use After Free).
- PolyBench (Adaptado): Para medir o overhead de verificação em algoritmos
computacionalmente intensivos (conforme literatura sobre MLIR/WASM).
- Aplicações IoT Reais: Módulos WASM extraídos de frameworks de Edge Computing (ex:
WasmEdge, Spin) atuando como handlers de requisições HTTP e processamento de
dados de sensores.
5.2. Cenários de Exploração (Proof of Concept - PoC)
Serão desenvolvidos módulos WASM intencionalmente vulneráveis simulando os ataques
descritos na literatura:
Cenário A (Buffer Overflow para SQLi): Um módulo IoT que processa telemetria. Um
buffer overflow na memória linear sobrescreve uma string adjacente usada em uma query
SQLite importada do Host. O Map2Check deve detectar a violação antes da execução.
Cenário B (Use-After-Free para Vazamento de Dados): Um módulo WASM manipula
credenciais temporárias do dispositivo IoT. A reatribuição indevida da memória linear
permite o vazamento do token.

5.3. Métricas de Avaliação
Taxa de Detecção (True Positive Rate) e Falsos Positivos: Avaliar a precisão da
ferramenta na identificação de CWEs específicas dentro do WASM.
Tempo de Verificação e Escalabilidade: Medir o tempo necessário (em segundos) e o
consumo de memória (RAM) para provar a segurança ou encontrar o contraexemplo (o
bug) via SMT Solver em módulos crescentes (de 1KB a 1MB de LLVM-IR).
Preservação Semântica: Comparar os resultados de detecção entre o código C/C++
analisado diretamente e o LLVM-IR gerado através do lifting do WASM compilado. O
objetivo é provar que a tradução baseada em LLVM-IR não degrada a capacidade de
encontrar o bug.
## Conclusão
A viabilização desta pesquisa posicionará o Map2Check como uma ferramenta pioneira na
segurança profunda de infraestruturas IoT de próxima geração baseadas em WebAssembly. Ao
transpor os desafios de abstração do LLVM-IR e conectar falhas binárias com consequências
de nível de aplicação, o projeto oferece uma resposta direta e automatizada às preocupações
modernas de cibersegurança em ambientes Edge/Cloud.
Referências (Adicionar no artigo do SBSeg):
[1] https://arxiv.org/html/2603.09426v1 [2] https://arxiv.org/abs/2506.16048