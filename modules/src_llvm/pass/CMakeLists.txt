# project(MemoryTrackPass)
set(PASS_FLAGS "-c -fpic ${INC} ${LLVM_INCLUDE} ${LLVM_LIBS} ")

# set(CMAKE_CXX_FLAGS "")
SET( CMAKE_CXX_FLAGS  "${PASS_FLAGS}" )

add_library(MemoryTrackPass OBJECT MemoryTrackPass.cpp)

# install (TARGETS MemoryTrackPass DESTINATION bin)
install (FILES MemoryTrackPass.h DESTINATION include)
