# set(CMAKE_C_COMPILER ${CLANG})
set(CMAKE_C_EMIT_LLVM_FLAGS -c -emit-llvm)

list(APPEND MAP2CHECK_C_LIB "")
list(APPEND MAP2CHECK_C_LIB "AllocationLog")
list(APPEND MAP2CHECK_C_LIB "AnalysisModeAssert")
list(APPEND MAP2CHECK_C_LIB "AnalysisModeMemcleanup")
list(APPEND MAP2CHECK_C_LIB "AnalysisModeMemtrack")
list(APPEND MAP2CHECK_C_LIB "AnalysisModeNone")
list(APPEND MAP2CHECK_C_LIB "AnalysisModeOverflow")
list(APPEND MAP2CHECK_C_LIB "BTree")
list(APPEND MAP2CHECK_C_LIB "ContainerBTree")
list(APPEND MAP2CHECK_C_LIB "ContainerRealloc")
list(APPEND MAP2CHECK_C_LIB "HeapLog")
list(APPEND MAP2CHECK_C_LIB "ListLog")
list(APPEND MAP2CHECK_C_LIB "Map2CheckFunctions")
list(APPEND MAP2CHECK_C_LIB "NonDetGeneratorNone")
list(APPEND MAP2CHECK_C_LIB "NonDetGeneratorKlee")
list(APPEND MAP2CHECK_C_LIB "NonDetGeneratorLibFuzzy")
list(APPEND MAP2CHECK_C_LIB "NonDetLog")
list(APPEND MAP2CHECK_C_LIB "PropertyGenerator")
list(APPEND MAP2CHECK_C_LIB "TrackBBLog")
list(APPEND MAP2CHECK_C_LIB "WitnessGeneration")
list(APPEND MAP2CHECK_C_LIB "WitnessGenerationNone")

# WasmRuntimeStubs requires WABT include path for wasm-rt.h
# Optional: only compiled when WABT is available (Docker). CI skips it.
# Docker: /opt/wabt-1.0.41/include, CI: apt wabt → /usr/include
if(DEFINED WABT_INCLUDE_DIR)
  set(_wasm_rt_include "${WABT_INCLUDE_DIR}")
elseif(EXISTS "/opt/wabt-1.0.41/include/wasm-rt.h")
  set(_wasm_rt_include "/opt/wabt-1.0.41/include")
elseif(EXISTS "/usr/include/wasm-rt.h")
  set(_wasm_rt_include "/usr/include")
elseif(EXISTS "/usr/local/include/wasm-rt.h")
  set(_wasm_rt_include "/usr/local/include")
else()
  set(_wasm_rt_include "")
endif()

if(_wasm_rt_include)
  message(STATUS "WABT found at ${_wasm_rt_include} — compiling WasmRuntimeStubs")
  add_custom_command(
    OUTPUT "WasmRuntimeStubs.bc"
    COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_EMIT_LLVM_FLAGS}
      -I${_wasm_rt_include}
      ${CMAKE_CURRENT_LIST_DIR}/WasmRuntimeStubs.c
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/WasmRuntimeStubs.c
    COMMENT "Compiling WasmRuntimeStubs to bytecode")
  add_custom_target(WasmRuntimeStubs ALL DEPENDS WasmRuntimeStubs.bc)
  install(FILES ${PROJECT_BINARY_DIR}/modules/backend/library/lib/WasmRuntimeStubs.bc
    DESTINATION lib)
else()
  message(STATUS "WABT not found — skipping WasmRuntimeStubs (WASM support needs Docker)")
endif()

foreach(F ${MAP2CHECK_C_LIB})
  add_custom_command(
    OUTPUT "${F}.bc"
    COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_EMIT_LLVM_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/${F}.c
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${F}.c
    COMMENT "Compiling ${F} to bytecode")

  add_custom_target(${F} ALL DEPENDS ${F}.bc)  
  install(FILES ${PROJECT_BINARY_DIR}/modules/backend/library/lib/${F}.bc
    DESTINATION lib)  
endforeach()

if(ENABLE_TEST)
foreach(F ${MAP2CHECK_C_LIB})
  add_library(${F}Lib OBJECT ${F}.c)
endforeach()
endif()