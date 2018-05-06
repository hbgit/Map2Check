project(Map2Check)

cmake_minimum_required(VERSION 3.5)

# FIND LLVM 6.0
set(LLVM_BIN ${PROJECT_SOURCE_DIR}/dependencies/clang)
find_program(CLANG_CC clang HINTS ${LLVM_BIN}/bin)
find_program(CLANG_CXX clang++ HINTS ${LLVM_BIN}/bin)
find_program(LLVM_CONFIG llvm-config HINTS ${LLVM_BIN}/bin DOC "llvm-config is used to generate flags to link with llvm.")

# Check if CLANG is present and configure LLVM
if(NOT CLANG_CC)
    message(FATAL_ERROR "CLANG not found! (Did you execute de boostrap script?)")
endif()

# Set CLANG as the default C/CXX compiler
set(CMAKE_C_COMPILER ${CLANG_CC})
set(CMAKE_CXX_COMPILER ${CLANG_CXX})

# Configure boost (Should fix this)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
find_package(Boost REQUIRED COMPONENTS program_options system)
include_directories(${Boost_INCLUDE_DIRS})

###########################################################################

add_subdirectory(modules)
#enable_testing()
#add_subdirectory(test)