add_library(DebugInfo OBJECT DebugInfo.cpp)
set_target_properties(DebugInfo PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})

list(APPEND MAP2CHECK_PASS_LIB "")
list(APPEND MAP2CHECK_PASS_LIB "NonDetPass")
list(APPEND MAP2CHECK_PASS_LIB "MemoryTrackPass")
list(APPEND MAP2CHECK_PASS_LIB "Map2CheckLibrary")
list(APPEND MAP2CHECK_PASS_LIB "TargetPass")

# TODO(rafa.sa.xp@gmail.com): fix for llvm 6
# list(APPEND MAP2CHECK_PASS_LIB "GenerateAutomataTruePass")
# list(APPEND MAP2CHECK_PASS_LIB "OverflowPass")
# list(APPEND MAP2CHECK_PASS_LIB "TrackBasicBlockPass")

foreach(F ${MAP2CHECK_PASS_LIB})
  add_library(${F} SHARED ${F}.cpp $<TARGET_OBJECTS:DebugInfo>)
  set_target_properties(${F} PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})
  install(TARGETS ${F} DESTINATION lib)  
endforeach()