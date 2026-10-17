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

# 1d. A libmap2check.a that is not the runtime build (no
# libmap2check_runtime.a next to it): configure must fail instead of linking
# a runtime without the libfuzzer and/or wasm entry points.
reject_lib_dir() { # <dir> <label> <expected message> [extra cmake args...]
  local d="$1" n="$2" msg="$3"; shift 3
  if cmake -S "$ROOT" -B "$W/rej-$n" "${EXTRA[@]}" -DMAP2CHECK_RUST_LIB_DIR="$d" "$@" >"$W/rej-$n.log" 2>&1; then
    fail "configure should reject $d/libmap2check.a ($n)"
  fi
  # CMake wraps the message; join its lines before matching.
  tr -s ' \n' ' ' <"$W/rej-$n.log" | grep -q "$msg" ||
    fail "no clear message for $d/libmap2check.a ($n)"
}
NOT_RT="is not the Map2Check runtime build"
synth_lib() { # <dir> <C source>: a one-object archive as <dir>/libmap2check.a
  mkdir -p "$1"; printf '%s\n' "$2" >"$1/x.c"
  cc -c "$1/x.c" -o "$1/x.o"; ar rcs "$1/libmap2check.a" "$1/x.o"; rm -f "$1/x.c" "$1/x.o"
}
# Core build: neither symbol.
synth_lib "$W/core-synth" 'int map2check_init(void) { return 0; }'
reject_lib_dir "$W/core-synth" core-synth "$NOT_RT"
# libfuzzer build: LLVMFuzzerTestOneInput but no map2check_wasm_malloc.
synth_lib "$W/fuzz-synth" 'int LLVMFuzzerTestOneInput(const unsigned char *d, unsigned long n) { return (int)(d != 0) + (int)n; }'
reject_lib_dir "$W/fuzz-synth" fuzz-synth "does not define map2check_wasm_malloc"
if [ -f "$LIB/libmap2check_runtime.a" ]; then
  # The release archive's own core and libfuzzer builds, each alone.
  for b in libmap2check libmap2check_libfuzzer; do
    [ -f "$LIB/$b.a" ] || continue
    mkdir "$W/rel-$b"; cp "$LIB/$b.a" "$W/rel-$b/libmap2check.a"
    reject_lib_dir "$W/rel-$b" "rel-$b" "$NOT_RT"
  done
fi
# No nm can read the archive: fail closed, and say so.
reject_lib_dir "$T" no-nm "no nm could read the archive" \
  -DCMAKE_NM=/bin/false -DM2C_NM=/bin/false -DM2C_LLVM_NM=/bin/false

# 1e. MAP2CHECK_RUST_SRC: cargo runs on every build, and the archive it leaves
# in target/runtime is checked. A fake cargo stands in for the real one
# (M2C_CARGO is the cached find_program result); it logs each call.
src_case() { # <name> <cargo body>; configures, prints the build's exit code
  local n="$1" S="$W/src-$1"
  mkdir -p "$S/target/runtime"
  cp "$W/core-synth/libmap2check.a" "$S/target/runtime/libmap2check.a"  # stale core build
  printf '#!/bin/sh\necho "$*" >>"%s/cargo.log"\n%s\n' "$S" "$2" >"$S/cargo"; chmod +x "$S/cargo"
  cmake -S "$ROOT" -B "$W/src-$n-b" "${EXTRA[@]}" -DMAP2CHECK_RUST_LIB_DIR= -DMAP2CHECK_RUST_SRC="$S" \
    -DMAP2CHECK_RUST_KLEE_BC= -DM2C_CARGO="$S/cargo" >"$W/src-$n.log" 2>&1 || fail "SRC route: configure failed ($n)"
  local rc=0
  cmake --build "$W/src-$n-b" --target Map2CheckRustRuntime >>"$W/src-$n.log" 2>&1 || rc=$?
  echo "$rc"
}
# cargo leaves the stale core archive alone: the build must fail.
[ "$(src_case stale ':')" != 0 ] || fail "SRC route: stale core archive accepted"
grep -q "build --profile runtime --features libfuzzer,wasm" "$W/src-stale/cargo.log" ||
  fail "SRC route: cargo not run although target/runtime/libmap2check.a existed"
tr -s ' \n' ' ' <"$W/src-stale.log" | grep -q "$NOT_RT" || fail "SRC route: no clear message for the stale archive"
# cargo rebuilds it as the runtime build: the build passes, the archive is
# the runtime one, and a second build runs cargo again.
[ "$(src_case rebuilt "cp '$RT' '$W/src-rebuilt/target/runtime/libmap2check.a'")" = 0 ] ||
  fail "SRC route: rebuilt runtime archive rejected"
cmp -s "$W/src-rebuilt/target/runtime/libmap2check.a" "$RT" || fail "SRC route: archive not the rebuilt one"
cmake --build "$W/src-rebuilt-b" --target Map2CheckRustRuntime >>"$W/src-rebuilt.log" 2>&1 ||
  fail "SRC route: second build failed"
[ "$(wc -l <"$W/src-rebuilt/cargo.log")" -eq 2 ] || fail "SRC route: cargo not run on every build"

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
