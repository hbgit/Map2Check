add_executable(HelloGTest
    HelloGTest.cpp
)
target_link_libraries(HelloGTest gtest_main)
add_test(NAME HelloTest COMMAND HelloGTest)
