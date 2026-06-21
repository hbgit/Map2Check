# Figuras do Artigo

Os arquivos `.mmd` são diagramas Mermaid. Para gerar as imagens PNG/SVG
para inclusão no LaTeX, execute:

```bash
# Instalar mermaid-cli (requer Node.js)
npm install -g @mermaid-js/mermaid-cli

# Gerar PNG
mmdc -i fig1-timeline.mmd -o fig1-timeline.png -b transparent
mmdc -i fig2-pipeline.mmd -o fig2-pipeline.png -b transparent
mmdc -i fig3-passes.mmd -o fig3-passes.png -b transparent
mmdc -i fig4-testcomp2026.mmd -o fig4-testcomp2026.png -b transparent
mmdc -i fig5-cicd.mmd -o fig5-cicd.png -b transparent
```

Ou use o site: https://mermaid.live

As figuras são referenciadas em `main.tex` via `\includegraphics{img/fig1-timeline}` etc.
