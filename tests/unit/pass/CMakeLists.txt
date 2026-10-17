# Tests for the LLVM passes against the core API of map2check-library.
#
# RuntimeABITest checks the central hook table (modules/backend/pass/RuntimeABI)
# against the ABI mapping. PassIRTest compiles tiny C inputs with clang-16, runs
# one pass plugin through opt-16 and inspects the resulting module.

find_program(M2C_TEST_CLANG NAMES clang-16 clang
  HINTS "${LLVM_TOOLS_BINARY_DIR}" NO_DEFAULT_PATH)
find_program(M2C_TEST_OPT NAMES opt-16 opt
  HINTS "${LLVM_TOOLS_BINARY_DIR}" NO_DEFAULT_PATH)
if(NOT M2C_TEST_CLANG OR NOT M2C_TEST_OPT)
  message(FATAL_ERROR "clang/opt not found in ${LLVM_TOOLS_BINARY_DIR}")
endif()

# The test executables only read and inspect bitcode. Link the LLVM shared
# library when the installation provides it (Debian/apt.llvm.org do): the
# static components pull in zstd::libzstd_shared, which needs its own
# find_package().
if(TARGET LLVM)
  set(M2C_TEST_LLVM_LIBS LLVM)
else()
  find_package(zstd QUIET)
  llvm_map_components_to_libnames(M2C_TEST_LLVM_LIBS core irreader bitreader support)
endif()

include(GoogleTest)

set(M2C_PASS_SRC "${PROJECT_SOURCE_DIR}/modules/backend/pass")

function(m2c_pass_test NAME)
  add_executable(${NAME} ${ARGN} "${M2C_PASS_SRC}/RuntimeABI.cpp")
  target_include_directories(${NAME} PRIVATE ${LLVM_INCLUDE_DIRS})
  separate_arguments(_m2c_llvm_defs NATIVE_COMMAND "${LLVM_DEFINITIONS}")
  target_compile_options(${NAME} PRIVATE ${_m2c_llvm_defs})
  target_compile_definitions(${NAME} PRIVATE
    MAP2CHECK_ABI_MAPPING_TSV="${CMAKE_CURRENT_SOURCE_DIR}/data/abi-mapping.tsv"
    M2C_TEST_INPUTS="${CMAKE_CURRENT_SOURCE_DIR}/inputs"
    M2C_TEST_WORKDIR="${CMAKE_CURRENT_BINARY_DIR}/work-${NAME}"
    M2C_TEST_CLANG="${M2C_TEST_CLANG}"
    M2C_TEST_OPT="${M2C_TEST_OPT}"
    M2C_PASS_PLUGIN_DIR="$<TARGET_FILE_DIR:Map2CheckLibrary>"
    M2C_PASS_PLUGIN_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}")
  target_link_libraries(${NAME} ${M2C_TEST_LLVM_LIBS} gtest_main)
  # One ctest entry per test case (RuntimeABI.*, PassIR.*), so a failing hook
  # is visible by name in the ctest summary.
  gtest_discover_tests(${NAME} DISCOVERY_MODE PRE_TEST)
endfunction()

m2c_pass_test(RuntimeABITest RuntimeABITest.cpp)

# PassIRTest loads the pass plugins into the system opt. With sanitizers on,
# the plugins are ASan/UBSan-instrumented and opt is not, so they cannot be
# loaded ("Failed to load passes"); the IR checks then run in the regular
# build only. RuntimeABITest still runs under the sanitizers.
if(MAP2CHECK_ENABLE_SANITIZERS)
  message(STATUS "PassIRTest skipped: sanitized pass plugins cannot load into opt")
else()
  m2c_pass_test(PassIRTest PassIRTest.cpp)
  # The plugins are loaded by opt at test time; build them before the test.
  add_dependencies(PassIRTest
    TrackBasicBlockPass AssertPass TargetPass LoopPredAssumePass Map2CheckLibrary
    OverflowPass NonDetPass MemoryTrackPass)

  # Steps 5 and 5b of Task 3.2: every input, instrumented with all passes of
  # each analysis mode, links against the Rust runtime; an assume rejection
  # unwinds at -O2 under the libFuzzer driver.
  # MAP2CHECK_RUST_RUNTIME_LIB: resolved by cmake/Map2CheckRustRuntime.cmake.
  if(MAP2CHECK_RUST_RUNTIME_LIB)
    m2c_pass_test(EndToEndLinkTest EndToEndLinkTest.cpp)
    target_compile_definitions(EndToEndLinkTest PRIVATE
      M2C_RUST_LIB="${MAP2CHECK_RUST_RUNTIME_LIB}")
    add_dependencies(EndToEndLinkTest
      NonDetPass MemoryTrackPass OverflowPass AssertPass TargetPass Map2CheckLibrary)
    if(TARGET Map2CheckRustRuntime)
      add_dependencies(EndToEndLinkTest Map2CheckRustRuntime)
    endif()
  else()
    message(STATUS "EndToEndLinkTest skipped: no Rust runtime configured")
  endif()
endif()
