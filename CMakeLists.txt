cmake_minimum_required(VERSION 3.20)
project(Map2Check VERSION 8.0.0 LANGUAGES C CXX)

option(BUILD_DOC "Build documentation" OFF)
option(SKIP_LIB_FUZZER "Don't use libFuzzer" OFF)
option(SKIP_KLEE "Don't use KLEE" OFF)
option(REGRESSION "Prepare Regression Tests" OFF)
option(ENABLE_TEST "Build all tests" OFF)
option(MAP2CHECK_ENABLE_SANITIZERS "Enable AddressSanitizer + UndefinedBehaviorSanitizer for testing" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Sanitizer configuration (Phase 1.5 — OpenSSF Best Practices) ---
if(MAP2CHECK_ENABLE_SANITIZERS)
  message(STATUS "Sanitizers enabled: ASan + UBSan (dynamic linking)")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
  add_link_options(-fsanitize=address,undefined)
  # ASan requires dynamic linking — skip static config
  set(Map2Check_MODE "SHARED")
else()
  # --- Static linking configuration ---
  set(Map2Check_MODE "STATIC")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  set(CMAKE_EXE_LINKER_FLAGS "-Bstatic -static-libgcc -static-libstdc++")
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
  enable_testing()
  include(cmake/FindGTest.cmake)
  message(STATUS "Adding tests")
  add_subdirectory(tests)
endif()

add_subdirectory(modules)

if(BUILD_DOC)
  include(cmake/GenerateDocs.cmake)
endif()
