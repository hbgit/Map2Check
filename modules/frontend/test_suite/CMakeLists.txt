# One object library for both sources: they are two halves of the same job --
# serialising a Test-Comp suite -- and the frontend links the pair together in
# every configuration.
add_library(TestSuite OBJECT test_suite.cpp ktest_reader.cpp)
set_target_properties(TestSuite PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})
