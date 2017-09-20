# Install script for directory: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang

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

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES
    "/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang"
    "/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang-c"
    FILES_MATCHING REGEX "/[^/]*\\.def$" REGEX "/[^/]*\\.h$" REGEX "/config\\.h$" EXCLUDE REGEX "/\\.svn$" EXCLUDE)
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang" FILES_MATCHING REGEX "/CMakeFiles$" EXCLUDE REGEX "/[^/]*\\.inc$" REGEX "/[^/]*\\.h$")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "libclang-headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang-c" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/\\.svn$" EXCLUDE)
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/clang/cmake/ClangTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/clang/cmake/ClangTargets.cmake"
         "/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/CMakeFiles/Export/share/clang/cmake/ClangTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/clang/cmake/ClangTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/clang/cmake/ClangTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/clang/cmake" TYPE FILE FILES "/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/CMakeFiles/Export/share/clang/cmake/ClangTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/clang/cmake" TYPE FILE FILES "/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/CMakeFiles/Export/share/clang/cmake/ClangTargets-release.cmake")
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/clang/cmake" TYPE FILE FILES "/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/cmake/modules/ClangConfig.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/utils/TableGen/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/lib/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/runtime/cmake_install.cmake")
  include("/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/docs/cmake_install.cmake")

endif()

