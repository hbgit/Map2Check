# Roteiro do Vídeo Tecnico — Map2Check 2026

**Duracao alvo:** 5–7 minutos  
**Formato:** Screencast + narracao  
**Resolucao:** 1920x1080, 30fps  
**Audio:** Narracao clara, sem musica de fundo  
**Legendas:** Recomendado

---

## Storyboard — 8 Cenas

### Cena 1: Introducao (30 segundos)
**Visual:** Terminal limpo, texto overlay.
**Texto:** Map2Check 2026 — Modernizacao de uma Ferramenta de Verificacao de Memory Safety.
**Narracao:** Map2Check e uma ferramenta de verificacao hibrida para programas em C. Em 2026, completamos uma modernizacao completa da stack: de LLVM 6 para LLVM 16, migrando todos os passes para o New Pass Manager e adotando C++17. Neste video, demonstramos o pipeline completo de instalacao, build e execucao.

---

### Cena 2: Clone e Build com Docker (60 segundos)
**Visual:** Terminal executando comandos. Time-lapse no docker build.
**Comandos:**
```bash
git clone https://github.com/<repo>/Map2Check.git
cd Map2Check
docker build -f Dockerfile.dev -t map2check-dev .
docker run --rm map2check-dev clang-16 --version
docker run --rm map2check-dev opt --version
```
**Narracao:** O ambiente e inteiramente containerizado. A imagem Docker inclui LLVM 16, KLEE 3.1, Z3, Boost e todas as dependencias. O build e reprodutivel em qualquer maquina com Docker.

---

### Cena 3: Execucao do Smoke Test (60 segundos)
**Visual:** Execucao de array-1.c (TRUE) e array-2.c (FALSE). Highlight no veredito.
**Comandos:**
```bash
map2check --property-file coverage-error-call.prp \
  --target-function-name reach_error loops/array-1.c
# Resultado: VERIFICATION SUCCEEDED

map2check --property-file coverage-error-call.prp \
  --target-function-name reach_error loops/array-2.c
# Resultado: VERIFICATION FAILED + counter-example
```
**Narracao:** O pipeline completo e invocado por um unico comando. No primeiro exemplo, o erro nunca e alcancado. No segundo, encontramos o erro e geramos um contra-exemplo concreto.

---

### Cena 4: Verificacao do Counter-Example (60 segundos)
**Visual:** Saida detalhada do counter-example. Highlight nos valores.
**Texto exibido:**
```
Counter-example:
  Call Function  : __VERIFIER_nondet_int()
  Value          : 31763
  Line Number    : 19
  Function Scope : main

Violated property:
  FALSE: Target Reached
```
**Narracao:** O contra-exemplo fornece valores concretos para as entradas nao-deterministicas que levam a violacao, tornando a falha reprodutivel.

---

### Cena 5: Framework TestComp 2026 (60 segundos)
**Visual:** Navegacao ate test-comp2026/simulation/. Execucao de run_checkpoint.sh.
**Comandos:**
```bash
cd test-comp2026/simulation/
./run_checkpoint.sh --category Heap --timeout 300
./verify_verdicts.sh
```
**Narracao:** Para validar em escala, executamos o framework de checkpoint contra benchmarks reais do TestComp 2026. O script gerencia multiplas tarefas, timeouts e validacao automatica.

---

### Cena 6: Resultados Consolidados (45 segundos)
**Visual:** Tabela/painel com resultados. Grafico de barras (Figura 4).
**Texto:**
```
Resultados TestComp 2026 — Heap
Total tasks : 594
Score       : 57
TRUE        : 264 (44.4%)
UNKNOWN     : 271 (45.6%)
FALSE       : 56  (9.4%)
TIMEOUT     : 2   (0.3%)
```
**Narracao:** Na subcategoria Heap, executamos 594 tasks. A ferramenta encontrou 56 bugs reais e alcancou um score de 57. Estes resultados demonstram a viabilidade competitiva.

---

### Cena 7: CI/CD e Qualidade de Artefato (45 segundos)
**Visual:** Pagina GitHub Actions. Badge verde. Terminal com static analysis.
**Comandos:**
```bash
./scripts/run-static-analysis.sh
mkdir build-asan && cd build-asan
cmake -DENABLE_SANITIZER_ADDRESS=ON ..
make -j$(nproc)
```
**Narracao:** A sustentabilidade e garantida por CI/CD automatizado, analise estatica com clang-tidy e cppcheck, e testes com sanitizers. Estas praticas sao essenciais para a reproducibilidade cientifica.

---

### Cena 8: WebAssembly — Sneak Peek (opcional, 20 segundos)
**Visual:** Terminal mostrando compilacao C -> WASM via LLVM.
**Comandos:**
```bash
clang-16 --target=wasm32-wasi -emit-llvm -c exemplo.c -o exemplo.bc
```
**Narracao:** Como direcao futura, estamos estendendo o pipeline LLVM para analise de modulos WebAssembly, ampliando o escopo da verificacao para runtimes WASM.

---

### Cena 9: Conclusao (30 segundos)
**Visual:** Tela final com link do repositorio e QR code.
**Texto:** Map2Check 2026 — Codigo aberto, verificacao sustentavel.
**Narracao:** O Map2Check esta disponivel como codigo aberto. Agradecemos o interesse e convidamos a comunidade a contribuir.

---

## Notas de Producao

- Usar `asciinema` ou `terminalizer` para gravacao elegante do terminal.
- Acelerar partes longas (docker build, execucao de multiplos benchmarks) com time-lapse.
- Manter fonte do terminal grande o suficiente para leitura em projetor.
- URL do repositorio pode constar normalmente no video (single-blind). Opcional: usar URL generica se preferir.
- Dura total nao deve exceder 7 minutos.
