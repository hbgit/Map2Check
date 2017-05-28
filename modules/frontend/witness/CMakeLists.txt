
set(VERIFIER_FLAGS "-fexceptions -fpic ${LLVM_LIBS}")
set(CMAKE_CXX_FLAGS  "${VERIFIER_FLAGS}" )

add_library(Witness OBJECT witness.cpp)
set_target_properties(Witness PROPERTIES COMPILE_FLAGS ${VERIFIER_FLAGS})
