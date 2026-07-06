# FindZ3.cmake — Locate Z3 solver (installed via apt or from source)
#
# In the Dockerfile.dev environment, Z3 is installed via:
#   apt-get install -y libz3-dev z3  (version 4.8.12)
#
# Sets:
#   Z3_INCLUDE_DIRS  — Z3 header directory
#   Z3_LIBRARIES     — Z3 library path

find_package(Z3 CONFIG QUIET)

if(Z3_FOUND)
  message(STATUS "Found Z3 ${Z3_VERSION} via CMake config")
else()
  # Fallback: find Z3 manually (apt install puts it in standard paths)
  find_path(Z3_INCLUDE_DIRS NAMES z3.h z3++.h
    PATHS /usr/include /usr/local/include)
  find_library(Z3_LIBRARIES NAMES z3 libz3
    PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)

  if(Z3_INCLUDE_DIRS AND Z3_LIBRARIES)
    message(STATUS "Found Z3: ${Z3_LIBRARIES} (headers: ${Z3_INCLUDE_DIRS})")
  else()
    message(FATAL_ERROR "Z3 not found! Install via: apt-get install libz3-dev z3")
  endif()
endif()

# Export for downstream use (e.g., KLEE cmake args)
set(Z3_FOLDER "${Z3_INCLUDE_DIRS}/.." CACHE PATH "Z3 installation prefix")
