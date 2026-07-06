# FindLibFuzzer.cmake — Locate LibFuzzer from LLVM 16 compiler-rt
#
# In LLVM 16, LibFuzzer is part of compiler-rt and does NOT need to be
# built separately. It is available via:
#   clang-16 -fsanitize=fuzzer
#
# For Map2Check's linking approach (linking libFuzzer.a directly),
# we locate the static archive in the LLVM compiler-rt directory.
#
# Sets:
#   LIBFUZZER_ARCHIVE  — path to libclang_rt.fuzzer-x86_64.a
#   LIBFUZZER_FOUND    — TRUE if found

# Determine the compiler-rt lib directory
execute_process(COMMAND ${CLANG_CC} --print-runtime-dir
  OUTPUT_VARIABLE CLANG_RUNTIME_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)

if(NOT CLANG_RUNTIME_DIR)
  # Fallback: construct path manually
  set(CLANG_RUNTIME_DIR "/usr/lib/llvm-16/lib/clang/16/lib/linux")
endif()

# Look for the fuzzer archive
find_library(LIBFUZZER_ARCHIVE
  NAMES clang_rt.fuzzer-x86_64 clang_rt.fuzzer_no_main-x86_64
  PATHS ${CLANG_RUNTIME_DIR}
  NO_DEFAULT_PATH)

if(LIBFUZZER_ARCHIVE)
  set(LIBFUZZER_FOUND TRUE)
  message(STATUS "Found LibFuzzer: ${LIBFUZZER_ARCHIVE}")

  # map2check.cpp/caller.cpp invoke a *copy* of clang installed at
  # ${MAP2CHECK_PATH}/bin/clang (see FindClang.cmake's install_exec_file).
  # That copy resolves its resource-dir relative to its own location
  # (<prefix>/lib/clang/<ver>/lib/linux), not the system LLVM install, so
  # -fsanitize=fuzzer needs the compiler-rt archives mirrored there —
  # a single renamed libFuzzer.a is never actually looked up by clang.
  execute_process(COMMAND ${CLANG_CC} --print-resource-dir
    OUTPUT_VARIABLE CLANG_RESOURCE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
  get_filename_component(CLANG_RESOURCE_VERSION "${CLANG_RESOURCE_DIR}" NAME)

  install(DIRECTORY ${CLANG_RUNTIME_DIR}/
    DESTINATION lib/clang/${CLANG_RESOURCE_VERSION}/lib/linux
    FILES_MATCHING PATTERN "*.a")
else()
  set(LIBFUZZER_FOUND FALSE)
  message(WARNING "LibFuzzer archive not found in ${CLANG_RUNTIME_DIR}. "
    "Fuzzer functionality will use -fsanitize=fuzzer flag instead.")
endif()
