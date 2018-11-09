add_executable(ContainerReallocTest
    ContainerTest.cpp
    $<TARGET_OBJECTS:ContainerReallocLib>
)

add_executable(ContainerBTreeTest
    ContainerTest.cpp
    $<TARGET_OBJECTS:BTreeLib>
    $<TARGET_OBJECTS:ContainerBTreeLib>
)

map2check_test(ContainerReallocTest)
map2check_test(ContainerBTreeTest)