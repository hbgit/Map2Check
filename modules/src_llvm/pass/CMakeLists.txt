include_directories(${LLVM_SOURCE}/include)
set(PASS_FLAGS "-c -fpic ${LLVM_INCLUDE} ${LLVM_LIBS} ")
set( CMAKE_CXX_FLAGS  "${PASS_FLAGS}" )

add_library(MemoryTrackPass OBJECT MemoryTrackPass.cpp)
# install (FILES MemoryTrackPass.h DESTINATION include)
