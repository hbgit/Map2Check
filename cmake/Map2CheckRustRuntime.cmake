# Map2Check runtime: map2check-library (Rust). Replaces modules/backend/library
# (the C bitcode runtime, no longer part of the build).
#
# Installs ${prefix}/lib/libmap2check.a and, when available,
# ${prefix}/lib/map2check-klee.bc (KLEE runtime bitcode, LLVM 16).
#
# The defaults come from the environment variables of the same name, which the
# map2check-dev image sets (Dockerfile.dev); -D on the command line wins.
# Being CACHE defaults, they are read only on the first configure of a build
# directory: after the environment changes, reconfigure in a fresh one.
set(MAP2CHECK_RUST_LIB_DIR "$ENV{MAP2CHECK_RUST_LIB_DIR}" CACHE PATH "Directory with a prebuilt libmap2check.a (empty = build from MAP2CHECK_RUST_SRC)")
set(MAP2CHECK_RUST_SRC "$ENV{MAP2CHECK_RUST_SRC}" CACHE PATH "map2check-library checkout used when MAP2CHECK_RUST_LIB_DIR is empty")
set(MAP2CHECK_RUST_KLEE_BC "$ENV{MAP2CHECK_RUST_KLEE_BC}" CACHE FILEPATH "Prebuilt KLEE runtime bitcode (empty = none, or built from MAP2CHECK_RUST_SRC when rustc 1.72.1 is available)")

# --- Static library ---
# Any nm that can read the archive will do (see Map2CheckRustRuntimeCheck.cmake).
find_program(M2C_NM nm)
find_program(M2C_LLVM_NM llvm-nm)
set(_m2c_nms ${CMAKE_NM} ${M2C_NM} ${M2C_LLVM_NM})
list(REMOVE_DUPLICATES _m2c_nms)
set(_m2c_check "${CMAKE_CURRENT_LIST_DIR}/Map2CheckRustRuntimeCheck.cmake")
set(_m2c_build_hint "Build it with: cargo build --profile runtime --features libfuzzer,wasm")

if(MAP2CHECK_RUST_LIB_DIR)
  # The release archive (lib/ of map2check-library-<ver>-<target>.tar.gz)
  # ships three builds; the one Map2Check links is libmap2check_runtime.a
  # (profile runtime, features libfuzzer,wasm). Its libmap2check.a is the
  # featureless core build. A directory without libmap2check_runtime.a must
  # be a local `cargo build --profile runtime --features libfuzzer,wasm`
  # output (target/runtime), whose libmap2check.a is the runtime build under
  # the plain name. That fallback is accepted only when the archive defines
  # the symbols of both features, so the core or libfuzzer-only build is
  # never picked by accident.
  if(EXISTS "${MAP2CHECK_RUST_LIB_DIR}/libmap2check_runtime.a")
    set(_m2c_rust_lib "${MAP2CHECK_RUST_LIB_DIR}/libmap2check_runtime.a")
  else()
    set(_m2c_rust_lib "${MAP2CHECK_RUST_LIB_DIR}/libmap2check.a")
    if(NOT EXISTS "${_m2c_rust_lib}")
      message(FATAL_ERROR "libmap2check.a not found in ${MAP2CHECK_RUST_LIB_DIR} "
        "(nor libmap2check_runtime.a; build it with: cargo build --profile runtime --features libfuzzer,wasm)")
    endif()
    set(M2C_LIB "${_m2c_rust_lib}")
    set(M2C_NMS "${_m2c_nms}")
    string(CONCAT M2C_HINT "Point MAP2CHECK_RUST_LIB_DIR at the lib/ of the "
      "map2check-library release archive (it has libmap2check_runtime.a), or at "
      "the target/runtime of: cargo build --profile runtime --features libfuzzer,wasm")
    include("${_m2c_check}")
  endif()
elseif(MAP2CHECK_RUST_SRC)
  find_program(M2C_CARGO cargo REQUIRED)
  set(_m2c_rust_lib "${MAP2CHECK_RUST_SRC}/target/runtime/libmap2check.a")
  # Always run: cargo is incremental, so this is cheap when nothing changed,
  # and an archive already in target/runtime (say, from a build without the
  # features) is never taken as up to date. The check after cargo fails the
  # build unless the archive is the runtime build.
  add_custom_target(Map2CheckRustRuntime ALL
    COMMAND ${M2C_CARGO} build --profile runtime --features libfuzzer,wasm
    COMMAND ${CMAKE_COMMAND} -DM2C_LIB=${_m2c_rust_lib} "-DM2C_NMS=${_m2c_nms}"
      "-DM2C_HINT=${_m2c_build_hint} in ${MAP2CHECK_RUST_SRC}" -P "${_m2c_check}"
    BYPRODUCTS "${_m2c_rust_lib}"
    WORKING_DIRECTORY "${MAP2CHECK_RUST_SRC}"
    COMMENT "Building the Rust runtime (libmap2check.a)"
    VERBATIM)
else()
  message(FATAL_ERROR "Set MAP2CHECK_RUST_LIB_DIR or MAP2CHECK_RUST_SRC to provide the Rust runtime")
endif()
# The path the tests link (tests/unit/pass); installed under the name the
# frontend expects (kRustRuntimeLibrary).
set(MAP2CHECK_RUST_RUNTIME_LIB "${_m2c_rust_lib}")
install(FILES "${_m2c_rust_lib}" DESTINATION lib RENAME libmap2check.a)
message(STATUS "Rust runtime: ${_m2c_rust_lib}")

# --- KLEE bitcode (optional) ---
find_program(M2C_LLVM_DIS NAMES llvm-dis-16 llvm-dis
  HINTS "${LLVM_TOOLS_BINARY_DIR}" "${LLVM_DIR}/../../../bin")

if(MAP2CHECK_RUST_KLEE_BC)
  if(NOT EXISTS "${MAP2CHECK_RUST_KLEE_BC}")
    message(FATAL_ERROR "MAP2CHECK_RUST_KLEE_BC=${MAP2CHECK_RUST_KLEE_BC} does not exist")
  endif()
  if(NOT M2C_LLVM_DIS)
    message(FATAL_ERROR "llvm-dis-16 not found: cannot validate MAP2CHECK_RUST_KLEE_BC")
  endif()
  execute_process(COMMAND "${M2C_LLVM_DIS}" "${MAP2CHECK_RUST_KLEE_BC}" -o /dev/null
    RESULT_VARIABLE _m2c_dis_rc ERROR_VARIABLE _m2c_dis_err OUTPUT_QUIET)
  if(NOT _m2c_dis_rc EQUAL 0)
    message(FATAL_ERROR "MAP2CHECK_RUST_KLEE_BC=${MAP2CHECK_RUST_KLEE_BC} cannot be read by ${M2C_LLVM_DIS}: ${_m2c_dis_err}")
  endif()
  install(FILES "${MAP2CHECK_RUST_KLEE_BC}" DESTINATION lib RENAME map2check-klee.bc)
  message(STATUS "Rust runtime KLEE bitcode: ${MAP2CHECK_RUST_KLEE_BC}")
elseif(MAP2CHECK_RUST_SRC AND EXISTS "${MAP2CHECK_RUST_SRC}/scripts/build-klee-runtime.sh")
  # Optional: build it through the library script (needs rustc 1.72.1 + LLVM 16 tools).
  find_program(M2C_RUSTUP rustup)
  set(_m2c_klee_ok FALSE)
  if(M2C_RUSTUP AND M2C_LLVM_DIS)
    execute_process(COMMAND "${M2C_RUSTUP}" run 1.72.1 rustc --version
      RESULT_VARIABLE _m2c_rc OUTPUT_QUIET ERROR_QUIET)
    if(_m2c_rc EQUAL 0)
      set(_m2c_klee_ok TRUE)
    endif()
  endif()
  if(_m2c_klee_ok)
    set(_m2c_klee_bc "${CMAKE_BINARY_DIR}/map2check-klee.bc")
    add_custom_command(OUTPUT "${_m2c_klee_bc}"
      COMMAND "${MAP2CHECK_RUST_SRC}/scripts/build-klee-runtime.sh" "${_m2c_klee_bc}"
      WORKING_DIRECTORY "${MAP2CHECK_RUST_SRC}"
      COMMENT "Building the Rust runtime KLEE bitcode"
      VERBATIM)
    add_custom_target(Map2CheckRustKleeBitcode ALL DEPENDS "${_m2c_klee_bc}")
    install(FILES "${_m2c_klee_bc}" DESTINATION lib RENAME map2check-klee.bc)
    message(STATUS "Rust runtime KLEE bitcode: will be built from ${MAP2CHECK_RUST_SRC}")
  else()
    message(STATUS "Rust runtime KLEE bitcode: skipped (set MAP2CHECK_RUST_KLEE_BC, or provide rustc 1.72.1 + llvm-dis)")
  endif()
else()
  message(STATUS "Rust runtime KLEE bitcode: none (MAP2CHECK_RUST_KLEE_BC unset)")
endif()
