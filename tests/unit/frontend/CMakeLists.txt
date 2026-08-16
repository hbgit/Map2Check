add_executable(Sha256Test
    Sha256Test.cpp
    $<TARGET_OBJECTS:Sha256>
)
map2check_test(Sha256Test)

add_executable(TestSuiteTest
    TestSuiteTest.cpp
    $<TARGET_OBJECTS:TestSuite>
)
map2check_test(TestSuiteTest)
