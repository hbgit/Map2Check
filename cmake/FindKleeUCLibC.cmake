cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Git REQUIRED)

set(KLEE_UCLIB_FOLDER ${PROJECT_BINARY_DIR}/dependencies/KleeUCLibC/src/KleeUCLibC)

ExternalProject_Add( KleeUCLibC
  PREFIX dependencies/KleeUCLibC
  GIT_TAG klee_0_9_29
  GIT_REPOSITORY https://github.com/klee/klee-uclibc.git
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --make-llvm-lib --with-llvm-config=${LLVM_CONFIG}
  BUILD_IN_SOURCE 1
  BUILD_COMMAND make -j2
  INSTALL_COMMAND ""  
)

