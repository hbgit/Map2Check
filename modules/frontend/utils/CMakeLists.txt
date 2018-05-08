set(CXX_LLVM_FLAGS "-fpic -fexceptions -std=c++1z")

add_library(AFL OBJECT afl.cpp)
set_target_properties(AFL PROPERTIES COMPILE_FLAGS ${CXX_LLVM_FLAGS})
