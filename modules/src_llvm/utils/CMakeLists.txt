set(CMAKE_C_COMPILER ${CLANG})
set(CMAKE_C_FLAGS -I${KLEE_INCLUDE} -c -emit-llvm)

message("Building memory utils")
execute_process(COMMAND ${CLANG} ${CMAKE_C_FLAGS} ${CMAKE_CURRENT_LIST_DIR}/MemoryUtils.c)

install(FILES ${PROJECT_BINARY_DIR}/MemoryUtils.bc DESTINATION lib)
