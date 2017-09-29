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

add_library(Map2CheckLibrary OBJECT Map2CheckLibrary.cpp)
set_target_properties(Map2CheckLibrary PROPERTIES
  COMPILE_FLAGS ${PASS_FLAGS})

add_library(TargetPass OBJECT TargetPass.cpp)
set_target_properties(TargetPass PROPERTIES
  COMPILE_FLAGS ${PASS_FLAGS})
  
add_library(GenerateAutomataTruePass OBJECT GenerateAutomataTruePass.cpp)
set_target_properties(GenerateAutomataTruePass PROPERTIES 
	COMPILE_FLAGS ${PASS_FLAGS})
