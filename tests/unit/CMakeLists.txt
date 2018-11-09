function(map2check_test EXECUTABLE)
  target_link_libraries(${EXECUTABLE} gtest_main)
  add_test(NAME ${EXECUTABLE} COMMAND ${EXECUTABLE})
endfunction(map2check_test)

add_executable(HelloGTest
    HelloGTest.cpp
)
map2check_test(HelloGTest)
