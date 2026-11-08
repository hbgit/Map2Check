#!/bin/bash
# test_wrapper_core_dumps.sh -- the wrapper must start map2check with core dumps off.
#
# On the competition machines BenchExec keeps every file the tool writes in a
# tmpfs that counts toward the run's memory limit, and it leaves RLIMIT_CORE
# as it found it (measured: `ulimit -c` is unlimited inside the container).
# Map2Check replays vectors natively, and a replay that reaches abort() can
# then drop a core file the size of the process into that tmpfs. A core is of
# no use to a test generator or a verifier, so the wrapper turns them off for
# itself and everything it starts. Both modes are checked: they build their
# command lines separately.
#
# No build needed: the wrapper is copied next to a fake map2check that records
# the limit it was started with.

set -u

REPO=$(cd "$(dirname "$0")/../.." && pwd)
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

echo "=== Wrapper: core dumps ==="

cp "$REPO/utils/map2check-wrapper.py" "$WORK/"
cat > "$WORK/map2check" <<EOF
#!/bin/bash
ulimit -c > "$WORK/core-limit"
mkdir -p test-suite
echo "VERIFICATION UNKNOWN"
EOF
chmod +x "$WORK/map2check"
echo 'int main(void) { return 0; }' > "$WORK/task.c"
echo 'COVER( init(main()), FQL(COVER EDGES(@DECISIONEDGE)) )' > "$WORK/cover.prp"
echo 'CHECK( init(main()), LTL(G ! overflow) )' > "$WORK/check.prp"

# The test itself must start where a competition run starts: with no limit.
ulimit -c unlimited 2>/dev/null || ulimit -c "$(ulimit -H -c)"

for mode in cover check; do
  rm -f "$WORK/core-limit"
  ( cd "$WORK" && python3 map2check-wrapper.py -p "$mode.prp" task.c ) \
    > "$WORK/$mode.log" 2>&1
  limit=$(cat "$WORK/core-limit" 2>/dev/null || echo "not started")
  if [ "$limit" = "0" ]; then
    ok "$mode: map2check starts with core dumps off"
  else
    fail "$mode: core dumps" "ulimit -c was '$limit', expected 0"
    tail -3 "$WORK/$mode.log" | sed 's/^/    /'
  fi
done

echo "  ---"
echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ] || exit 1
