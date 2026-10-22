# Fails unless M2C_LIB is the Map2Check runtime build of map2check-library
# (cargo build --profile runtime --features libfuzzer,wasm): the archive must
# define LLVMFuzzerTestOneInput (feature libfuzzer) and map2check_wasm_malloc
# (feature wasm). The featureless core build defines neither, the libfuzzer
# build only the first.
#
# Used two ways, with the same inputs:
#   include() at configure time, with M2C_LIB and M2C_NMS set;
#   cmake -DM2C_LIB=<archive> -DM2C_NMS=<nm;nm...> -DM2C_HINT=<text> -P <this file>
#   as a build step after cargo (MAP2CHECK_RUST_SRC route).
#
# M2C_NMS: nm programs to try. Any one that can read the archive will do. The
# members carry bitcode from rustc's LLVM, newer than 16, which llvm-nm-16 (the
# CMAKE_NM of a clang-16 toolchain) rejects, so several are tried.
# M2C_HINT: how to get the right archive, appended to the error.
if(NOT EXISTS "${M2C_LIB}")
  message(FATAL_ERROR "${M2C_LIB} does not exist. ${M2C_HINT}")
endif()
set(_m2c_syms LLVMFuzzerTestOneInput map2check_wasm_malloc)
set(_m2c_missing "")
set(_m2c_readers "")
foreach(_m2c_nm IN LISTS M2C_NMS)
  if(NOT _m2c_nm)
    continue()
  endif()
  execute_process(COMMAND "${_m2c_nm}" -g --defined-only "${M2C_LIB}"
    RESULT_VARIABLE _m2c_nm_rc OUTPUT_VARIABLE _m2c_nm_out ERROR_QUIET)
  # An nm that fails or prints nothing could not read the archive; its
  # silence says nothing about the symbols.
  if(NOT _m2c_nm_rc EQUAL 0 OR _m2c_nm_out STREQUAL "")
    continue()
  endif()
  list(APPEND _m2c_readers "${_m2c_nm}")
  set(_m2c_missing_here "")
  foreach(_m2c_sym IN LISTS _m2c_syms)
    if(NOT _m2c_nm_out MATCHES " T ${_m2c_sym}(\n|$)")
      list(APPEND _m2c_missing_here ${_m2c_sym})
    endif()
  endforeach()
  if(NOT _m2c_missing_here)
    return()  # accepted
  endif()
  set(_m2c_missing ${_m2c_missing_here})
endforeach()

if(NOT _m2c_readers)
  list(JOIN M2C_NMS ", " _m2c_tried)
  message(FATAL_ERROR "cannot check that ${M2C_LIB} is the Map2Check runtime "
    "build: no nm could read the archive (tried: ${_m2c_tried}). Install a "
    "binutils nm or an llvm-nm at least as new as rustc's LLVM. ${M2C_HINT}")
endif()
list(JOIN _m2c_missing ", " _m2c_missing)
message(FATAL_ERROR "${M2C_LIB} is not the Map2Check runtime build: it does "
  "not define ${_m2c_missing} (the core build defines neither symbol, the "
  "libfuzzer build lacks map2check_wasm_malloc). ${M2C_HINT}")
