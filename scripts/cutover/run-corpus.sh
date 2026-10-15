#!/usr/bin/env bash
# Executa o corpus de regressão e grava, por tarefa/propriedade/gerador/repetição,
# o veredito, a tag de map2check_property, linha, função, witness, tempo e RSS.
# Uso: run-corpus.sh <dir-com-map2check> <saida.tsv>
set -euo pipefail
BIN_DIR="$(cd "$1" && pwd)"; OUT="$2"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CASES="${CASES:-$ROOT/tests/regression_test/test_cases/sv-benchmarks}"
GENS="${GENS:-fuzzer symex}"; REPS="${REPS:-5}"; TIMEOUT="${TIMEOUT:-45}"

flags_for() {
  case "$1" in
    valid-memsafety) echo "--memtrack" ;;
    valid-memcleanup) echo "--memtrack --memcleanup-property" ;;
    no-overflow) echo "--check-overflow" ;;
    unreach-call) echo "--target-function" ;;
    *) return 1 ;;
  esac
}

printf 'task\tprp\tgen\trep\tverdict\ttag\tline\tfunction\twitness\twall_s\tmaxrss_kb\n' > "$OUT"
while IFS= read -r yml; do
  dir="$(dirname "$yml")"
  input="$dir/$(sed -nE "s/^input_files: *'?([^' ]+)'?.*/\1/p" "$yml")"
  while IFS= read -r prp; do
    flags="$(flags_for "$prp")" || continue
    for gen in $GENS; do
      for rep in $(seq 1 "$REPS"); do
        work="$(mktemp -d)"
        # shellcheck disable=SC2086
        ( cd "$work" && /usr/bin/time -f '%e\t%M' -o time.txt \
            timeout "$TIMEOUT" "$BIN_DIR/map2check" --nondet-generator "$gen" $flags "$input" \
            > out.txt 2>&1 ) || true
        verdict=TIMEOUT
        grep -q 'VERIFICATION SUCCEEDED' "$work/out.txt" && verdict=SUCCEEDED
        grep -q 'VERIFICATION FAILED' "$work/out.txt" && verdict=FAILED
        grep -q 'VERIFICATION UNKNOWN' "$work/out.txt" && verdict=UNKNOWN
        tag=-; line=-; func=-
        if [ -f "$work/map2check_property" ]; then
          tag="$(sed -n 1p "$work/map2check_property")"
          line="$(sed -nE 's/^Line: *//p' "$work/map2check_property" | head -1)"; line="${line:--}"
          func="$(sed -nE 's/^Function: *//p' "$work/map2check_property" | head -1)"; func="${func:--}"
        fi
        witness=0; ls "$work"/*.graphml "$work"/map2check_checked_error >/dev/null 2>&1 && witness=1
        timing="$(tail -1 "$work/time.txt" 2>/dev/null || printf -- '-\t-')"
        printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
          "${yml#"$CASES"/}" "$prp" "$gen" "$rep" "$verdict" "$tag" "$line" "$func" "$witness" "$timing" >> "$OUT"
        rm -rf "$work"
      done
    done
  done < <(sed -nE 's#.*property_file: *.*/([a-z-]+)\.prp.*#\1#p' "$yml")
done < <(find "$CASES" -name '*.yml' | sort)
