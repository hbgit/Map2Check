add_executable(AllocationLogTest
    AllocationLogTest.cpp
    $<TARGET_OBJECTS:AllocationLogLib>
    $<TARGET_OBJECTS:ContainerReallocLib>
)

add_executable(HeapLogTest
    HeapLogTest.cpp
    $<TARGET_OBJECTS:HeapLogLib>
    $<TARGET_OBJECTS:ContainerReallocLib>
)

add_executable(ContainerReallocTest
    ContainerTest.cpp
    $<TARGET_OBJECTS:AllocationLogLib>
    $<TARGET_OBJECTS:ContainerReallocLib>
)

add_executable(BTreeTest
    BTreeTest.cpp
    $<TARGET_OBJECTS:BTreeLib>
)

add_executable(ContainerBTreeTest
    ContainerTest.cpp
    $<TARGET_OBJECTS:AllocationLogLib>
    $<TARGET_OBJECTS:BTreeLib>
    $<TARGET_OBJECTS:ContainerBTreeLib>
)

add_executable(MemTrackTest
    MemTrackTest.cpp
    $<TARGET_OBJECTS:AllocationLogLib>
    $<TARGET_OBJECTS:HeapLogLib>
    $<TARGET_OBJECTS:ListLogLib>
    $<TARGET_OBJECTS:NonDetLogLib>
    $<TARGET_OBJECTS:TrackBBLogLib>
    $<TARGET_OBJECTS:ContainerReallocLib>
    $<TARGET_OBJECTS:AnalysisModeMemtrackLib>
    $<TARGET_OBJECTS:Map2CheckFunctionsLib>
    $<TARGET_OBJECTS:PropertyGeneratorLib>
    $<TARGET_OBJECTS:NonDetGeneratorNoneLib>
    $<TARGET_OBJECTS:WitnessGenerationNoneLib>
)

map2check_test(AllocationLogTest)
map2check_test(HeapLogTest)
map2check_test(ContainerReallocTest)
map2check_test(BTreeTest)
map2check_test(ContainerBTreeTest)
map2check_test(MemTrackTest)