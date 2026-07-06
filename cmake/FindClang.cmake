# FindClang.cmake — Locate LLVM/Clang 16 toolchain
#
# Expects LLVM_DIR to be set (e.g., /usr/lib/llvm-16/lib/cmake/llvm)
# or LLVM to be findable via find_package(LLVM REQUIRED CONFIG).
#
# Sets:
#   CLANG_CC, CLANG_CXX    — paths to clang, clang++
#   LLVM_CONFIG_BIN         — path to llvm-config
#   CXX_FLAGS, CPP_FLAGS    — compiler flags from llvm-config

function(install_link_file LINK DEST NAME)
  execute_process(COMMAND readlink -f ${LINK}
    OUTPUT_VARIABLE EXTERNAL_LIB
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  install(FILES ${EXTERNAL_LIB}
    DESTINATION ${DEST}
    RENAME ${NAME})
endfunction()

function(install_exec_file LINK DEST NAME)
  execute_process(COMMAND readlink -f ${LINK}
    OUTPUT_VARIABLE EXTERNAL_LIB
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  install(FILES ${EXTERNAL_LIB}
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${DEST}
    RENAME ${NAME})
endfunction()

# --- Find LLVM 16 ---
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} in ${LLVM_DIR}")

set(SEARCH_PATH ${LLVM_TOOLS_BINARY_DIR})

find_program(CLANG_CC NAMES clang PATHS ${SEARCH_PATH} NO_DEFAULT_PATH)
find_program(CLANG_CXX NAMES clang++ PATHS ${SEARCH_PATH} NO_DEFAULT_PATH)
find_program(LLVM_CONFIG_BIN NAMES llvm-config PATHS ${SEARCH_PATH} NO_DEFAULT_PATH)

if(NOT CLANG_CC)
  message(FATAL_ERROR "clang not found in ${SEARCH_PATH}")
endif()

if(NOT LLVM_CONFIG_BIN)
  message(FATAL_ERROR "llvm-config not found in ${SEARCH_PATH}")
endif()

# Set CLANG as the default C/CXX compiler
set(CMAKE_C_COMPILER ${CLANG_CC} CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER ${CLANG_CXX} CACHE FILEPATH "CXX compiler" FORCE)

# Confirm clang version
execute_process(COMMAND ${CMAKE_C_COMPILER} --version
  OUTPUT_VARIABLE clang_full_version_string)
message(STATUS "Using Clang: ${clang_full_version_string}")

# Get LLVM compiler flags
execute_process(COMMAND ${LLVM_CONFIG_BIN} --cxxflags
  OUTPUT_VARIABLE CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_BIN} --cppflags --link-static
  OUTPUT_VARIABLE CPP_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

# --- Install LLVM tools ---
list(APPEND MAP2CHECK_EXTERNAL_CLANG_BIN "clang" "opt" "llvm-link")

foreach(B ${MAP2CHECK_EXTERNAL_CLANG_BIN})
  set(BIN ${LLVM_TOOLS_BINARY_DIR}/${B})
  install_exec_file(${BIN} bin ${B})
endforeach()
