#!/usr/bin/env bash
# Testa run-corpus.sh com um binário falso e um corpus de uma tarefa.
# Os fixtures *-synthetic.txt espelham o formato exato impresso por
# modules/frontend/counter_example/counter_example.hpp: FALSE-FREE (l.115-123),
# TARGET-REACHED (l.124-133), OVERFLOW (l.143-150), FALSE-DIVBYZERO (l.152-159),
# FALSE-MEMTRACK/MEMCLEANUP (l.160-171, sem linha "file ... line ... function"),
# ASSERT (l.172-181). c-unmapped inventa uma tag desconhecida. c-failed/c-succeeded
# sao stdout capturado de uma execucao real do runtime C.
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
CASES="$T/cases" GENS=afl REPS=1 "$HERE/run-corpus.sh" "$T/bin" "$T/out.tsv"
row="$(sed -n 2p "$T/out.tsv")"
IFS=$'\t' read -r task prp gen rep verdict tag line func witness wall rss <<<"$row"
[ "$task" = "x/a.yml" ] && [ "$prp" = "valid-memsafety" ] && [ "$gen" = afl ] \
  && [ "$verdict" = FAILED ] && [ "$tag" = FALSE-DEREF ] && [ "$line" = 7 ] \
  && [ "$func" = main ] && [ "$witness" = 0 ] || { echo "linha inesperada: $row" >&2; exit 1; }


FIX="$HERE/fixtures"
mkdir -p "$T/cbin"
cat > "$T/cbin/map2check" <<'EOS'
#!/usr/bin/env bash
cat "$FIXTURE"
EOS
chmod +x "$T/cbin/map2check"
rm -f "$T/cases/x/a.yml"
cp "$T/cases/x/a.c" "$T/cases/x/b.c"
cat > "$T/cases/x/b.yml" <<'EOS'
format_version: '1.0'
input_files: 'b.c'
properties:
  - property_file: ../properties/valid-memsafety.prp
    expected_verdict: false
EOS

# fixture | verdict | tag | line | function  (caminho C: so stdout)
while IFS='|' read -r fx exp_verdict exp_tag exp_line exp_func; do
  FIXTURE="$FIX/$fx" CASES="$T/cases" GENS=symex REPS=1 "$HERE/run-corpus.sh" "$T/cbin" "$T/c.tsv" 2>"$T/err"
  row="$(sed -n 2p "$T/c.tsv")"
  [ "$(awk -F'\t' '{print NF}' <<<"$row")" = 11 ] || { echo "colunas != 11: $row" >&2; exit 1; }
  IFS=$'\t' read -r task prp gen rep verdict tag line func witness wall rss <<<"$row"
  [ "$verdict" = "$exp_verdict" ] && [ "$tag" = "$exp_tag" ] && [ "$line" = "$exp_line" ] \
    && [ "$func" = "$exp_func" ] && [ "$witness" = 0 ] \
    || { echo "$fx inesperado: $verdict $tag $line $func $witness" >&2; exit 1; }
  if [ "$exp_tag" = UNMAPPED ]; then grep -q 'UNMAPPED.*FALSE-SOMETHING-NEW' "$T/err" || { echo "sem aviso UNMAPPED" >&2; exit 1; }; fi
done <<'EOS'
c-failed-stdout.txt|FAILED|FALSE-DEREF|108|is_relaxed_prefix
c-succeeded-stdout.txt|SUCCEEDED|-|-|-
c-false-free-synthetic.txt|FAILED|FALSE-FREE|42|foo
c-false-memtrack-synthetic.txt|FAILED|FALSE-MEMTRACK|-|-
c-false-memcleanup-synthetic.txt|FAILED|FALSE-MEMCLEANUP|-|-
c-overflow-synthetic.txt|FAILED|OVERFLOW|42|foo
c-assert-synthetic.txt|FAILED|ASSERT|42|foo
c-target-reached-synthetic.txt|FAILED|TARGET-REACHED|42|foo
c-false-divbyzero-synthetic.txt|FAILED|FALSE-DIVBYZERO|42|foo
c-unmapped-synthetic.txt|FAILED|UNMAPPED|42|foo
EOS

# Caminho Rust: arquivos de propriedade no cwd. spec: nome-do-arquivo|conteudo (\n) | tag | line | func
V="VERIFICATION FAILED"
mkrust() { # $1 = shell snippet que cria arquivos
  cat > "$T/cbin/map2check" <<EOS
#!/usr/bin/env bash
$1
echo "$V"
EOS
}
check_rust() { # desc exp_verdict exp_tag exp_line exp_func
  CASES="$T/cases" GENS=afl REPS=1 "$HERE/run-corpus.sh" "$T/cbin" "$T/r.tsv"
  IFS=$'\t' read -r task prp gen rep verdict tag line func witness wall rss <<<"$(sed -n 2p "$T/r.tsv")"
  [ "$verdict" = "$2" ] && [ "$tag" = "$3" ] && [ "$line" = "$4" ] && [ "$func" = "$5" ] \
    || { echo "Rust '$1' inesperado: $verdict $tag $line $func" >&2; exit 1; }
}
V="VERIFICATION SUCCEEDED"
mkrust "printf 'NONE\n' > map2check_property"
check_rust "NONE em SUCCEEDED" SUCCEEDED - - -
V="VERIFICATION FAILED"
mkrust "printf 'FALSE-MEMTRACK\n' > map2check_property_memtrack"
check_rust "so memtrack" FAILED FALSE-MEMTRACK - -
mkrust "printf 'x\n' > map2check_property_memcleanup"
check_rust "so memcleanup" FAILED FALSE-MEMCLEANUP - -
cat > "$T/cbin/map2check" <<'EOS'
#!/usr/bin/env bash
printf 'OVERFLOW\nLine: 3\nFunction: f\n' > map2check_property
echo "VERIFICATION UNKNOWN"
EOS
check_rust "violacao com veredito UNKNOWN" UNKNOWN - - -

echo ok
