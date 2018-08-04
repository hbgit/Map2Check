# set(CMAKE_C_COMPILER ${CLANG})
set(CMAKE_C_FLAGS -c -emit-llvm)

list(APPEND MAP2CHECK_C_LIB "")
list(APPEND MAP2CHECK_C_LIB "AllocationLog")
list(APPEND MAP2CHECK_C_LIB "AnalysisModeMemtrack")
list(APPEND MAP2CHECK_C_LIB "AnalysisModeNone")
#list(APPEND MAP2CHECK_C_LIB "BinaryOperation")
list(APPEND MAP2CHECK_C_LIB "BTree")
list(APPEND MAP2CHECK_C_LIB "ContainerBTree")
list(APPEND MAP2CHECK_C_LIB "ContainerRealloc")
list(APPEND MAP2CHECK_C_LIB "HeapLog")
list(APPEND MAP2CHECK_C_LIB "ListLog")
list(APPEND MAP2CHECK_C_LIB "Map2CheckFunctions")
list(APPEND MAP2CHECK_C_LIB "NonDetGeneratorNone")
list(APPEND MAP2CHECK_C_LIB "NonDetGeneratorLibFuzzy")
list(APPEND MAP2CHECK_C_LIB "NonDetLog")
list(APPEND MAP2CHECK_C_LIB "PropertyGenerator")
list(APPEND MAP2CHECK_C_LIB "TrackBBLog")
list(APPEND MAP2CHECK_C_LIB "WitnessGeneration")
list(APPEND MAP2CHECK_C_LIB "WitnessGenerationNone")

foreach(F ${MAP2CHECK_C_LIB})
  add_custom_command(
    OUTPUT "${F}.bc"
    COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/${F}.c
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${F}.c
    COMMENT "Compiling ${F} to bytecode")

  add_custom_target(${F} ALL DEPENDS ${F}.bc)  
  install(FILES ${PROJECT_BINARY_DIR}/modules/backend/library/lib/${F}.bc
    DESTINATION lib)  
endforeach()
