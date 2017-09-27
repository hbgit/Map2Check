include_directories(${LLVM_SOURCE}/include)

set(PASS_FLAGS "-fexceptions -fpic ${LLVM_INCLUDE} ${LLVM_LIBS}")
# set( CMAKE_CXX_FLAGS  "${PASS_FLAGS}" )

add_library(MemoryTrackPass OBJECT MemoryTrackPass.cpp)
add_library(GenerateAutomataTruePass OBJECT GenerateAutomataTruePass.cpp)

set_target_properties(MemoryTrackPass PROPERTIES COMPILE_FLAGS ${PASS_FLAGS})
set_target_properties(GenerateAutomataTruePass PROPERTIES COMPILE_FLAGS ${PASS_FLAGS})
