# Licenses of the Map2Check Test-Comp archive

Every file in the archive belongs to one of the components below, and the
archive carries the license texts of each. `make-archive.sh` enforces this
table: it refuses to build an archive with a file that matches no row, or
with a license file of a row missing.

| Component | Version / source | Files in the archive | License | Texts in `licenses/` |
|---|---|---|---|---|
| Map2Check | this repository | `map2check`, `map2check-wrapper.py`, `smoketest.sh`, `examples/`, `README`, `lib/lib*Pass.so`, `lib/libMap2CheckLibrary.so`, `lib/*.bc` (runtime) | GPL-2.0 | `../LICENSE` |
| KLEE | 3.1 (`fe22b90`) | `bin/klee`, `bin/kleaver`, `bin/ktest-tool` | NCSA | `KLEE-3.1.LICENSE.TXT` |
| KLEE runtime | 3.1 (`runtime/`) | `lib/klee/runtime/libkleeRuntime*.bca`, `libkleeUBSan64*.bca` | NCSA; 9 files of klee-libc and Freestanding under BSD (University of California, FreeBSD); the UBSan runtime from LLVM compiler-rt | `KLEE-3.1.LICENSE.TXT`, `KLEE-3.1-runtime-NOTICES.txt` (the 9 notices, verbatim), `LLVM-16.LICENSE.TXT` |
| klee-uclibc | `klee/klee-uclibc` | `lib/klee/runtime/klee-uclibc.bca` | LGPL-2.1 | `klee-uclibc.COPYING.LIB` |
| AFL++ | 4.40c | `bin/afl-*`, `lib/afl/*` | Apache-2.0; t1ha (zlib-style) and xxHash (BSD-2-Clause) compiled into `afl-fuzz`; `SanitizerCoveragePCGUARD.so` from LLVM | `AFLplusplus-4.40c.LICENSE`, `AFLplusplus-4.40c-THIRD-PARTY.txt`, `LLVM-16.LICENSE.TXT` |
| STP | 2.3.4 | `lib/libstp.so.2.3` | MIT; components Bit::Vector (Artistic / GPL / LGPL), CVC's SMT-LIB parser, ABC (UC) | `STP-2.3.4.LICENSE`, `STP-2.3.4.LICENSE_COMPONENTS`, `Artistic-1.0-Perl.txt`, `LGPL-2.0.txt`, GPL-2 in `../LICENSE` |
| MiniSat | `stp/minisat` (`62a143a`) | `lib/libminisat.so.2` | MIT | `MiniSat.LICENSE` |
| LLVM / Clang 16 | Ubuntu 22.04 packages | `bin/clang`, `bin/opt`, `bin/llvm-link`, `lib/libLLVM-16.so.1`, `lib/libclang-cpp.so.16` | Apache-2.0 WITH LLVM-exception | `clang-16.copyright`, `llvm-16.copyright`, `libllvm16.copyright`, `libclang-cpp16.copyright`, `LLVM-16.LICENSE.TXT` |
| Clang resource headers | Ubuntu 22.04 `libclang-common-16-dev` | `lib/clang/16/`, `include/` | Apache-2.0 WITH LLVM-exception | `libclang-common-16-dev.copyright`, `LLVM-16.LICENSE.TXT` |
| Shared libraries | Ubuntu 22.04 packages | every other `lib/*.so*` (Z3, Boost, libstdc++, ICU, libxml2, ...) | as stated in each package's copyright | `<package>.copyright`, one per package, and the texts they name in `common-licenses/` |

Sources of the texts:
- KLEE, AFL++, STP and MiniSat: the license files of the tagged versions above,
  and the notices copied verbatim from their source files;
- the LLVM license: `llvm/LICENSE.TXT` of the `release/16.x` branch;
- `Artistic-1.0-Perl.txt` and `LGPL-2.0.txt`: the SPDX license list (v3.25.0),
  the texts STP's `LICENSE_COMPONENTS` refers to for Bit::Vector;
- the Debian copyright files and `common-licenses/`: copied from the build
  image by `make-archive.sh`.
