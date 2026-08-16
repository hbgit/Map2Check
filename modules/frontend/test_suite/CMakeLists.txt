add_library(TestSuite OBJECT test_suite.cpp)
set_target_properties(TestSuite PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})
