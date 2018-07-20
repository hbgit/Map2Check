project(Map2Check)

cmake_minimum_required(VERSION 3.5)

option(CopyUbuntuShared "CopyUbuntuShared" OFF)

# TODO(rafa.sa.xp@gmail.com): Create option to define clang path
include(FindClang.cmake)

#if(CopyUbuntuShared)
include(ExternalDeps.cmake)
#endif()

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(modules)
