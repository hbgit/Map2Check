project(Map2Check)

cmake_minimum_required(VERSION 3.5)

option(COPY_EXTERNAL "COPY_EXTERNAL" OFF)

include(cmake/FindClang.cmake)

# This should only be executed from docker container
if(COPY_EXTERNAL)
  include(cmake/ExternalDeps.cmake)
endif()

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(modules)
