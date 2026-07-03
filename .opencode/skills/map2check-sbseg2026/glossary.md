# Glossary — Map2Check SBSeg 2026

**AFL++** — American Fuzzy Lop++, fuzzer guiado por cobertura, sucessor do AFL (Ch 1, Pré-projeto)

**ASAN** — AddressSanitizer, detector de erros de memória em tempo de execução do LLVM (Ch 3)

**AssertPass** — Pass LLVM que instrumenta chamadas a `__VERIFIER_assert` (Ch 3)

**BMC** — Bounded Model Checking, verificação exaustiva até profundidade K de loops (Ch 2)

**C++17** — Padrão C++ adotado na modernização, traz `std::filesystem`, structured bindings (Ch 3)

**CBMC** — C Bounded Model Checker, verificador BMC usado na versão original do Map2Check (Ch 2)

**CI/CD** — Continuous Integration / Continuous Deployment com GitHub Actions (Ch 3)

**CPAchecker** — Framework de verificação configurável com múltiplas estratégias, referência desde SV-COMP 2013 (Ch 2)

**Crab-LLVM** — Análise estática baseada em interpretação abstrata para invariantes indutivos (Ch 2)

**CWE-119** — Buffer Overflow: acesso fora dos limites de buffer (Ch 1, Ch 4)

**CWE-190** — Integer Overflow/Wraparound (Ch 4)

**CWE-401** — Memory Leak: vazamento de memória alocada (Ch 1, Ch 4)

**CWE-415** — Double-Free: liberação dupla do mesmo ponteiro (Ch 1, Ch 4)

**CWE-416** — Use-After-Free: acesso a memória já liberada (Ch 1, Ch 4)

**CWE-787** — Out-of-bounds Write (Ch 1, Ch 4)

**DG Library** — Dependence Graph, biblioteca para slicing estático baseado em SDG (Pré-projeto)

**Execução Simbólica** — Técnica que analisa programas com valores simbólicos, construindo path constraints (Ch 1)

**FALSE (veredito)** — Propriedade de segurança violada: bug encontrado com contra-exemplo (Ch 4, Ch 5)

**FuSeBMC** — Ferramenta de ponta com smart seeds e BMC para desbloquear fuzzer (Ch 1)

**GenerateAutomataTruePass** — Pass que gera autômato de witness em GraphML (Ch 3)

**GraphML** — Formato XML padronizado para witness de verificação SV-COMP (Ch 3)

**isRequired()** — Método do New PM que declara passe como sempre requerido — fix crítico (Ch 3, Ch 4)

**Iterative Deepening** — Combinação KLEE + LibFuzzer em fases alternadas (Ch 3)

**KLEE** — Motor de execução simbólica para LLVM bitcode, versão 3.x (Ch 2, Ch 3)

**Legacy Pass Manager** — API antiga do LLVM para passes, deprecada desde v13 (Ch 3)

**LibFuzzer** — Motor de fuzzing do LLVM guiado por SanitizerCoverage (Ch 2, Ch 3)

**LLVM 16** — Versão do framework LLVM adotada na modernização (Ch 3)

**LLVM IR** — Representação Intermediária do LLVM, base para análise e instrumentação (Ch 1, Ch 3)

**LoopPredAssumePass** — Pass que injeta predicados `__VERIFIER_assume` em laços (Ch 3)

**Map2CheckLibrary** — Pass que renomeia `main` → `__map2check_main__` (Ch 3)

**MemoryTrackPass** — Pass que instrumenta acessos a memória (alloc, free, deref) (Ch 3, Ch 4)

**New Pass Manager** — API C++ moderna do LLVM, usa `PassInfoMixin` (Ch 3)

**NonDetPass** — Pass que substitui `__VERIFIER_nondet_*` por chamadas simbólicas (Ch 3)

**OpenSSF** — Open Source Security Foundation, checklist Best Practices (Ch 3)

**OpenSSF Best Practices** — Programa de certificação de boas práticas para projetos open source (Ch 1, Ch 3, Ch 6)

**OverflowPass** — Pass LLVM que emprega análise de intervalos para detectar buffer overflow e integer overflow (Ch 3, Ch 4)

**SBSeg** — Simpósio Brasileiro de Segurança da Informação e de Sistemas Computacionais (Ch 5)

**Smart Seeds** — Sementes geradas por BMC e injetadas no fuzzer (FuSeBMC) (Ch 1, Pré-projeto)

**SMT Solver** — Satisfiability Modulo Theories, resolve path constraints (Z3) (Ch 1)

**SV-COMP** — Competition on Software Verification, benchmark padrão (Ch 1)

**Symbiotic** — Ferramenta que integra slicing estático (DG) com KLEE (Ch 1)

**TargetPass** — Pass que marca função-alvo para verificação (e.g. `reach_error`) (Ch 3)

**TestComp 2026** — Competition on Software Testing, sub-categoria Heap (Ch 4)

**TrackBasicBlockPass** — Pass que rastreia cobertura de blocos básicos (Ch 3)

**TRUE (veredito)** — Propriedade de segurança garantida: programa é seguro (Ch 4)

**TSAN** — ThreadSanitizer, detector de data races do LLVM (Ch 3)

**UBSAN** — UndefinedBehaviorSanitizer, detector de comportamento indefinido do LLVM (Ch 3)

**UNKNOWN (veredito)** — Verificação inconclusiva dentro do timeout (Ch 4, Ch 6)

**WASM** — WebAssembly, módulos portáteis para runtimes como Wasmtime/Wasmer (Ch 4, Ch 6)

**Witness** — Evidência de verificação: contra-exemplo ou prova de corretude (Ch 3)

**Z3** — Solver SMT da Microsoft Research, usado como solver padrão no Map2Check (Ch 4)

**CPAchecker** — Framework de verificação configurável com múltiplas estratégias (análise de valor, predicate abstraction, BMC) (Ch 2)

**Program Slicing** — Técnica de redução de código que remove trechos irrelevantes para a propriedade de segurança (Ch 1, Ch 2)

**Smart Seeds** — Sementes geradas por BMC e injetadas no fuzzer para desbloquear caminhos com magic numbers (Ch 1, Pré-projeto)

**wasi-sdk** — Toolchain para compilar C para WASM com target `wasm32-wasip1` (Ch 4, Implementation Plan)

**Portfólio de Solver** — Uso combinado de múltiplos solvers SMT (Z3, Boolector, Yices) para maximizar taxa de resultados conclusivos (Ch 2)
