#!/bin/bash
# test_test_suite_emission.sh -- the --generate-test-suite contract.
#
# Asserts structure, not coverage. Whether the suite actually reproduces the
# bug is TestCov's question and is asked by a separate gate; what is checked
# here is the part nothing else covers -- that the files exist, are well-formed
# XML, carry the right public identifiers, and that the input sequence is the
# sequence the program consumed.
#
# The ordering assertion is the important one. A suite with the right values in
# the wrong order is structurally perfect and worth exactly zero points, and
# nothing about the run looks wrong when it happens.

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
CLANG="${CLANG:-/usr/bin/clang-16}"

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

# Two nondet reads, one error reachable only when both match. The suite must
# therefore carry 42 then 7, in that order.
cat > "$WORK/target.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  if (a == 42) {
    if (b == 7) {
      reach_error();
    }
  }
  return 0;
}
EOF

echo "============================================================"
echo "Test-Comp test-suite emission"
echo "============================================================"

( cd "$WORK" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 300 \
    "$MAP2CHECK" --target-function --target-function-name reach_error \
    --generate-test-suite --nondet-generator symex --timeout 120 target.c ) \
  > "$WORK/run.log" 2>&1

SUITE="$WORK/test-suite"

if [ -d "$SUITE" ]; then
  ok "test-suite/ directory created"
else
  fail "suite directory" "not created"
  tail -15 "$WORK/run.log"
  echo "Results: $PASSED passed, $FAILED failed"
  exit 1
fi

[ -f "$SUITE/metadata.xml" ] && ok "metadata.xml written" \
                             || fail "metadata.xml" "absent"

n_tc=$(find "$SUITE" -name 'testcase-*.xml' | wc -l)
[ "$n_tc" -ge 1 ] && ok "at least one testcase file ($n_tc)" \
                  || fail "testcase files" "none written"

# Well-formedness checked by a real parser, not by grepping. Python's expat is
# used rather than xmllint because it ships with the image; the DOCTYPE is
# external and must not be fetched, hence the no-op entity resolver.
bad=0
for f in "$SUITE"/*.xml; do
  python3 - "$f" <<'PY' || { bad=1; echo "    malformed: $f"; }
import sys, xml.parsers.expat
p = xml.parsers.expat.ParserCreate()
p.ExternalEntityRefHandler = lambda *a: 1
try:
    with open(sys.argv[1], 'rb') as fh:
        p.ParseFile(fh)
except Exception as e:
    print("   ", e)
    sys.exit(1)
PY
done
[ "$bad" -eq 0 ] && ok "every emitted file is well-formed XML" \
                 || fail "well-formedness" "a file failed to parse"

grep -q 'DTD test-format test-metadata 1.1//EN' "$SUITE/metadata.xml" \
  && ok "metadata carries the 1.1 public identifier" \
  || fail "metadata doctype" "public identifier missing or wrong"

grep -q '<sourcecodelang>C</sourcecodelang>' "$SUITE/metadata.xml" \
  && ok "sourcecodelang is C" || fail "sourcecodelang" "wrong or missing"

grep -qE '<programhash>[0-9a-f]{64}</programhash>' "$SUITE/metadata.xml" \
  && ok "programhash is 64 hex characters" \
  || fail "programhash" "not a SHA-256 digest"

# It must be the hash OF THE PROGRAM, so sha256sum has to agree exactly.
want=$(sha256sum "$WORK/target.c" | cut -d' ' -f1)
got=$(sed -n 's:.*<programhash>\(.*\)</programhash>.*:\1:p' "$SUITE/metadata.xml")
[ "$want" = "$got" ] && ok "programhash matches sha256sum of the program" \
                     || fail "programhash value" "want $want, got $got"

first_tc=$(find "$SUITE" -name 'testcase-*.xml' | sort | head -1)

grep -q 'DTD test-format testcase 1.1//EN' "$first_tc" \
  && ok "testcase carries the 1.1 public identifier" \
  || fail "testcase doctype" "public identifier missing or wrong"

grep -q 'coversError="true"' "$first_tc" \
  && ok "the violating test case is marked coversError" \
  || fail "coversError" "not set on a run that found the error"

mapfile -t inputs < <(sed -n 's:.*<input>\(.*\)</input>.*:\1:p' "$first_tc")
if [ "${#inputs[@]}" -eq 2 ] && [ "${inputs[0]}" = "42" ] && [ "${inputs[1]}" = "7" ]; then
  ok "inputs are 42 then 7, in consumption order"
else
  fail "input vector" "expected [42, 7], got [${inputs[*]}]"
fi

# --- the nondet log's own shape -----------------------------------------------
# The suite is only as good as the CSV it is serialized from, and that CSV is
# written by the C runtime, where nothing else checks it. An UNSIGNED row used
# to come out with EIGHT fields instead of seven: the UNSIGNED arm of the writer
# was an `if` with no `else`, so the value was printed once as %u and again as
# %d. It parsed by accident -- the value column is first, so it still landed at
# index 5 -- while the type column silently moved to index 7.
#
# Checked on __VERIFIER_nondet_unsigned specifically: __VERIFIER_nondet_uint is
# tagged UINT, a different enumerator, and takes the arm that was always right.
#
# Runs in its own subdirectory, because --debug keeps the scratch directory and
# the cleanup assertion below looks for exactly that at depth 1 of $WORK.
UWORK="$WORK/unsigned_run"
mkdir -p "$UWORK"
cat > "$UWORK/unsigned.c" <<'EOF'
extern unsigned __VERIFIER_nondet_unsigned(void);
extern void reach_error(void);
int main(void) {
  unsigned u = __VERIFIER_nondet_unsigned();
  if (u == 5u) { reach_error(); }
  return 0;
}
EOF

(
  cd "$UWORK" || exit 1
  MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 200 "$MAP2CHECK" \
      --target-function --target-function-name reach_error \
      --nondet-generator symex --debug --timeout 120 unsigned.c
) > "$UWORK/unsigned.log" 2>&1

csv=$(find "$UWORK" -name klee_log.csv -print -quit)
if [ -z "$csv" ]; then
  fail "unsigned nondet log" "no klee_log.csv produced"
else
  malformed=$(awk -F';' 'NF != 7 {print NF" fields: "$0}' "$csv" | head -1)
  if [ -z "$malformed" ]; then
    ok "every nondet log row has exactly 7 fields"
  else
    fail "nondet log row shape" "$malformed"
  fi

  # The type column is the one the duplicated value displaced. UNSIGNED is 5 in
  # enum NONDET_TYPE, and it has to be readable as the LAST field.
  last=$(awk -F';' 'NR==1{print $7}' "$csv")
  if [ "$last" = "5" ]; then
    ok "the type column survives as the last field (UNSIGNED=5)"
  else
    fail "type column" "last field is '$last', expected 5"
  fi
fi

# The suite must survive cleanGarbage(), which removes the scratch directory.
if [ -z "$(find "$WORK" -maxdepth 1 -name '*.map2check' -print -quit)" ]; then
  ok "scratch directory cleaned and the suite survived"
else
  fail "cleanup" "scratch directory left behind"
fi

echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
