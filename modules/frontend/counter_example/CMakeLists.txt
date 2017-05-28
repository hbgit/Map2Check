
set(VERIFIER_FLAGS "-fexceptions -fpic ${LLVM_LIBS}")
set(CMAKE_CXX_FLAGS  "${VERIFIER_FLAGS}" )

add_library(CounterExample OBJECT counter_example.cpp)
set_target_properties(CounterExample PROPERTIES COMPILE_FLAGS "${VERIFIER_FLAGS}")