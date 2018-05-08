set(CMAKE_C_COMPILER ${CLANG})
set(CMAKE_C_FLAGS -c -emit-llvm)

list(APPEND MAP2CHECK_C_LIB "")
list(APPEND MAP2CHECK_C_LIB "AllocationLog")
list(APPEND MAP2CHECK_C_LIB "Container")
list(APPEND MAP2CHECK_C_LIB "KleeLog")
list(APPEND MAP2CHECK_C_LIB "ListLog")
list(APPEND MAP2CHECK_C_LIB "HeapLog")
list(APPEND MAP2CHECK_C_LIB "PropertyGenerator")
list(APPEND MAP2CHECK_C_LIB "Map2CheckFunctions")
list(APPEND MAP2CHECK_C_LIB "BinaryOperation")
list(APPEND MAP2CHECK_C_LIB "TrackBBLog")
list(APPEND MAP2CHECK_C_LIB "BTree")

foreach(F ${MAP2CHECK_C_LIB})
  add_custom_command(
    OUTPUT "${F}.bc"
    COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/${F}.c
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${F}.c
    ${CMAKE_CURRENT_LIST_DIR}/${F}.h
    ${CMAKE_CURRENT_LIST_DIR}/Map2CheckTypes.h
    ${CMAKE_CURRENT_LIST_DIR}/Map2CheckGlobals.h
    ${CMAKE_CURRENT_LIST_DIR}/Container.h
    ${CMAKE_CURRENT_LIST_DIR}/BTree.h    
    COMMENT "Compiling ${F} to bytecode")

  add_custom_target(${F} ALL DEPENDS ${F}.bc)  

  install(FILES ${PROJECT_BINARY_DIR}/modules/backend/library/${F}.bc
    DESTINATION lib)  

endforeach()
#add_subdirectory(test)
