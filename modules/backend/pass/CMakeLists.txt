set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE LLVM_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)

add_library(NonDetPass NonDetPass.cpp DebugInfo.cpp)
set_target_properties(NonDetPass PROPERTIES
COMPILE_FLAGS ${LLVM_INCLUDE})

add_library(MemoryTrackPass OBJECT MemoryTrackPass.cpp)
set_target_properties(MemoryTrackPass PROPERTIES
COMPILE_FLAGS ${LLVM_INCLUDE})

add_library(Map2CheckLibrary OBJECT Map2CheckLibrary.cpp)
set_target_properties(Map2CheckLibrary PROPERTIES
COMPILE_FLAGS ${LLVM_INCLUDE})

add_library(TargetPass OBJECT TargetPass.cpp)
set_target_properties(TargetPass PROPERTIES
  COMPILE_FLAGS ${LLVM_INCLUDE})
  
# TODO(rafa.sa.xp@gmail.com): fix this build
# add_library(GenerateAutomataTruePass OBJECT GenerateAutomataTruePass.cpp)
# set_target_properties(GenerateAutomataTruePass PROPERTIES 
# 	COMPILE_FLAGS ${LLVM_INCLUDE})

# TODO(rafa.sa.xp@gmail.com): fix this build  
# add_library(OverflowPass OBJECT OverflowPass.cpp)
# set_target_properties(OverflowPass PROPERTIES
#   COMPILE_FLAGS ${LLVM_INCLUDE})
  
# TODO(rafa.sa.xp@gmail.com): fix this build  
# add_library(TrackBasicBlockPass OBJECT TrackBasicBlockPass.cpp)
# set_target_properties(TrackBasicBlockPass PROPERTIES 
# COMPILE_FLAGS ${LLVM_INCLUDE})