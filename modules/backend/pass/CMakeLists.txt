include_directories(${LLVM_SOURCE}/include)

set(PASS_FLAGS "-fexceptions -fpic ${LLVM_INCLUDE} ${LLVM_LIBS}")
# set( CMAKE_CXX_FLAGS  "${PASS_FLAGS}" )

add_library(MemoryTrackPass OBJECT MemoryTrackPass.cpp)
set_target_properties(MemoryTrackPass PROPERTIES
  COMPILE_FLAGS ${PASS_FLAGS})
  
add_library(NonDetPass OBJECT NonDetPass.cpp)
set_target_properties(NonDetPass PROPERTIES
  COMPILE_FLAGS ${PASS_FLAGS})
  
add_library(DebugInfo OBJECT DebugInfo.cpp)
set_target_properties(DebugInfo PROPERTIES
  COMPILE_FLAGS ${PASS_FLAGS})
