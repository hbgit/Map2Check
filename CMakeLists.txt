project(Map2Check)

cmake_minimum_required(VERSION 3.5)

# FIND LLVM 6.0
set(LLVM_BIN ${PROJECT_SOURCE_DIR}/dependencies/clang)
set(LLVM_SOURCE ${PROJECT_SOURCE_DIR}/dependencies/clang)
# list(APPEND CMAKE_MODULE_PATH "${LLVM_SOURCE}/lib/cmake/llvm")
# list(APPEND  CMAKE_PREFIX_PATH "${LLVM_SOURCE}/lib/cmake/llvm")
# find_package(LLVM REQUIRED CONFIG)

find_program(CLANG_CC clang HINTS ${LLVM_BIN}/bin)
find_program(CLANG_CXX clang++ HINTS ${LLVM_BIN}/bin)
find_program(LLVM_CONFIG llvm-config HINTS ${LLVM_BIN}/bin DOC "llvm-config is used to generate flags to link with llvm.")

# Check if CLANG is present and configure LLVM
if(NOT CLANG_CC)
    message(FATAL_ERROR "CLANG not found! (Did you execute de bootstrap script?)")
endif()

if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "LLVM-CONFIG not found! (Did you execute de bootstrap script?)")
endif()

# Set CLANG as the default C/CXX compiler
set(CMAKE_C_COMPILER ${CLANG_CC})
set(CMAKE_CXX_COMPILER ${CLANG_CXX})

# Configure boost (Should fix this)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
set(BOOST_USE_STATIC_LIBS true)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS program_options system)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})

###########################################################################

add_subdirectory(modules)
enable_testing()
# add_subdirectory(test)
