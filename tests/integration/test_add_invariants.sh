#!/bin/bash
# test_add_invariants.sh -- the --add-invariants contract.
#
# This file has two jobs, and the second is unusual.
#
# The first is ordinary: --add-invariants must never be accepted and then
# ignored without a trace. That was the state issue #54 describes, and it held
# on the .bc input path -- which is how the entire v5 baseline and the CASTLE
# harness invoke the tool. The guard that was supposed to warn lived inside
# `if (!is_llvmir_in)`, so bitcode input skipped it entirely.
#
# The second job is to detect PROGRESS. The invariant generator is absent
# today, so the ABSENT assertions below are what passes. Once Clam dev16 is
# built into the image, the same run must instead succeed AND inject
# verifier.assume calls into the bitcode. The script probes which state it is
# in and asserts the contract for that state, printing the state either way.
# A green run means "the contract holds"; the CAPABILITY line says which
# contract that was, so the day invariants come back is visible in the CI log
# rather than inferred.
#
# Deliberately not done: parking a red check that is expected to be red. A
# permanently failing job gets ignored, and then nobody notices when it goes
# green -- which is exactly the transition this test exists to catch.

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

# A loop with an inductive invariant (sum >= 0), which is the shape abstract
# interpretation is good at and a fuzzer is not. Once Clam is in place this is
# also a program where the invariant is worth something.
cat > "$WORK/loop.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  int i = 0;
  int sum = 0;
  if (n < 0 || n > 100) return 0;
  while (i < n) { sum += i; i++; }
  if (sum < 0) { reach_error(); }
  return 0;
}
EOF

"$CLANG" -c -emit-llvm -g -o "$WORK/loop.bc" "$WORK/loop.c" 2>/dev/null

echo "============================================================"
echo "--add-invariants contract"
echo "============================================================"

# --debug keeps the scratch directory, which is the only place the intermediate
# bitcode exists: cleanGarbage() deletes it on a normal run, so without this
# there is nothing left to inspect for injected invariants.
run_flag() {
  local input="$1" rc=0
  ( cd "$WORK" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 180 \
      "$MAP2CHECK" --debug --target-function --target-function-name reach_error \
      --add-invariants --nondet-generator symex --timeout 60 "$input" ) \
    > "$WORK/out.txt" 2>&1 || rc=$?
  echo "$rc"
}

rc_bc=$(run_flag loop.bc)
out_bc=$(cat "$WORK/out.txt")

if [ "$rc_bc" -eq 3 ]; then
  CAPABILITY="ABSENT"
elif [ "$rc_bc" -eq 0 ]; then
  CAPABILITY="PRESENT"
else
  CAPABILITY="BROKEN"
fi
echo "  CAPABILITY: invariant generator is $CAPABILITY (exit $rc_bc)"

case "$CAPABILITY" in
  ABSENT)
    # The contract while Clam is not built.
    if echo "$out_bc" | grep -qi "add-invariants"; then
      ok "the refusal names the flag that caused it"
    else
      fail "diagnostic" "exit 3 with no mention of --add-invariants"
    fi

    # A refusal that does not say why, and does not say what to do about it,
    # just moves the confusion from "it did nothing" to "it failed".
    if echo "$out_bc" | grep -qiE "no invariant generator|not built|unavailable|not installed"; then
      ok "the refusal says the generator is missing"
    else
      fail "diagnostic" "no explanation of why it refused"
    fi

    if echo "$out_bc" | grep -q "ENABLE_CLAM"; then
      ok "the refusal says how to fix it"
    else
      fail "diagnostic" "no remedy offered (-DENABLE_CLAM=ON)"
    fi

    # The silence bug itself: .c input already warned, .bc did not. Both paths
    # must behave identically now.
    rc_c=$(run_flag loop.c)
    if [ "$rc_c" -eq 3 ]; then
      ok ".c input refuses the same way .bc does (issue #54)"
    else
      fail "input path parity" ".bc exited 3 but .c exited $rc_c"
    fi

    # Runs that do not ask for invariants must be untouched.
    rc_plain=0
    ( cd "$WORK" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 180 \
        "$MAP2CHECK" --target-function --target-function-name reach_error \
        --nondet-generator symex --timeout 60 loop.bc ) \
      > "$WORK/plain.txt" 2>&1 || rc_plain=$?
    if [ "$rc_plain" -ne 3 ]; then
      ok "runs without --add-invariants are unaffected"
    else
      fail "collateral damage" "a run without the flag also exited 3"
    fi
    ;;

  PRESENT)
    # The contract once Clam dev16 is in place. Reaching this branch at all is
    # the progress signal: it means the migration landed.
    ok "--add-invariants completes successfully"

    # Succeeding is not enough. Without this assertion the flag could go back
    # to being a no-op, only now wearing a success exit code -- which is worse
    # than the bug this test was written for, because it looks correct.
    # The scratch directory (<sha1>.map2check/) holds every intermediate: the
    # Clam-compiled bitcode carrying verifier.assume, and the instrumented one
    # where NonDetPass has rewritten it to map2check_crab_assume. Either symbol
    # proves the invariants reached the pipeline.
    found=""
    while IFS= read -r bc; do
      if llvm-dis-16 -o - "$bc" 2>/dev/null \
           | grep -q 'verifier\.assume\|map2check_crab_assume'; then
        found="$bc"
        break
      fi
    done < <(find "$WORK" -name '*.bc' -type f 2>/dev/null | sort)

    if [ -n "$found" ]; then
      ok "the produced bitcode carries invariant assumes ($(basename "$found"))"
    else
      n_bc=$(find "$WORK" -name '*.bc' -type f 2>/dev/null | wc -l)
      fail "invariant injection" \
           "exit 0 but no assume symbol in any of the $n_bc bitcode files"
    fi
    ;;

  BROKEN)
    fail "capability probe" "exit $rc_bc is neither 3 (absent) nor 0 (present)"
    echo "$out_bc" | tail -20
    ;;
esac

echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed  [capability: $CAPABILITY]"
echo "============================================================"
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
