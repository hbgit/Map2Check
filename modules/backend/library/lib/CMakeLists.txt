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

# WasmRuntimeStubs: only needed for WASM mode, optional for CI builds
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/WasmRuntimeStubs.c")
  find_path(WASM_RT_INCLUDE wasm-rt.h
    PATHS /opt/wabt-1.0.41/include /usr/include /usr/local/include
    NO_DEFAULT_PATH)
  if(WASM_RT_INCLUDE)
    message(STATUS "WABT found at ${WASM_RT_INCLUDE} — compiling WasmRuntimeStubs")
    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WasmRuntimeStubs.bc"
      COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_EMIT_LLVM_FLAGS}
        -I${WASM_RT_INCLUDE}
        ${CMAKE_CURRENT_LIST_DIR}/WasmRuntimeStubs.c
      DEPENDS ${CMAKE_CURRENT_LIST_DIR}/WasmRuntimeStubs.c
      COMMENT "Compiling WasmRuntimeStubs to bytecode")
    add_custom_target(WasmRuntimeStubs DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/WasmRuntimeStubs.bc")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/WasmRuntimeStubs.bc" DESTINATION lib)
  else()
    message(WARNING "wasm-rt.h not found — WasmRuntimeStubs skipped (WASM needs Docker)")
  endif()
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