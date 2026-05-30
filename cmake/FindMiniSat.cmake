# FindMiniSat.cmake — Locate MiniSat solver (pre-installed in container)
#
# In the Dockerfile.dev environment, MiniSat is built from source:
#   git clone https://github.com/stp/minisat.git
#   cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local && make install
#
# Sets:
#   MINI_SAT_FOLDER  — MiniSat installation prefix

find_library(MINISAT_LIBRARY NAMES minisat
  PATHS /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu)
find_path(MINISAT_INCLUDE_DIR NAMES minisat/core/Solver.h
  PATHS /usr/local/include /usr/include)

if(MINISAT_LIBRARY AND MINISAT_INCLUDE_DIR)
  message(STATUS "Found MiniSat: ${MINISAT_LIBRARY}")
  get_filename_component(MINI_SAT_FOLDER "${MINISAT_LIBRARY}" DIRECTORY)
  get_filename_component(MINI_SAT_FOLDER "${MINI_SAT_FOLDER}/.." ABSOLUTE)
else()
  message(FATAL_ERROR "MiniSat not found! Build from source: https://github.com/stp/minisat.git")
endif()
