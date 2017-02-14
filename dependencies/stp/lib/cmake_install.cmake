# Install script for directory: /home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/map2check/devel_tool/map_src_on_docker/release")
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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/libstp.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/stp" TYPE FILE FILES "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/include/stp/c_interface.h")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Globals/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/AST/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/AbsRefineCounterExample/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Simplifier/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Printer/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Interface/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/extlib-abc/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/extlib-constbv/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/STPManager/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/ToSat/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Sat/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Util/cmake_install.cmake")

endif()

