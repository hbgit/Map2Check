cmake_minimum_required(VERSION 3.20)
project(Map2Check VERSION 8.0.0 LANGUAGES C CXX)

option(BUILD_DOC "Build documentation" OFF)
option(SKIP_LIB_FUZZER "Don't use libFuzzer" OFF)
option(SKIP_KLEE "Don't use KLEE" OFF)
option(REGRESSION "Prepare Regression Tests" OFF)
option(ENABLE_TEST "Build all tests" OFF)
option(MAP2CHECK_ENABLE_SANITIZERS "Enable AddressSanitizer + UndefinedBehaviorSanitizer for testing" OFF)
option(MAP2CHECK_DYNAMIC_LINK "Link dynamically (required when KLEE/Z3 are only available as shared libraries)" OFF)
option(ENABLE_CLAM "Enable --add-invariants via Clam (requires Clam dev16 installed, see docs/reports/2026-08-16-crabllvm-review.md)" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/release"
      CACHE PATH "Install prefix (default: <repo>/release)" FORCE)
endif()

# --- Sanitizer configuration (Phase 1.5 — OpenSSF Best Practices) ---
if(MAP2CHECK_ENABLE_SANITIZERS)
  message(STATUS "Sanitizers enabled: ASan + UBSan (dynamic linking)")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
  add_link_options(-fsanitize=address,undefined)
  # Ensure all targets (including OBJECT libraries) are compiled with -fPIC,
  # required for LLVM pass shared libraries (.so)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  # ASan requires dynamic linking — skip static config
  set(Map2Check_MODE "SHARED")
else()
  # --- Static linking configuration ---
  set(Map2Check_MODE "STATIC")
  # Prefer static archives, but fall back to shared objects: Ubuntu's
  # libz3-dev package only ships libz3.so (no libz3.a), so a ".a"-only
  # suffix list makes find_library() fail to locate Z3 even when installed.
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so")
  set(CMAKE_EXE_LINKER_FLAGS "-Bstatic -static-libgcc -static-libstdc++")
endif()

# --- Abstract-interpretation invariants (Clam, formerly crab-llvm) ---
# Off by default, and deliberately so: an unsound invariant does not raise an
# error, it produces a wrong TRUE. Under KLEE klee_assume() prunes a reachable
# state; under LibFuzzer nondet_assume() calls pthread_exit() and the execution
# disappears. Promoting this to a default needs the differential evidence
# described in docs/reports/2026-08-16-crabllvm-review.md.
#
# Clam is a subprocess tool, not a linked library, so this only records intent.
# The binary still checks at run time that the driver is actually installed --
# neither condition implies the other.
if(ENABLE_CLAM)
  add_compile_definitions(MAP2CHECK_ENABLE_CLAM)
  message(STATUS "Clam invariants: ENABLED (--add-invariants available)")
else()
  message(STATUS "Clam invariants: disabled (-DENABLE_CLAM=ON to enable)")
endif()

# --- Core dependencies ---
include(cmake/FindClang.cmake)
include(cmake/FindBoost.cmake)

if(NOT SKIP_LIB_FUZZER)
  include(cmake/FindLibFuzzer.cmake)
endif()

if(NOT SKIP_KLEE)
  include(cmake/FindZ3.cmake)
  include(cmake/FindMiniSat.cmake)
  include(cmake/FindSTP.cmake)
  include(cmake/FindKleeUCLibC.cmake)
  include(cmake/FindKlee.cmake)
endif()

if(REGRESSION)
  include(cmake/DownloadRegression.cmake)
endif()

include_directories(${PROJECT_SOURCE_DIR})

if(ENABLE_TEST)
  # include(CTest) em vez de enable_testing(): gera DartConfiguration.tcl,
  # necessário para `ctest -T memcheck` (job Valgrind no CI).
  include(CTest)
  include(cmake/FindGTest.cmake)
  message(STATUS "Adding tests")
  add_subdirectory(tests)
endif()

add_subdirectory(modules)

if(BUILD_DOC)
  include(cmake/GenerateDocs.cmake)
endif()
