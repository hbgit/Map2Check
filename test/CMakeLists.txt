add_executable (HELLO hello.cpp )

target_link_libraries(HELLO ${Boost_LIBRARIES})
enable_testing()
add_test(NAME MyTest COMMAND HELLO)
