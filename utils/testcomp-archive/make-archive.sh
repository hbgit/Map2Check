#!/bin/bash
# Builds the Test-Comp tool archive: a self-contained Map2Check directory that
# runs on the competition machines (Ubuntu 24.04, clang 18, no LLVM 16, no KLEE,
# no AFL++), zipped as the competition expects (one top-level directory).
# The host provides only python3 and gcc-multilib, the linker and C runtime
# that AFL++'s native build needs (required_ubuntu_packages in FM-Tools).
#
# Run inside the development image, where the install and its dependencies
# live (Dockerfile.dev, or map2check-dev:aflpp):
#
#   docker run --rm -v "$PWD:/workspace" -w /workspace map2check-dev:aflpp \
#     utils/testcomp-archive/make-archive.sh /workspace/install_v9b /workspace/map2check.zip
#
# What goes in, and why:
#   - the install (map2check, its passes and runtime bitcode);
#   - LLVM 16's clang, opt and llvm-link with clang's resource directory: the
#     frontend compiles and links bitcode with them, and the image has clang 18;
#   - KLEE 3.1 with its runtime (klee-uclibc included);
#   - AFL++: afl-fuzz, afl-cc and its passes and runtime objects, which the
#     wrapper points to with MAP2CHECK_AFL_CC, MAP2CHECK_AFL_FUZZ and AFL_PATH;
#   - every shared library those need that is not glibc, with each ELF's
#     RUNPATH rewritten to find them. Not LD_LIBRARY_PATH: it would leak into
#     the system's bash, gcc and timeout, and a library from Ubuntu 22.04
#     loaded under them could break them.
# Optional engines (Clam for --add-invariants, sbt-slicer for --slice) stay out:
# both are off by default and absent from the competition configuration.
set -euo pipefail

# Absolute paths: the zip runs from inside the staging directory, where a
# relative output name would create the archive only to have the cleanup
# delete it -- after `rm -f "$OUT"` removed the caller's previous one (review
# of #73).
INSTALL=$(realpath -m "${1:?install directory}")
OUT=$(realpath -m "${2:?output zip}")
[ -d "$(dirname "$OUT")" ] || { echo "no such directory: $(dirname "$OUT")" >&2; exit 2; }
AFL_ROOT=${AFL_ROOT:-/usr/local}
LLVM=${LLVM:-/usr/lib/llvm-16}
KLEE=${KLEE:-/opt/klee}
HERE=$(cd "$(dirname "$0")" && pwd)

command -v patchelf > /dev/null || { apt-get update -qq && apt-get install -y -qq patchelf > /dev/null; }
command -v zip > /dev/null || { apt-get update -qq && apt-get install -y -qq zip > /dev/null; }

STAGE=$(mktemp -d)
T=$STAGE/map2check
mkdir -p "$T"/{bin,lib,include,lib/afl}

# --- Map2Check -------------------------------------------------------------
cp -a "$INSTALL/map2check" "$T/"
cp -aL "$INSTALL/lib/." "$T/lib/" 2>/dev/null || true
rm -rf "$T/lib/clang" "$T/lib/klee" "$T/lib/libz3*"
cp "$HERE/../map2check-wrapper.py" "$T/"
chmod +x "$T/map2check-wrapper.py"

# --- LLVM 16 ---------------------------------------------------------------
for tool in clang opt llvm-link; do cp -L "$LLVM/bin/$tool" "$T/bin/"; done
mkdir -p "$T/lib/clang"
cp -aL "$LLVM/lib/clang/16" "$T/lib/clang/"
cp -aL "$LLVM/lib/clang/16/include/." "$T/include/"

# --- KLEE ------------------------------------------------------------------
cp -L "$KLEE/bin/klee" "$KLEE/bin/kleaver" "$T/bin/"
cp -L "$KLEE/bin/ktest-tool" "$T/bin/" 2>/dev/null || true
mkdir -p "$T/lib/klee"
cp -aL "$KLEE/lib/klee/runtime" "$T/lib/klee/"

# --- AFL++ -----------------------------------------------------------------
cp -L "$AFL_ROOT/bin/afl-fuzz" "$AFL_ROOT/bin/afl-cc" "$T/bin/"
# Copies, not symlinks: an archive without links unpacks the same with any zip
# tool. afl-cc picks its mode from argv[0], so a copy behaves as the link would.
cp "$T/bin/afl-cc" "$T/bin/afl-clang-fast"
cp -aL "$AFL_ROOT/lib/afl/." "$T/lib/afl/"

# --- shared libraries ------------------------------------------------------
# Everything ldd resolves outside glibc (and the loader), for every ELF here.
is_glibc() {
  case "$(basename "$1")" in
    libc.so*|libm.so*|libdl.so*|libpthread.so*|librt.so*|ld-linux*|libresolv.so*|libutil.so*) return 0 ;;
  esac
  return 1
}
elves=$(find "$T" -type f \( -perm -u+x -o -name '*.so*' \) -exec sh -c 'head -c4 "$1" | grep -q "ELF"' _ {} \; -print)
for elf in $elves; do
  { ldd "$elf" 2>/dev/null || true; } | awk '/=> \//{print $3}' | while read -r lib; do
    is_glibc "$lib" && continue
    [ -e "$T/lib/$(basename "$lib")" ] || cp -L "$lib" "$T/lib/"
  done
done
# Libraries bring their own dependencies: one more round over lib/.
for lib in "$T"/lib/*.so*; do
  { ldd "$lib" 2>/dev/null || true; } | awk '/=> \//{print $3}' | while read -r dep; do
    is_glibc "$dep" && continue
    [ -e "$T/lib/$(basename "$dep")" ] || cp -L "$dep" "$T/lib/"
  done
done

# --- RUNPATH ---------------------------------------------------------------
for elf in $(find "$T" -type f -exec sh -c 'head -c4 "$1" | grep -q "ELF"' _ {} \; -print); do
  dir=$(dirname "$elf")
  rel=$(realpath --relative-to="$dir" "$T/lib")
  # Relocatable objects (afl-compiler-rt.o) and static archives have no RUNPATH.
  if patchelf --print-rpath "$elf" > /dev/null 2>&1; then
    patchelf --set-rpath "\$ORIGIN/$rel" "$elf"
  fi
done

# --- smoke test, README, licenses ------------------------------------------
# What the FM-Tools CI and the Test-Comp rules ask of the archive: a
# smoketest.sh that runs the tool on examples shipped with it, a README, and a
# LICENSE that allows anybody to reproduce the evaluation -- here Map2Check's
# own plus one for every third-party component it carries.
cp "$HERE/smoketest.sh" "$T/" && chmod +x "$T/smoketest.sh"
cp -r "$HERE/examples" "$T/"
cp "$HERE/README.txt" "$T/README"
cp "$HERE/../../LICENSE" "$T/LICENSE"
mkdir -p "$T/licenses"
cp "$HERE"/licenses/* "$T/licenses/"
cp /opt/klee-uclibc/COPYING.LIB "$T/licenses/klee-uclibc.COPYING.LIB"
# The Debian copyright file of every bundled library and LLVM tool that came
# from an Ubuntu package (the ones built from source are in licenses/ above),
# and of clang's resource headers (lib/clang/16, include/). dpkg records some
# libraries under /lib, the merged-/usr path under /usr/lib.
declare -A DEB_PKG # file in the archive -> its Debian package
debian_package() { # <absolute path on this system>
  local real; real=$(readlink -f "$1")
  { dpkg -S "$real" 2>/dev/null || dpkg -S "${real/#\/usr\/lib/\/lib}" 2>/dev/null; } | cut -d: -f1 | head -1
}
for f in "$T"/lib/*.so* "$T"/bin/clang "$T"/bin/opt "$T"/bin/llvm-link; do
  name=$(basename "$f")
  for dir in /usr/lib/x86_64-linux-gnu /lib/x86_64-linux-gnu "$LLVM/lib" "$LLVM/bin"; do
    [ -f "$dir/$name" ] || continue
    pkg=$(debian_package "$dir/$name")
    if [ -n "$pkg" ] && [ -f "/usr/share/doc/$pkg/copyright" ]; then
      cp "/usr/share/doc/$pkg/copyright" "$T/licenses/$pkg.copyright"
      DEB_PKG[${f#"$T"/}]=$pkg
    fi
    break
  done
done
headers_pkg=$(debian_package "$LLVM/lib/clang/16/include/stddef.h")
cp "/usr/share/doc/$headers_pkg/copyright" "$T/licenses/$headers_pkg.copyright"
# Debian copyright files name their licenses and point to the full text in
# /usr/share/common-licenses; the texts travel with them.
mkdir -p "$T/licenses/common-licenses"
for name in $(grep -oh "/usr/share/common-licenses/[A-Za-z0-9.+_-]*" "$T"/licenses/*.copyright \
              | sed 's#.*/##; s/[.,]*$//' | sort -u); do
  if [ -f "/usr/share/common-licenses/$name" ]; then
    cp "/usr/share/common-licenses/$name" "$T/licenses/common-licenses/"
  else
    echo "a Debian copyright file names /usr/share/common-licenses/$name, which is missing" >&2
    exit 1
  fi
done

# --- license coverage ------------------------------------------------------
# Every file in the archive belongs to a component, and every component's
# license texts are in it: the archive is not built otherwise. The table is
# licenses/README.md; this is its executable form.
need() { for l in "$@"; do [ -f "$T/$l" ] || { echo "missing license $l" >&2; return 1; }; done; }
uncovered=()
while IFS= read -r -d '' file; do
  rel=${file#"$T"/}
  case "$rel" in
    licenses/*) continue ;;
    LICENSE|README|smoketest.sh|map2check|map2check-wrapper.py|examples/*|lib/*Pass.so|lib/libMap2CheckLibrary.so|lib/*.bc)
      need LICENSE ;;
    bin/klee|bin/kleaver|bin/ktest-tool)
      need licenses/KLEE-3.1.LICENSE.TXT ;;
    lib/klee/runtime/klee-uclibc.bca)
      need licenses/klee-uclibc.COPYING.LIB ;;
    lib/klee/runtime/*)
      need licenses/KLEE-3.1.LICENSE.TXT licenses/KLEE-3.1-runtime-NOTICES.txt licenses/LLVM-16.LICENSE.TXT ;;
    bin/afl-*|lib/afl/*)
      need licenses/AFLplusplus-4.40c.LICENSE licenses/AFLplusplus-4.40c-THIRD-PARTY.txt licenses/LLVM-16.LICENSE.TXT ;;
    lib/libstp.so*)
      need licenses/STP-2.3.4.LICENSE licenses/STP-2.3.4.LICENSE_COMPONENTS licenses/Artistic-1.0-Perl.txt licenses/LGPL-2.0.txt ;;
    lib/libminisat.so*)
      need licenses/MiniSat.LICENSE ;;
    lib/clang/*|include/*)
      need "licenses/$headers_pkg.copyright" licenses/LLVM-16.LICENSE.TXT ;;
    *)
      if [ -n "${DEB_PKG[$rel]:-}" ]; then
        need "licenses/${DEB_PKG[$rel]}.copyright"
        case "${DEB_PKG[$rel]}" in clang-*|llvm-*|libllvm*|libclang*) need licenses/LLVM-16.LICENSE.TXT ;; esac
      else
        uncovered+=("$rel")
      fi ;;
  esac || exit 1
done < <(find "$T" -type f -print0)
if [ "${#uncovered[@]}" -gt 0 ]; then
  echo "files with no license component (add a rule above and to licenses/README.md):" >&2
  printf '  %s\n' "${uncovered[@]}" >&2
  exit 1
fi
echo "license coverage: $(find "$T" -type f ! -path "$T/licenses/*" | wc -l) files, each with its license"

# --- check and zip ---------------------------------------------------------
missing=$(for elf in "$T"/map2check "$T"/bin/* "$T"/lib/*.so* "$T"/lib/afl/*.so; do
  [ -f "$elf" ] || continue
  { ldd "$elf" 2>/dev/null || true; } | grep "not found" | sed "s#^#$(basename "$elf"): #" || true
done | sort -u)
if [ -n "$missing" ]; then echo "unresolved libraries:"; echo "$missing"; exit 1; fi

rm -f "$OUT"
if [ -n "$(find "$T" -type l)" ]; then echo "symlinks left in the archive:"; find "$T" -type l; exit 1; fi
(cd "$STAGE" && zip -qr "$OUT" map2check)
echo "archive: $OUT ($(du -h "$OUT" | cut -f1), $(find "$T" -type f | wc -l) files)"
rm -rf "$STAGE"
