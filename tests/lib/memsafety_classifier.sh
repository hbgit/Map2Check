# shellcheck shell=bash
# classify_memsafety_result <expected true|false> <subproperty> <verdict>
# <verdict> is classify_map2check_verdict's output. A FALSE counts as correct
# only when its kind matches the subproperty the task declares; FALSE of the
# wrong kind is a wrong answer, not a lucky one.
classify_memsafety_result() {
  local expected="$1" sub="$2" verdict="$3" want=""
  case "$sub" in
    valid-deref) want="FALSE-DEREF" ;;
    valid-free) want="FALSE-FREE" ;;
    valid-memtrack) want="FALSE-MEMTRACK" ;;
    valid-memcleanup) want="FALSE-MEMCLEANUP" ;;
    # MemSafety tasks that declare no subproperty (sv-benchmarks' Juliet_Test):
    # any memory-safety FALSE answers them, a leak-at-exit or overflow does not.
    any) case "$verdict" in FALSE-DEREF|FALSE-FREE|FALSE-MEMTRACK) want="$verdict" ;; esac ;;
  esac
  case "$verdict" in
    TRUE) [ "$expected" = "true" ] && echo correct-true || echo wrong-true ;;
    FALSE*) if [ "$expected" = "false" ] && [ "$verdict" = "$want" ]; then
              echo correct-false; else echo wrong-false; fi ;;
    ERROR) echo error ;;
    *) echo unknown ;;
  esac
}
