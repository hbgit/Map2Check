cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Git REQUIRED)

set(Z3_FOLDER ${PROJECT_BINARY_DIR}/dependencies/Z3/src/)
ExternalProject_Add( z3Solver
  PREFIX dependencies/Z3
  GIT_REPOSITORY https://github.com/Z3Prover/z3.git
  CMAKE_ARGS  -DCMAKE_BUILD_TYPE=Release
  INSTALL_COMMAND ""  
)



