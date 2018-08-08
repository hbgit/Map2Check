cmake_minimum_required(VERSION 3.5)
project(Map2Check)

set(Map2Check_VERSION_MAJOR 7)
set(Map2Check_VERSION_MINOR 0)

option(COPY_EXTERNAL "COPY_EXTERNAL" OFF)
option(BUILD_DOC "Build documentation" OFF)

set (CMAKE_CXX_STANDARD 11)

# This should only be executed from docker container
if(COPY_EXTERNAL)
  #If this option is setted, external deps MUST be called before find clang and findqt
  set(Map2Check_MODE "STATIC")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  set(CMAKE_EXE_LINKER_FLAGS "-Bstatic -static-libgcc -static-libstdc++")
  include(cmake/ExternalDeps.cmake)  
else()
    set(Map2Check_MODE "DYNAMIC")
endif()

include(cmake/FindClang.cmake)
include(cmake/FindBoost.cmake)

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(modules)

if(BUILD_DOC)
  include(cmake/GenerateDocs.cmake)
endif()

