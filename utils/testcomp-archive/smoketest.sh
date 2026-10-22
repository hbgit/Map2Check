#!/bin/sh
# Smoke test of the Test-Comp archive (required by the FM-Tools CI): runs
# Map2Check on the two examples in examples/, one per Test-Comp property, and
# checks that each run writes a test suite in the exchange format. Exits 0
# only if both do.
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
TOOL="$HERE/map2check-testcomp-wrapper.py"

"$TOOL" --version

run() { # <program> <property> <budget>
  work=$(mktemp -d)
  (cd "$work" && "$TOOL" -p "$HERE/examples/$2" --data-model LP64 --budget "$3" \
    "$HERE/examples/$1") > "$work/run.log" 2>&1 || true
  if [ ! -f "$work/test-suite/metadata.xml" ]; then
    echo "FAIL $1: no test-suite/metadata.xml" >&2
    cat "$work/run.log" >&2
    rm -rf "$work"
    exit 1
  fi
  echo "$work"
}

# Cover-Error: the suite must hold a test case with the input that reaches
# reach_error (x == 42).
work=$(run cover_error.c coverage-error-call.prp 60)
if ! grep -q "<input>42</input>" "$work"/test-suite/testcase-*.xml 2>/dev/null; then
  echo "FAIL cover_error.c: no test case with the input 42" >&2
  cat "$work/run.log" >&2
  rm -rf "$work"
  exit 1
fi
echo "ok cover_error.c"
rm -rf "$work"

# Cover-Branches: the suite must hold at least one test case.
work=$(run cover_branches.c coverage-branches.prp 30)
if ! ls "$work"/test-suite/testcase-*.xml > /dev/null 2>&1; then
  echo "FAIL cover_branches.c: no test case" >&2
  cat "$work/run.log" >&2
  rm -rf "$work"
  exit 1
fi
echo "ok cover_branches.c"
rm -rf "$work"

echo "smoke test passed"
