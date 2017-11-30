set(CMAKE_C_FLAGS -I${KLEE_INCLUDE} )

add_executable(BTreeTests BTreeTests.c ../BTree.c)
add_test(NAME TestBTree COMMAND BTreeTests)
