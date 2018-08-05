cmake_minimum_required(VERSION 3.5)
project(Map2Check)

set(Map2Check_VERSION_MAJOR 7)
set(Map2Check_VERSION_MINOR 0)

option(COPY_EXTERNAL "COPY_EXTERNAL" OFF)

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

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )

else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

