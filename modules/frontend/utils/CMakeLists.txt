add_library(AFL OBJECT afl.cpp)
set_target_properties(AFL PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})

add_library(Log OBJECT log.cpp)
set_target_properties(Log PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})

add_library(Tools OBJECT tools.cpp)
set_target_properties(Tools PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})

