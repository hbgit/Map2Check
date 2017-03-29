add_executable (HELLO hello.cpp )

target_link_libraries(HELLO ${Boost_LIBRARIES})

add_test(NAME MyTest COMMAND HELLO)
