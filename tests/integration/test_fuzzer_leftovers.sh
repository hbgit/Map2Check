#!/bin/bash
# test_fuzzer_leftovers.sh -- no fuzzing target outlives afl-fuzz.
#
# afl-fuzz starts its forkservers with setsid(), so the targets live in
# sessions of their own, outside the process group that `timeout` signals.
# When afl-fuzz is SIGKILLed (timeout -k), a running target survives it and
# keeps consuming the run's CPU time after Map2Check has finished: in a 900 s
# BenchExec run, 870 s of wall time became 908 s of CPU time and a TIMEOUT.
#
# A fake afl-fuzz (MAP2CHECK_AFL_FUZZ) reproduces exactly that: it leaves a
# process named like the target in a new session, then exits. Map2Check must
# kill it when the fuzzer phase ends.

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
WORK=$(mktemp -d)
LEFT="$WORK/leftover.pid"
cleanup() {
  [ -s "$LEFT" ] && kill -9 "$(cat "$LEFT")" 2>/dev/null
  rm -rf "$WORK"
}
trap cleanup EXIT

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

echo "=== Fuzzer phase: leftover targets ==="

cat > "$WORK/afl-fuzz" <<EOF
#!/bin/bash
# The target follows "--" on the command line.
while [ \$# -gt 0 ] && [ "\$1" != "--" ]; do shift; done
target=\$2
setsid bash -c 'echo \$\$ > "$LEFT"; exec -a "\$0" sleep 300' "\$target" < /dev/null > /dev/null 2>&1 &
for i in \$(seq 1 50); do [ -s "$LEFT" ] && break; sleep 0.1; done
exit 0
EOF
chmod +x "$WORK/afl-fuzz"

cat > "$WORK/task.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 10) return 1;
  return 0;
}
EOF

(cd "$WORK" && MAP2CHECK_AFL_FUZZ="$WORK/afl-fuzz" \
   "$MAP2CHECK_DIR/map2check" --cover-branches --generate-test-suite \
   --nondet-generator afl --timeout 30 task.c > map2check.out 2>&1)

if [ ! -s "$LEFT" ]; then
  fail "the fake afl-fuzz ran" "no leftover was started (see map2check.out)"
  sed -n '1,40p' "$WORK/map2check.out"
else
  ok "the fake afl-fuzz left a target behind"
  pid=$(cat "$LEFT")
  if kill -0 "$pid" 2>/dev/null; then
    fail "no target outlives the fuzzer" "pid $pid is still running"
  else
    ok "no target outlives the fuzzer"
  fi
fi

echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ]
