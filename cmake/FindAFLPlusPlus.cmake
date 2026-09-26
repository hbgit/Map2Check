# FindAFLPlusPlus.cmake — Locate the AFL++ fuzzers (4.40c, LLVM 16)
#
# AFL++ is a standalone toolchain invoked at run time by caller.cpp through
# system(): afl-clang-fast compiles the fuzzer binary (PCGUARD) and afl-fuzz
# drives it. It is installed into the image by Dockerfile.dev section 7 and
# resolved at run time by Map2Check::aflClangFastBinary() /
# Map2Check::aflFuzzBinary() (tools.hpp), which honour an env override and fall
# back to /usr/local/bin.
#
# This module only records availability so the build can say so — the role
# the previous fuzzer find-module played before the AFL++ migration.
#
# Sets:
#   AFL_PLUS_PLUS_FOUND  — TRUE if both binaries are present

find_program(AFL_CLANG_FAST afl-clang-fast PATHS /usr/local/bin /opt/afl++/bin)
find_program(AFL_FUZZ afl-fuzz PATHS /usr/local/bin /opt/afl++/bin)

if(AFL_CLANG_FAST AND AFL_FUZZ)
  set(AFL_PLUS_PLUS_FOUND TRUE)
  message(STATUS "Found AFL++: ${AFL_CLANG_FAST} / ${AFL_FUZZ}")
else()
  set(AFL_PLUS_PLUS_FOUND FALSE)
  message(WARNING "AFL++ not found (afl-clang-fast/afl-fuzz). "
    "Fuzzing will be unavailable; build the dev image (Dockerfile.dev section 7) "
    "or set AFL_CC/AFL_FUZZ.")
endif()
