#!/bin/bash
# test_ktest_object_names.sh -- symbolic objects must be named by their type.
#
# KLEE writes one .ktest per explored path, and each holds that path's input
# vector: the symbolic objects in the order the program consumed them. That is
# exactly the material a Cover-Branches suite is made of, produced by the
# engine at no cost and without perturbing the search.
#
# What a .ktest records per object is a NAME, a SIZE and the bytes -- not a
# type. So the name is the only place the type can travel, and it got there by
# accident of a macro:
#
#     klee_make_symbolic(&non_det, sizeof(non_det), "non_det_#type");
#
# `#type` inside a string literal is not the stringify operator, it is the two
# characters # and t. Every object in every .ktest was called "non_det_#type",
# identical for an int, a double and a char. Nothing read it, so nothing broke,
# and it survived every baseline this tool has run.
#
# This test exists because the bug was invisible: the suites were correct, the
# verdicts were correct, and only a consumer of the .ktest files would ever
# notice. Now there is one.

set -u

MAP2CHECK_DIR="${MAP2CHECK_PATH:-/workspace/install_e2e}"
MAP2CHECK="$MAP2CHECK_DIR/map2check"
KTEST_TOOL="$MAP2CHECK_DIR/bin/ktest-tool"

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

# Two different types, so a name that ignored the type would collide visibly.
# The sizes differ too (4 and 1), which is what makes the collision harmful:
# without the name there is nothing to say whether one byte is a char or the
# low byte of something wider.
cat > "$WORK/typed.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern char __VERIFIER_nondet_char(void);
extern void reach_error(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  char c = __VERIFIER_nondet_char();
  if (a == 42 && c == 'z') { reach_error(); }
  return 0;
}
EOF

echo "=== KLEE symbolic object names ==="

if [ ! -x "$KTEST_TOOL" ]; then
  fail "ktest-tool available" "not found at $KTEST_TOOL"
  echo "  Results: $PASSED passed, $FAILED failed"
  exit 1
fi

# --debug keeps the scratch directory, which is where klee-last lives.
( cd "$WORK" && MAP2CHECK_PATH="$MAP2CHECK_DIR" timeout -k 10 250 \
    "$MAP2CHECK" --target-function --target-function-name reach_error \
    --nondet-generator symex --debug --timeout 120 typed.c ) \
  > "$WORK/run.log" 2>&1

scratch=$(find "$WORK" -maxdepth 1 -name '*.map2check' -print -quit)
if [ -z "$scratch" ]; then
  fail "scratch directory kept" "no *.map2check directory under --debug"
  echo "  Results: $PASSED passed, $FAILED failed"
  exit 1
fi

# -L: klee-last is a symlink to klee-out-N, and find does not descend into a
# symlinked start point without it. Without the -L this reported "no ktest
# files" on a run that had produced three.
mapfile -t ktests < <(find -L "$scratch/klee-last" -name '*.ktest' 2>/dev/null | sort)
if [ "${#ktests[@]}" -ge 1 ]; then
  ok "KLEE produced per-path test vectors (${#ktests[@]})"
else
  fail "ktest files" "none produced"
  echo "  Results: $PASSED passed, $FAILED failed"
  exit 1
fi

# The last one is the deepest path, the one that satisfied both conditions.
dump=$("$KTEST_TOOL" "${ktests[-1]}" 2>&1)

if echo "$dump" | grep -q "non_det_#type"; then
  fail "object names carry the type" \
       "still the literal 'non_det_#type' -- the macro does not stringify"
else
  ok "no object is named with the unexpanded macro text"
fi

if echo "$dump" | grep -q "name: 'non_det_int'"; then
  ok "the int read is named non_det_int"
else
  fail "int object name" "$(echo "$dump" | grep -m1 name)"
fi

if echo "$dump" | grep -q "name: 'non_det_char'"; then
  ok "the char read is named non_det_char"
else
  fail "char object name" "$(echo "$dump" | grep -m2 name | tail -1)"
fi

# Names alone are not enough: the pairing of name to size is what lets a
# consumer decode the bytes. A char named non_det_int would be worse than no
# name at all.
if echo "$dump" | grep -A1 "name: 'non_det_char'" | grep -q "size: 1"; then
  ok "non_det_char is one byte, as its name promises"
else
  fail "char object size" "not 1"
fi

echo "  ---"
echo "  Results: $PASSED passed, $FAILED failed"
[ "$FAILED" -eq 0 ] || exit 1
