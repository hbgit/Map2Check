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

# Composição do artefato: wrapper SV-COMP + licença + documentação
cp utils/map2check-wrapper.py release/
cp LICENSE README.md release/

ZIP="map2check-v${VERSION}-linux-x86_64.zip"
rm -f map2check-v*-linux-x86_64.zip
zip -qry "$ZIP" release/
echo "Gerado: $ZIP"
# tail, não head: head fecha o pipe cedo e o SIGPIPE + pipefail derrubam o script
unzip -l "$ZIP" | tail -n 5
