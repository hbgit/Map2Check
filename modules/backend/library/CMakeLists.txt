set(CMAKE_C_COMPILER ${CLANG})
set(CMAKE_C_FLAGS -I${KLEE_INCLUDE} -c -emit-llvm)

message("Building memory utils")
# execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/MemoryUtils.c)

execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/AllocationLog.c)
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/Container.c)
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/KleeLog.c)
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/ListLog.c)
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/HeapLog.c)
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/PropertyGenerator.c)
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/Map2CheckFunctions.c)
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/BinaryOperation.c)
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/TrackBBLog.c)


# execute_process(COMMAND ${LLVM_LINK} ${PROJECT_BINARY_DIR}/AllocationLog.bc  ${PROJECT_BINARY_DIR}/Map2CheckFunctions.bc >  ${PROJECT_BINARY_DIR}/Result.bc )
# ${PROJECT_BINARY_DIR}/Container.bc  ${PROJECT_BINARY_DIR}/KleeLog.bc ${PROJECT_BINARY_DIR}/ListLog.bc ${PROJECT_BINARY_DIR}/PropertyGenerator.bc

# install(FILES ${PROJECT_BINARY_DIR}/MemoryUtils.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/AllocationLog.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/Container.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/KleeLog.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/ListLog.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/HeapLog.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/PropertyGenerator.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/Map2CheckFunctions.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/BinaryOperation.bc DESTINATION lib)
install(FILES ${PROJECT_BINARY_DIR}/TrackBBLog.bc DESTINATION lib)
