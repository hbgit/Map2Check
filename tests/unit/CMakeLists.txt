function(map2check_test EXECUTABLE)
  target_link_libraries(${EXECUTABLE} gtest_main)
  add_test(NAME ${EXECUTABLE} COMMAND ${EXECUTABLE})
endfunction(map2check_test)

# backend-library unit tests exercise the C runtime objects, which are no longer
# built (the runtime is the Rust library). The files stay until their assertions
# are ported to Rust (plan Task 5.2) and are deleted with the C runtime (Task 6.2).
# add_subdirectory(backend-library)
add_subdirectory(frontend)
add_subdirectory(pass)
