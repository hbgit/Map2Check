cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add( STP
  PREFIX dependencies/STP
  DEPENDS MiniSat
  GIT_REPOSITORY https://github.com/stp/stp.git  
  GIT_TAG 2.1.2
  CMAKE_ARGS
      -DBUILD_SHARED_LIBS=OFF
      -DENABLE_PYTHON_INTERFACE=OFF
      -DMINISAT_LIBRARY=${MINI_SAT_FOLDER}/MiniSat-build/libminisat.a
      -DMINISAT_INCLUDE_DIR=${MINI_SAT_FOLDER}/MiniSat/
  INSTALL_COMMAND ""  
)
