project(Map2Check)

cmake_minimum_required(VERSION 3.5)
message("Started Map2Check CMake")

set(LLVM_VERSION 3.8.1)  
# set(CMAKE_CXX_COMPILER g++)

set(INC -I${PROJECT_SOURCE_DIR}/dependencies/llvm-${LLVM_VERSION}/include)

find_program(LLVM_CONFIG llvm-config
  # dependencies/llvm-build-cmake
  HINTS ${PROJECT_SOURCE_DIR}/dependencies/llvm-build-cmake/bin
  # PATHS ${libclang_llvm_lib_search_paths}
  # PATH_SUFFIXES LLVM/lib #Windows package from http://llvm.org/releases/
  DOC "llvm-config is used to generate flags to link with llvm.")

if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "LLVM not found!")
endif()

set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE LLVM_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)

set (EXECUTE_LLVM_LDFLAGS ${LLVM_CONFIG} --ldflags)
execute_process(COMMAND ${EXECUTE_LLVM_LDFLAGS} OUTPUT_VARIABLE LLVM_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set (EXECUTE_LLVM_LIBSFLAGS ${LLVM_CONFIG} --libs)
execute_process(COMMAND ${EXECUTE_LLVM_LDFLAGS} OUTPUT_VARIABLE LLVM_LIBSFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set (EXECUTE_LLVM_SYSTEMFLAGS ${LLVM_CONFIG} --system-libs)
execute_process(COMMAND ${EXECUTE_LLVM_LDFLAGS} OUTPUT_VARIABLE LLVM_SYSTEMFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(LLVM_LIBS "${LLVM_LDFLAGS} ${LLVM_LIBSFLAGS} ${LLVM_SYSTEMFLAGS}")

add_subdirectory(modules)

