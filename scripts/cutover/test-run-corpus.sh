#!/usr/bin/env bash
# Testa run-corpus.sh com um binário falso e um corpus de uma tarefa.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
T="$(mktemp -d)"; trap 'rm -rf "$T"' EXIT
mkdir -p "$T/bin" "$T/cases/x"
cat > "$T/bin/map2check" <<'EOS'
#!/usr/bin/env bash
printf 'FALSE-DEREF\nLine: 7\nFunction: main\n' > map2check_property
echo "VERIFICATION FAILED"
EOS
chmod +x "$T/bin/map2check"
printf 'int main(){}\n' > "$T/cases/x/a.c"
cat > "$T/cases/x/a.yml" <<'EOS'
format_version: '1.0'
input_files: 'a.c'
properties:
  - property_file: ../properties/valid-memsafety.prp
    expected_verdict: false
EOS
CASES="$T/cases" GENS=fuzzer REPS=1 "$HERE/run-corpus.sh" "$T/bin" "$T/out.tsv"
row="$(sed -n 2p "$T/out.tsv")"
IFS=$'\t' read -r task prp gen rep verdict tag line func witness wall rss <<<"$row"
[ "$task" = "x/a.yml" ] && [ "$prp" = "valid-memsafety" ] && [ "$gen" = fuzzer ] \
  && [ "$verdict" = FAILED ] && [ "$tag" = FALSE-DEREF ] && [ "$line" = 7 ] \
  && [ "$func" = main ] && [ "$witness" = 0 ] || { echo "linha inesperada: $row" >&2; exit 1; }
echo ok
