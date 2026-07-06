# Figuras do Artigo SBSeg 2026

## Figuras TikZ (compilação nativa pelo LaTeX)
- `fig1-timeline.tikz.tex` — Timeline 2016→2026 (Substitui PNG antigo, fonte maior)
- `fig2-pipeline.tikz.tex` — Pipeline de verificação 5 estágios (Substitui PNG antigo)

Estas são incluídas diretamente no LaTeX via `\input{img/figX-tikz.tex}`.

## Figura Mermaid (requer renderização externa)
- `fig6-wasm-pipeline.mmd` — Pipeline WASM: `.wasm → wasm2c → LLVM IR → Passes → KLEE`

Para gerar PNG a partir do .mmd:
```bash
npm install -g @mermaid-js/mermaid-cli
mmdc -i fig6-wasm-pipeline.mmd -o fig6-wasm-pipeline.png -b transparent -w 1200
```

Ou use: https://mermaid.live (copiar conteúdo do .mmd, exportar como PNG)

O arquivo `main.tex` referencia via `\includegraphics{img/fig6-wasm-pipeline}`.

## Figuras removidas
As figuras fig3-passes, fig4-testcomp2026 e fig5-cicd foram removidas do artigo por orientação do revisor. Seus arquivos `.mmd` e `.png` ainda existem no diretório mas não são mais referenciadas no `main.tex`.
