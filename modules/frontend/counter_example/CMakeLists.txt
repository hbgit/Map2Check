add_library(CounterExample OBJECT counter_example.cpp)
set_target_properties(CounterExample PROPERTIES COMPILE_FLAGS "${CPP_FLAGS}")
