project(Map2Check)

cmake_minimum_required(VERSION 3.5)

option(COPY_EXTERNAL "COPY_EXTERNAL" OFF)

# This should only be executed from docker container
if(COPY_EXTERNAL)
  #If this option is setted, external deps MUST be called before find clang
  include(cmake/ExternalDeps.cmake)
  
endif()

include(cmake/FindClang.cmake)

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(modules)
