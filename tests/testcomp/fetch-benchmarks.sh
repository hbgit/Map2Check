#!/bin/bash
# fetch-benchmarks.sh -- clones sv-benchmarks for the stratified Test-Comp runs.
#
# Not a submodule and not committed: the checkout is ~13 GB, it is needed only
# for evaluation runs, and pinning it as a submodule would put that weight on
# every clone of Map2Check including CI, which does not use it.
#
# --depth 1 on purpose. The corpus is used as a snapshot of programs, never for
# its history, and the full history is several times the size again.

set -eu

HERE="$(cd "$(dirname "$0")" && pwd)"
DEST="$HERE/bench/sv-benchmarks"
REPO="${SV_BENCHMARKS_REPO:-https://gitlab.com/sosy-lab/benchmarking/sv-benchmarks.git}"

if [ -d "$DEST/c" ]; then
  echo "sv-benchmarks already present at $DEST"
  exit 0
fi

mkdir -p "$HERE/bench"
echo "Cloning sv-benchmarks (~13 GB, shallow) into $DEST ..."
git clone --depth 1 "$REPO" "$DEST"
echo "Done. Build a corpus with:"
echo "  python3 $HERE/build_corpus.py --property cover-error --per-category 40 \\"
echo "      --out $HERE/corpus/cover-error.tsv"
