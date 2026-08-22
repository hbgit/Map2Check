#!/usr/bin/env bash
# Empacota o diretório release/ (gerado por `ninja install`) para o
# GitHub Release. Chamado pelo @semantic-release/exec com a versão
# calculada: scripts/package-release.sh 8.1.0
set -euo pipefail

VERSION="${1:?usage: package-release.sh <version>}"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO_ROOT"

[ -x release/map2check ] || {
  echo "ERROR: release/map2check não existe — rode 'ninja install' antes." >&2
  exit 1
}

# Composição do artefato: wrappers SV-COMP e Test-Comp + licença + documentação.
#
# O wrapper do Test-Comp entra aqui porque o módulo tool-info
# (utils/moduleBenchExec/map2check_testcomp.py) o declara em REQUIRED_PATHS: o
# BenchExec copia exatamente esses caminhos para o nó de execução, então um
# arquivo ausente do zip vira um erro na infraestrutura da competição, não aqui.
cp utils/map2check-wrapper.py release/
cp utils/map2check-testcomp-wrapper.py release/
cp LICENSE README.md release/

ZIP="map2check-v${VERSION}-linux-x86_64.zip"
rm -f map2check-v*-linux-x86_64.zip
zip -qry "$ZIP" release/
echo "Gerado: $ZIP"
# tail, não head: head fecha o pipe cedo e o SIGPIPE + pipefail derrubam o script
unzip -l "$ZIP" | tail -n 5
