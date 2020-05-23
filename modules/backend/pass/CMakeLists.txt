add_library(DebugInfo OBJECT DebugInfo.cpp)
# BUG: If we do not set this flag opt will not work with our pass
# even if it was the same build from llvm
set(CXX_FLAGS "${CXX_FLAGS}  -fno-rtti ")

set_target_properties(DebugInfo PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})

list(APPEND MAP2CHECK_PASS_LIB "")
list(APPEND MAP2CHECK_PASS_LIB "NonDetPass")
list(APPEND MAP2CHECK_PASS_LIB "MemoryTrackPass")
list(APPEND MAP2CHECK_PASS_LIB "Map2CheckLibrary")
list(APPEND MAP2CHECK_PASS_LIB "TargetPass")
list(APPEND MAP2CHECK_PASS_LIB "TrackBasicBlockPass")
list(APPEND MAP2CHECK_PASS_LIB "GenerateAutomataTruePass")
list(APPEND MAP2CHECK_PASS_LIB "AssertPass")
list(APPEND MAP2CHECK_PASS_LIB "OverflowPass")
list(APPEND MAP2CHECK_PASS_LIB "LoopPredAssumePass")
list(APPEND MAP2CHECK_PASS_LIB "CheckNonDetFunctPass")



foreach(F ${MAP2CHECK_PASS_LIB})  
  add_library(${F} SHARED ${F}.cpp $<TARGET_OBJECTS:DebugInfo>)
  set_target_properties(${F} PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})
  install(TARGETS ${F} DESTINATION lib)  
endforeach()
