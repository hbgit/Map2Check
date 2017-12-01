set(CMAKE_C_FLAGS "-g -O0 " )

add_executable(BTreeTests BTreeTests.c ../BTree.c)

add_test(NAME TestBTree COMMAND BTreeTests)
