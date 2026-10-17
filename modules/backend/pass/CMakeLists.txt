add_library(DebugInfo OBJECT DebugInfo.cpp)
# Names and types of the runtime functions the passes emit (map2check-library).
add_library(RuntimeABI OBJECT RuntimeABI.cpp)
# BUG: If we do not set this flag opt will not work with our pass
# even if it was the same build from llvm
set(CXX_FLAGS "${CXX_FLAGS}  -fno-rtti ")

set_target_properties(DebugInfo PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})
set_target_properties(RuntimeABI PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})
# Both are linked into every plugin (a shared object).
set_target_properties(DebugInfo RuntimeABI PROPERTIES POSITION_INDEPENDENT_CODE ON)

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



foreach(F ${MAP2CHECK_PASS_LIB})  
  add_library(${F} SHARED ${F}.cpp $<TARGET_OBJECTS:DebugInfo>
    $<TARGET_OBJECTS:RuntimeABI>)
  set_target_properties(${F} PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})
  install(TARGETS ${F} DESTINATION lib)  
endforeach()

# TrackBasicBlockPass reads the C source through SourceCodeHelper; without
# these objects the plugin fails to load (undefined SourceCodeHelper::getLine).
target_sources(TrackBasicBlockPass PRIVATE
  ${PROJECT_SOURCE_DIR}/modules/frontend/utils/tools.cpp
  ${PROJECT_SOURCE_DIR}/modules/frontend/utils/log.cpp)
