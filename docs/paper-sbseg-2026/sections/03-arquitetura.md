# 3. Arquitetura e Funcionalidades

**Estado:** Rascunho nao iniciado.  
**Fontes:** docs/migration/1.3-passes-new-pm.md, docs/migration/1.4-frontend-cpp17.md, .github/workflows/.  
**Figuras:** Figura 2 (pipeline), Figura 3 (passes), Figura 5 (CI/CD).

---

## 3.1 Pipeline de verificacao (visao geral)

[PARAGRAFO A REDIGIR]
- Fluxo completo: Codigo C -> clang-16 -> LLVM IR -> opt (passes) -> llvm-link -> runtime -> KLEE / LibFuzzer -> Veredito + Witness
- Figura 2: Diagrama do pipeline

## 3.2 Passes de instrumentacao (New Pass Manager)

[PARAGRAFO A REDIGIR]
- Descricao dos 9 passes migrados:
  - AssertPass, TargetPass, LoopPredAssumePass, Map2CheckLibrary
  - NonDetPass, TrackBasicBlockPass
  - OverflowPass, GenerateAutomataTruePass, MemoryTrackPass
- Figura 3: Diagrama dos passes
- Destacar: isRequired() como fix critico

## 3.3 Motores de analise

[PARAGRAFO A REDIGIR]
- KLEE 3.1 (LLVM 16): execucao simbolica
- LibFuzzer (LLVM 16): fuzzing guiado por cobertura
- Iterative deepening: combinacao dos dois motores

## 3.4 Modernizacao de engenharia (pilar de sustentabilidade)

[PARAGRAFO A REDIGIR]
- LLVM 16 + New Pass Manager
- C++17 (std::filesystem, structured bindings)
- CI/CD (GitHub Actions)
- Docker (Dockerfile.dev, Ubuntu 22.04)
- Static analysis (clang-tidy, cppcheck)
- Sanitizers (ASAN, UBSAN, TSAN)
- OpenSSF Best Practices (em andamento)
- Figura 5: Workflow CI/CD
- CTA: esta infraestrutura pontua nos criterios de artefato

---

*Nota: Aguardando redacao na Sprint 1.*
