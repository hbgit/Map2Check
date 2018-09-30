cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Git REQUIRED)

set(MINI_SAT_FOLDER ${PROJECT_BINARY_DIR}/dependencies/MiniSat/src)
ExternalProject_Add( MiniSat
  PREFIX dependencies/MiniSat
  GIT_REPOSITORY https://github.com/stp/minisat.git
  CMAKE_ARGS
  -DSTATIC_BINARIES=ON
  INSTALL_COMMAND ""  
)
