#!/usr/bin/env bash
# Downloads the pinned map2check-library release (the Rust runtime Map2Check
# links), checks its SHA-256 and extracts it into DEST (default
# /opt/map2check-library): DEST/include/map2check.h and DEST/lib/*.a.
# Configure Map2Check with -DMAP2CHECK_RUST_LIB_DIR=DEST/lib.
#
# The pin lives here only; CI (.github/workflows/ci.yml) and Dockerfile.dev
# both call this script.
#
# Usage: scripts/runtime/fetch-map2check-library.sh [DEST]
set -euo pipefail

M2C_LIB_VERSION="1.1.0-rc1"
M2C_LIB_TARGET="x86_64-unknown-linux-gnu"
M2C_LIB_SHA256="dd90e8885288a0de03d4d2b8624b6cb4a8ebee9f53ed9adcbf42501910209666"
M2C_LIB_NAME="map2check-library-${M2C_LIB_VERSION}-${M2C_LIB_TARGET}"
M2C_LIB_URL="https://github.com/hbgit/map2check-library/releases/download/v${M2C_LIB_VERSION}/${M2C_LIB_NAME}.tar.gz"

dest="${1:-/opt/map2check-library}"
die() { echo "fetch-map2check-library: $*" >&2; exit 1; }

work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT
archive="$work/${M2C_LIB_NAME}.tar.gz"

wget -q "$M2C_LIB_URL" -O "$archive" || die "download failed: $M2C_LIB_URL"
echo "${M2C_LIB_SHA256}  ${archive}" | sha256sum -c --quiet - ||
  die "SHA-256 mismatch for ${M2C_LIB_NAME}.tar.gz"

mkdir -p "$dest"
tar xzf "$archive" -C "$dest" --strip-components=1 --no-same-owner
for f in include/map2check.h lib/libmap2check_runtime.a; do
  [ -f "$dest/$f" ] || die "$dest/$f missing after extraction"
done
grep -qxF "#define MAP2CHECK_VERSION \"${M2C_LIB_VERSION}\"" "$dest/include/map2check.h" ||
  die "$dest/include/map2check.h is not version ${M2C_LIB_VERSION}"
echo "fetch-map2check-library: ${M2C_LIB_NAME} -> $dest"
