cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Subversion REQUIRED)

ExternalProject_Add( libFuzzer
  PREFIX dependencies/Fuzzer
  SVN_REPOSITORY http://llvm.org/svn/llvm-project/compiler-rt/trunk/lib/fuzzer

  CONFIGURE_COMMAND ""
  BUILD_COMMAND CXX=${CLANG_CXX} <SOURCE_DIR>/build.sh
  BUILD_BYPRODUCTS libFuzzer.a 
  INSTALL_COMMAND ""
)

install(FILES ${PROJECT_BINARY_DIR}/dependencies/Fuzzer/src/libFuzzer-build/libFuzzer.a
    DESTINATION lib
    RENAME libFuzzer.a)


