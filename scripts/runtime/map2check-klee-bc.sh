#!/usr/bin/env bash
# Builds the KLEE (symex) runtime bitcode of the pinned map2check-library
# release, which the release archive does not ship: a checkout of the tag,
# verified against its commit, then the library's own
# scripts/build-klee-runtime.sh. Configure Map2Check with
# -DMAP2CHECK_RUST_KLEE_BC=<out.bc>.
#
# Needs rustup with the 1.72.1 toolchain installed (its LLVM is 16), the LLVM
# 16 tools (opt-16, llvm-dis-16, llvm-nm-16, llvm-as-16, lli-16), python3, git.
#
# Usage: scripts/runtime/map2check-klee-bc.sh <out.bc>
set -euo pipefail

M2C_LIB_TAG="v1.1.0-rc1"
M2C_LIB_COMMIT="49a25e40fe8b52932db1f0adfdfa896bac65fb5c"
M2C_LIB_REPO="https://github.com/hbgit/map2check-library.git"

die() { echo "map2check-klee-bc: $*" >&2; exit 1; }
[ $# -eq 1 ] || die "usage: $0 <out.bc>"
out="$(realpath -m "$1")"

work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT
git -c advice.detachedHead=false clone -q --depth 1 --branch "$M2C_LIB_TAG" "$M2C_LIB_REPO" "$work/src"
got="$(git -C "$work/src" rev-parse HEAD)"
[ "$got" = "$M2C_LIB_COMMIT" ] || die "$M2C_LIB_TAG is $got, expected $M2C_LIB_COMMIT"

mkdir -p "$(dirname "$out")"
CARGO_TARGET_DIR="$work/target" "$work/src/scripts/build-klee-runtime.sh" "$out"
