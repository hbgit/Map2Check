# 1. Introducao e Motivacao

**Estado:** Rascunho nao iniciado.  
**Fontes:** docs/pre-projeto, dados de CVEs/CWEs (usuario deve buscar estatisticas atualizadas).  
**Figuras:** Nenhuma nesta secao.

---

## 1.1 O problema de memory safety em C/C++

[PARAGRAFO A REDIGIR]
- Abrir com estatisticas de vulnerabilidades de memoria (CWEs 119, 416, 415, 401, 787)
- Citar que memory-safety bugs continuam entre as principais causas de CVEs exploraveis
- Contextualizar: sistemas embarcados, infraestrutura critica, consequencias de falhas

## 1.2 Verificacao hibrida como abordagem promissora

[PARAGRAFO A REDIGIR]
- Breve explicacao da combinacao analise estatica + dinamica (fuzzing + execucao simbolica)
- Citar trabalhos de ponta: FuSeBMC (smart seeds), Symbiotic (slicing + KLEE)
- Destacar que ferramentas de competicao (SV-COMP, TestComp) impulsionam o estado da arte

## 1.3 Posicionamento do Map2Check

[PARAGRAFO A REDIGIR]
- Map2Check como verificador hibrido consolidado para programas C
- Mencao as publicacoes anteriores (TACAS 2016, 2018, 2020)
- O problema: estagnacao desde 2019 (LLVM 6.0, Ubuntu 16.04, ultimo release nov/2019)

## 1.4 Contribuicoes desta versao (2026)

[PARAGRAFO A REDIGIR]
- Lista numerada de contribuicoes:
  1. Migracao completa da stack: LLVM 6 -> 16, New Pass Manager, C++17
  2. Infraestrutura de engenharia: CI/CD, Docker, sanitizers, static analysis
  3. Validacao em benchmarks reais: TestComp 2026 (Heap, 594 tasks, score 57)
  4. Fortalecimento do posicionamento em ciberseguranca (mapeamento CWE)

---

*Nota: Aguardando redacao na Sprint 1.*
