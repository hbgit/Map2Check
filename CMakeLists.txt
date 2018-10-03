cmake_minimum_required(VERSION 3.5)
project(Map2Check)

set(Map2Check_VERSION_MAJOR 7)
set(Map2Check_VERSION_MINOR 0)

option(USE_PREBUILT_CLANG "Download and Install pre-built clang" ON)
option(BUILD_DOC "Build documentation" OFF)
option(SKIP_LIB_FUZZER "Don't build libFuzzer" OFF)
option(SKIP_KLEE "Don't build KLEE" OFF)
option(REGRESSION "Prepare Regression Tests" OFF)

set (CMAKE_CXX_STANDARD 11)


# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# set(CMAKE_EXE_LINKER_FLAGS "-Bstatic -static-libgcc -static-libstdc++")
# include(cmake/ExternalDeps.cmake)  

include(cmake/FindClang.cmake)
include(cmake/FindBoost.cmake)

if(NOT SKIP_LIB_FUZZER)
  # include(cmake/FindLibFuzzer.cmake)
endif()

if(NOT SKIP_KLEE)
  # include(cmake/FindZ3.cmake)
  include(cmake/FindMiniSat.cmake)
  include(cmake/FindSTP.cmake)
  include(cmake/FindKleeUCLibC.cmake)
  include(cmake/FindKlee.cmake)
endif()

if(REGRESSION)
  include(cmake/DownloadRegression.cmake)
endif()

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(modules)

if(BUILD_DOC)
  include(cmake/GenerateDocs.cmake)
endif()
