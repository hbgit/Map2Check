# FindKleeUCLibC.cmake — Locate klee-uclibc (pre-built in container)
#
# In the Dockerfile.dev environment, klee-uclibc is built at /opt/klee-uclibc:
#   git clone https://github.com/klee/klee-uclibc.git /opt/klee-uclibc
#   ./configure --make-llvm-lib --with-cc clang-16 --with-llvm-config llvm-config-16
#   make
#
# Sets:
#   KLEE_UCLIB_FOLDER  — klee-uclibc source/build directory

# Check KLEE_UCLIBC_PATH env var first, then default paths
if(DEFINED ENV{KLEE_UCLIBC_PATH})
  set(KLEE_UCLIB_FOLDER "$ENV{KLEE_UCLIBC_PATH}")
elseif(EXISTS "/opt/klee-uclibc")
  set(KLEE_UCLIB_FOLDER "/opt/klee-uclibc")
else()
  message(FATAL_ERROR "klee-uclibc not found! Set KLEE_UCLIBC_PATH or install to /opt/klee-uclibc")
endif()

if(EXISTS "${KLEE_UCLIB_FOLDER}/lib/libc.a")
  message(STATUS "Found klee-uclibc: ${KLEE_UCLIB_FOLDER}")
else()
  message(WARNING "klee-uclibc directory found at ${KLEE_UCLIB_FOLDER} but lib/libc.a is missing. Build may fail.")
endif()
