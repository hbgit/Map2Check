project(Map2Check)

cmake_minimum_required(VERSION 3.5)

# FIND LLVM 6.0
set(LLVM_BIN ${PROJECT_SOURCE_DIR}/dependencies/clang)
set(LLVM_SOURCE ${PROJECT_SOURCE_DIR}/dependencies/clang)

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
message("C COMPILER=" ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${CLANG_CXX})
message("CXX COMPILER=" ${CMAKE_CXX_COMPILER})

# Get Flags
set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

# Configure boost (Should fix this)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
set(BOOST_USE_STATIC_LIBS true)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
#find_package(Boost 1.4.0 COMPONENTS filesystem REQUIRED)
find_package(Boost COMPONENTS program_options system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})

###########################################################################

add_subdirectory(modules)
enable_testing()
# add_subdirectory(test)
