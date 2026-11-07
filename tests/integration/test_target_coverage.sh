#!/bin/bash
# test_target_coverage.sh -- reachability targets that are never called.
#
# TargetPass instruments CALL SITES whose callee matches --target-function-name.
# If the program never calls that function, nothing is instrumented, and the
# reachability property holds for a reason that has nothing to do with the
# program: the target simply is not there.
#
# That is not an error, and this test does not treat it as one. A program that
# genuinely never calls reach_error IS safe with respect to reach_error, and
# TRUE is the right answer. The defect is that such a TRUE is indistinguishable
# in the output from a TRUE earned by exploring the program.
#
# That indistinguishability is finding B, and it is not hypothetical. The
# CASTLE harness mapped seven CWEs onto `--target-function-name main`; a
# program does not call its own entry point, so zero sites were instrumented.
# On the v6 baseline, 98 of 217 runs went through that mode: 57 TRUE, 22
# TIMEOUT, 9 ERROR, 10 UNKNOWN -- and not one FALSE, across 59 programs known
# to be vulnerable. Twelve of them were scored as true negatives.
#
# So the contract asserted here is about VISIBILITY: the trivial case must
# announce itself, and the real case must not cry wolf.

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

# One program, used for both cases. It really does call reach_error, so the
# difference between the two runs below is only which name is asked for --
# which is the point: the same program is trivially safe with respect to a
# target it does not contain, and genuinely analysable with respect to one it
# does.
cat > "$WORK/prog.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 7) {
    reach_error();
  }
  return 0;
}
EOF

MARKER="is never called in this module"

echo "=== Target coverage warning ==="

# --- Case 1: a target the program really calls -------------------------------
# The budget is small on purpose. The warning is emitted during instrumentation,
# long before any solving, so this test never needs the analysis to finish.
out_real=$( cd "$WORK" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 5 90 \
  "$MAP2CHECK" --target-function --target-function-name reach_error \
  --timeout 10 prog.c 2>&1 )

if echo "$out_real" | grep -q "$MARKER"; then
  fail "a called target stays quiet" \
       "warned about reach_error, which this program does call"
else
  ok "a called target stays quiet"
fi

# --- Case 2: the degenerate target (finding B) -------------------------------
out_main=$( cd "$WORK" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 5 90 \
  "$MAP2CHECK" --target-function --target-function-name main \
  --timeout 10 prog.c 2>&1 )

if echo "$out_main" | grep -q "$MARKER"; then
  ok "an uncalled target announces itself"
else
  fail "an uncalled target announces itself" \
       "no warning for --target-function-name main, which nothing calls"
fi

# The warning has to name the target. A generic "target not found" would send
# whoever reads the log looking for the wrong thing.
if echo "$out_main" | grep -q "'main'"; then
  ok "the warning names the target it is about"
else
  fail "the warning names the target it is about" "target name absent"
fi

# And it has to say what the verdict is worth, not just that something is
# missing. "TRUE means the target is absent, not that the program is safe" is
# the part a reader acts on.
if echo "$out_main" | grep -qi "not that the program is safe"; then
  ok "the warning says what a TRUE verdict is worth here"
else
  fail "the warning says what a TRUE verdict is worth here" \
       "no statement about the meaning of the verdict"
fi

echo "  ---"
echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ] || exit 1
