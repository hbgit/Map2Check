set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE LLVM_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)

set(PASS_FLAGS "${LLVM_INCLUDE}")
message("Pass flags: ${PASS_FLAGS}")
add_library(NonDetPass NonDetPass.cpp DebugInfo.cpp)
set_target_properties(NonDetPass PROPERTIES
COMPILE_FLAGS ${PASS_FLAGS})
