# Install script for directory: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/IR/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/IRReader/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/CodeGen/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Bitcode/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Transforms/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Linker/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Analysis/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/LTO/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/MC/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Object/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Option/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/DebugInfo/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Target/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/AsmParser/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/LineEditor/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ProfileData/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Fuzzer/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/Passes/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/LibDriver/cmake_install.cmake")

endif()

