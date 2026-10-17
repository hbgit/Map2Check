#!/usr/bin/env bash
# CMake/install layer of the Rust runtime: only libmap2check.a (and optionally
# map2check-klee.bc) is installed, no C runtime bitcode; configure fails loudly
# without a library or with an unreadable bitcode.
#
# Env: MAP2CHECK_RUST_LIB_DIR  directory with the runtime-profile libmap2check.a
#                              (target/runtime), or the lib/ of the release
#                              archive (required)
#      CMAKE_ARGS              extra configure flags (e.g. -G Ninja -DLLVM_DIR=...)
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
LIB="${MAP2CHECK_RUST_LIB_DIR:?set MAP2CHECK_RUST_LIB_DIR}"
read -r -a EXTRA <<<"${CMAKE_ARGS:-}"
W="$(mktemp -d)"
trap 'rm -rf "$W"' EXIT
fail() { echo "FAIL: $*" >&2; exit 1; }

install_tree() { # <name> <extra cmake args...> ; prints nothing, installs to $W/<name>-p
  local n="$1"; shift
  cmake -S "$ROOT" -B "$W/$n-b" "${EXTRA[@]}" -DCMAKE_INSTALL_PREFIX="$W/$n-p" "$@" >/dev/null
  cmake --build "$W/$n-b" -j >/dev/null
  cmake --install "$W/$n-b" >/dev/null
}

# 1. Only the Rust static library is installed; no C runtime .bc.
# The runtime build (features libfuzzer,wasm) in $LIB, whatever its layout.
if [ -f "$LIB/libmap2check_runtime.a" ]; then RT="$LIB/libmap2check_runtime.a"; else RT="$LIB/libmap2check.a"; fi
install_tree plain -DMAP2CHECK_RUST_LIB_DIR="$LIB"
P="$W/plain-p"
test -f "$P/lib/libmap2check.a" || fail "libmap2check.a not installed"
if ls "$P"/lib/*.bc 2>/dev/null | grep .; then fail "bitcode installed without MAP2CHECK_RUST_KLEE_BC"; fi
if find "$P" -name '*.bc' | grep -E 'map2check|memtrack|bbtrack|nondet|caller|container|analysis|list|btree|AnalysisMode' ; then
  fail "C runtime bitcode still installed"
fi

# 1b. Release archive layout (lib/ of the map2check-library tarball): the
# runtime build, libmap2check_runtime.a, is the one installed as
# libmap2check.a; the featureless core libmap2check.a next to it is not.
R="$W/release-lib"; mkdir "$R"
cp "$RT" "$R/libmap2check_runtime.a"
echo 'not the runtime' >"$R/libmap2check.a"
install_tree release -DMAP2CHECK_RUST_LIB_DIR="$R"
cmp -s "$W/release-p/lib/libmap2check.a" "$R/libmap2check_runtime.a" ||
  fail "release layout: libmap2check_runtime.a not installed as libmap2check.a"

# 1c. Local build layout (target/runtime): only libmap2check.a, and it is the
# runtime build, so it is accepted under the plain name.
T="$W/local-lib"; mkdir "$T"
cp "$RT" "$T/libmap2check.a"
install_tree local -DMAP2CHECK_RUST_LIB_DIR="$T"
cmp -s "$W/local-p/lib/libmap2check.a" "$RT" ||
  fail "local layout: the runtime-profile libmap2check.a not installed"

# 1d. Only a featureless core libmap2check.a (no libmap2check_runtime.a next
# to it): configure must fail instead of linking a runtime without the
# libfuzzer and wasm entry points.
check_core_rejected() { # <dir> <label>
  if cmake -S "$ROOT" -B "$W/core-$2" "${EXTRA[@]}" -DMAP2CHECK_RUST_LIB_DIR="$1" >"$W/core-$2.log" 2>&1; then
    fail "configure should reject the core libmap2check.a ($2)"
  fi
  # CMake wraps the message; join its lines before matching.
  tr -s ' \n' ' ' <"$W/core-$2.log" | grep -q "is not the Map2Check runtime build" ||
    fail "no clear message for the core libmap2check.a ($2)"
}
C="$W/core-synth"; mkdir "$C"
echo 'int map2check_init(void) { return 0; }' >"$W/core.c"
cc -c "$W/core.c" -o "$W/core.o"
ar rcs "$C/libmap2check.a" "$W/core.o"
check_core_rejected "$C" synth
if [ -f "$LIB/libmap2check_runtime.a" ] && [ -f "$LIB/libmap2check.a" ]; then
  # The release archive's own core build, alone in a directory.
  C2="$W/core-release"; mkdir "$C2"
  cp "$LIB/libmap2check.a" "$C2/libmap2check.a"
  check_core_rejected "$C2" release
fi

# 2. A readable KLEE bitcode is installed as lib/map2check-klee.bc.
LLVM_DIS="${LLVM_DIS:-llvm-dis-16}"; LLVM_AS="${LLVM_AS:-llvm-as-16}"
echo 'define i32 @f() { ret i32 0 }' >"$W/ok.ll"
"$LLVM_AS" "$W/ok.ll" -o "$W/ok.bc"
install_tree klee -DMAP2CHECK_RUST_LIB_DIR="$LIB" -DMAP2CHECK_RUST_KLEE_BC="$W/ok.bc"
P="$W/klee-p"
test -f "$P/lib/map2check-klee.bc" || fail "map2check-klee.bc not installed"
if ls "$P"/lib/*.bc | grep -vE 'map2check-klee\.bc$'; then fail "extra bitcode installed"; fi

# 3. No library: configure must fail, with a clear message.
E="$W/empty"; mkdir "$E"
if cmake -S "$ROOT" -B "$W/bad1" "${EXTRA[@]}" -DMAP2CHECK_RUST_LIB_DIR="$E" >"$W/bad1.log" 2>&1; then
  fail "configure should fail without libmap2check.a"
fi
grep -q "libmap2check.a not found" "$W/bad1.log" || fail "no clear message for missing library"
if cmake -S "$ROOT" -B "$W/bad2" "${EXTRA[@]}" -DMAP2CHECK_RUST_LIB_DIR= -DMAP2CHECK_RUST_SRC= >"$W/bad2.log" 2>&1; then
  fail "configure should fail with neither MAP2CHECK_RUST_LIB_DIR nor MAP2CHECK_RUST_SRC"
fi
grep -q "MAP2CHECK_RUST_LIB_DIR or MAP2CHECK_RUST_SRC" "$W/bad2.log" || fail "no clear message for unset variables"

# 4. Unreadable bitcode: configure must fail.
echo 'not bitcode' >"$W/bad.bc"
if cmake -S "$ROOT" -B "$W/bad3" "${EXTRA[@]}" -DMAP2CHECK_RUST_LIB_DIR="$LIB" -DMAP2CHECK_RUST_KLEE_BC="$W/bad.bc" >"$W/bad3.log" 2>&1; then
  fail "configure should fail with an unreadable bitcode"
fi
grep -q "cannot be read" "$W/bad3.log" || fail "no clear message for unreadable bitcode"
if cmake -S "$ROOT" -B "$W/bad4" "${EXTRA[@]}" -DMAP2CHECK_RUST_LIB_DIR="$LIB" -DMAP2CHECK_RUST_KLEE_BC="$W/missing.bc" >"$W/bad4.log" 2>&1; then
  fail "configure should fail with a missing bitcode file"
fi
echo ok
