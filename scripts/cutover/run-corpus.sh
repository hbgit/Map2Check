#!/usr/bin/env bash
# Executa o corpus de regressão e grava, por tarefa/propriedade/gerador/repetição,
# o veredito, a tag de map2check_property, linha, função, witness, tempo e RSS.
# Uso: run-corpus.sh <dir-com-map2check> <saida.tsv>
set -euo pipefail
BIN_DIR="$(cd "$1" && pwd)"; OUT="$2"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CASES="${CASES:-$ROOT/tests/regression_test/test_cases/sv-benchmarks}"
GENS="${GENS:-afl symex}"; REPS="${REPS:-5}"; TIMEOUT="${TIMEOUT:-45}"

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
            timeout "$((TIMEOUT + 15))" "$BIN_DIR/map2check" --timeout "$TIMEOUT" --nondet-generator "$gen" $flags "$input" \
            > out.txt 2>&1 ) || true
        verdict=TIMEOUT
        grep -q 'VERIFICATION SUCCEEDED' "$work/out.txt" && verdict=SUCCEEDED
        grep -q 'VERIFICATION FAILED' "$work/out.txt" && verdict=FAILED
        grep -q 'VERIFICATION UNKNOWN' "$work/out.txt" && verdict=UNKNOWN
        tag=-; line=-; func=-
        # tag/line/function are only meaningful for a FAILED verdict; every other
        # verdict gets "-" whichever source (property file or stdout) was used.
        if [ "$verdict" = FAILED ]; then
          raw=
          ptag=; [ -f "$work/map2check_property" ] && ptag="$(sed -n 1p "$work/map2check_property")"
          case "$ptag" in
            ""|NONE|UNKNOWN) ptag= ;;
          esac
          if [ -n "$ptag" ]; then
            # Rust runtime: the property file is left in the cwd.
            tag="$ptag"
            line="$(sed -nE 's/^Line: *//p' "$work/map2check_property" | head -1)"; line="${line:--}"
            func="$(sed -nE 's/^Function: *//p' "$work/map2check_property" | head -1)"; func="${func:--}"
          elif [ -f "$work/map2check_property_memtrack" ]; then
            # Same precedence as the frontend (modules/frontend/utils/tools.cpp:198-207):
            # map2check_property_memtrack is checked before map2check_property_memcleanup.
            tag=FALSE-MEMTRACK
          elif [ -f "$work/map2check_property_memcleanup" ]; then
            tag=FALSE-MEMCLEANUP
          else
            # C runtime: the property file lives in a scratch dir that is deleted, so
            # parse stdout (modules/frontend/counter_example/counter_example.hpp:115-170):
            #   Violated property:
            #   <TAB>file map2check_property line N function F   (absent for MEMTRACK/MEMCLEANUP)
            #   <TAB>FALSE-DEREF: Reference to pointer was lost | FALSE-FREE: ... | OVERFLOW |
            #   <TAB>FALSE: Target Reached | FALSE-ASSERT | FALSE-MEMTRACK | FALSE-MEMCLEANUP |
            #   <TAB>FALSE-DIVBYZERO (lines 152-157)
            clean="$(sed -E 's/\x1b\[[0-9;]*m//g' "$work/out.txt")"
            viol="$(printf '%s\n' "$clean" | sed -n '/^Violated property:/,/^VERIFICATION/p')"
            loc="$(printf '%s\n' "$viol" | sed -nE 's/^[[:space:]]*file .* line ([0-9]+) function (.*)$/\1 \2/p' | head -1)"
            [ -n "$loc" ] && { line="${loc%% *}"; func="${loc#* }"; func="${func:--}"; }
            case "$viol" in
              *FALSE-DEREF*) tag=FALSE-DEREF ;;
              *FALSE-FREE*) tag=FALSE-FREE ;;
              *FALSE-MEMTRACK*) tag=FALSE-MEMTRACK ;;
              *FALSE-MEMCLEANUP*) tag=FALSE-MEMCLEANUP ;;
              *FALSE-DIVBYZERO*) tag=FALSE-DIVBYZERO ;;
              *FALSE-ASSERT*) tag=ASSERT ;;
              *"FALSE: Target Reached"*) tag=TARGET-REACHED ;;
              *OVERFLOW*) tag=OVERFLOW ;;
              *) tag=UNMAPPED
                 raw="$(printf '%s' "$viol" | tr '\n\t' '  ')"
                 echo "run-corpus: UNMAPPED property for ${yml#"$CASES"/} ($prp, $gen): $raw" >&2 ;;
            esac
          fi
        fi
        witness=0; ls "$work"/*.graphml "$work"/map2check_checked_error >/dev/null 2>&1 && witness=1
        grep -qE 'Generating witness|\.graphml' "$work/out.txt" && witness=1
        timing="$(tail -1 "$work/time.txt" 2>/dev/null || true)"
        case "$timing" in *$'\t'*) ;; *) timing=$'-\t-' ;; esac
        printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
          "${yml#"$CASES"/}" "$prp" "$gen" "$rep" "$verdict" "$tag" "$line" "$func" "$witness" "$timing" >> "$OUT"
        rm -rf "$work"
      done
    done
  done < <(sed -nE 's#.*property_file: *.*/([a-z-]+)\.prp.*#\1#p' "$yml")
done < <(find "$CASES" -name '*.yml' | sort)
