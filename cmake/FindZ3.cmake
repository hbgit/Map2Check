cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add( z3Solver
  PREFIX dependencies/Z3
  GIT_REPOSITORY https://github.com/Z3Prover/z3.git
  CMAKE_ARGS  -DCMAKE_BUILD_TYPE=Release
  INSTALL_COMMAND ""  
)



