cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add( CrabLlvm
  PREFIX dependencies/crab-llvm
  GIT_REPOSITORY https://github.com/seahorn/crab-llvm.git
  CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=run 
      -DCMAKE_BUILD_TYPE=Release
      -DUSE_LDD=ON 
      -DUSE_APRON=ON
  INSTALL_COMMAND ""  
)



