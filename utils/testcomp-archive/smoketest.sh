#!/bin/sh
# Smoke test of the Test-Comp archive (required by the FM-Tools CI): runs
# Map2Check on the two examples in examples/, one per Test-Comp property, and
# checks that each run writes a test suite in the exchange format and that the
# engines ran: AFL++ in both, KLEE in Cover-Branches. Exits 0 only if all do.
#
# The engine checks matter because a missing host package does not stop the
# tool: without a native linker and C runtime (gcc-multilib) the AFL++ binary
# fails to build, Map2Check gives the whole budget to KLEE, and a suite is
# still written.
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
TOOL="$HERE/map2check-wrapper.py"

"$TOOL" --version

run() { # <program> <property> <budget>
  work=$(mktemp -d)
  # The wrapper exits 0 whenever Map2Check ran (a verdict, no verdict, or the
  # budget spent): any other status is a failure, even with a suite on disk.
  status=0
  (cd "$work" && "$TOOL" -p "$HERE/examples/$2" --data-model LP64 --budget "$3" \
    "$HERE/examples/$1") > "$work/run.log" 2>&1 || status=$?
  if [ "$status" -ne 0 ]; then
    echo "FAIL $1: the wrapper exited with status $status" >&2
    cat "$work/run.log" >&2
    rm -rf "$work"
    exit 1
  fi
  if [ ! -f "$work/test-suite/metadata.xml" ]; then
    echo "FAIL $1: no test-suite/metadata.xml" >&2
    cat "$work/run.log" >&2
    rm -rf "$work"
    exit 1
  fi
  echo "$work"
}

fail() { # <work> <message>
  echo "FAIL $2" >&2
  cat "$1/run.log" >&2
  rm -rf "$1"
  exit 1
}

# The run log must show that <engine> ran. Not the "Executing ..." banners:
# map2check prints those before launching the engine, so they appear when it
# fails to start too, and the other engine can still produce the suite (review
# of #86). These lines are printed after the run, from what the engine itself
# wrote: afl-fuzz's execution count in fuzzer_stats, KLEE's .ktest files.
ran() { # <work> <program> <engine>
  case "$3" in
    AFL++) line="AFL++ ran: [1-9][0-9]* executions" ;;
    KLEE) line="KLEE ran: [1-9][0-9]* tests generated" ;;
  esac
  grep -q "$line" "$1/run.log" || fail "$1" "$2: $3 did not run"
}

# Cover-Error: the suite must hold a test case with the input that reaches
# reach_error (x == 42). AFL++ finds it in its first window, so KLEE is not
# required here.
work=$(run cover_error.c coverage-error-call.prp 60)
ran "$work" cover_error.c AFL++
grep -q "<input>42</input>" "$work"/test-suite/testcase-*.xml 2>/dev/null ||
  fail "$work" "cover_error.c: no test case with the input 42"
echo "ok cover_error.c"
rm -rf "$work"

# Cover-Branches: both engines take a turn, and the suite must hold at least
# one test case. A smaller budget leaves no window for KLEE after AFL++'s;
# the run still ends in seconds, once KLEE has explored the four paths.
work=$(run cover_branches.c coverage-branches.prp 60)
ran "$work" cover_branches.c AFL++
ran "$work" cover_branches.c KLEE
ls "$work"/test-suite/testcase-*.xml > /dev/null 2>&1 ||
  fail "$work" "cover_branches.c: no test case"
echo "ok cover_branches.c"
rm -rf "$work"

echo "smoke test passed"
