cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Git REQUIRED)

set(Z3_FOLDER ${CMAKE_INSTALL_PREFIX}/Z3)
ExternalProject_Add( z3Solver
  PREFIX dependencies/Z3
  GIT_REPOSITORY https://github.com/Z3Prover/z3.git
  GIT_TAG z3-4.4.1
  CONFIGURE_COMMAND CXX=g++ CC=gcc python scripts/mk_make.py --prefix=${Z3_FOLDER}
  BUILD_COMMAND cd build && make -j4
  INSTALL_COMMAND cd build && make install
  BUILD_IN_SOURCE 1
#     -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
)
