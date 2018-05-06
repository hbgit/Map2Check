
# AFL
add_executable (__AFL_TEST afl_test.cpp )
add_test(NAME AFL_TEST COMMAND __AFL_TEST)
