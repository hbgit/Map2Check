# FindKlee.cmake — Locate KLEE 3.1 (pre-installed in container)
#
# In the Dockerfile.dev environment, KLEE 3.1 is built from source and
# installed to /opt/klee:
#   git clone -b v3.1 https://github.com/klee/klee.git
#   cmake .. -DCMAKE_INSTALL_PREFIX=/opt/klee ...
#   make install
#
# Sets:
#   KLEE_DIR          — KLEE installation prefix
#   KLEE_BIN_DIR      — KLEE binary directory (contains klee, kleaver, etc.)
#   KLEE_INCLUDE_DIR  — KLEE header directory
#   KLEE_LIB_DIR      — KLEE library directory

# Check KLEE_DIR env var first, then default paths
if(DEFINED ENV{KLEE_DIR})
  set(KLEE_DIR "$ENV{KLEE_DIR}")
elseif(EXISTS "/opt/klee")
  set(KLEE_DIR "/opt/klee")
else()
  message(FATAL_ERROR "KLEE not found! Set KLEE_DIR or install to /opt/klee")
endif()

set(KLEE_BIN_DIR "${KLEE_DIR}/bin")
set(KLEE_INCLUDE_DIR "${KLEE_DIR}/include")
set(KLEE_LIB_DIR "${KLEE_DIR}/lib")

# Validate installation
find_program(KLEE_EXECUTABLE NAMES klee PATHS ${KLEE_BIN_DIR} NO_DEFAULT_PATH)

if(KLEE_EXECUTABLE)
  execute_process(COMMAND ${KLEE_EXECUTABLE} --version
    OUTPUT_VARIABLE KLEE_VERSION_STRING
    ERROR_VARIABLE KLEE_VERSION_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Found KLEE: ${KLEE_EXECUTABLE}")
  message(STATUS "KLEE version: ${KLEE_VERSION_STRING}")
else()
  message(WARNING "KLEE binary not found in ${KLEE_BIN_DIR}. Runtime execution will fail.")
endif()

# Install KLEE binaries for release packaging
if(KLEE_EXECUTABLE)
  list(APPEND MAP2CHECK_KLEE_BINS "klee" "kleaver" "ktest-tool")
  foreach(B ${MAP2CHECK_KLEE_BINS})
    set(BIN "${KLEE_BIN_DIR}/${B}")
    if(EXISTS "${BIN}")
      install(PROGRAMS ${BIN} DESTINATION bin)
    endif()
  endforeach()
endif()
