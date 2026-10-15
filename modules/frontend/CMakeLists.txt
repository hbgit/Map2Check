# set(CMAKE_CXX_COMPILER g++)
add_subdirectory(utils)
add_subdirectory(counter_example)
add_subdirectory(witness)
add_subdirectory(test_suite)
# Version string embedded in the binary (--version / help banner).
# The release pipeline overrides it with -DMAP2CHECK_VERSION=<semver>;
# regular builds fall back to the project() version.
if(NOT MAP2CHECK_VERSION)
  set(MAP2CHECK_VERSION ${PROJECT_VERSION})
endif()
execute_process(COMMAND date -u +%Y-%m-%d
  OUTPUT_VARIABLE MAP2CHECK_BUILD_DATE
  OUTPUT_STRIP_TRAILING_WHITESPACE)
configure_file(map2check_version.hpp.in
  "${CMAKE_CURRENT_BINARY_DIR}/map2check_version.hpp" @ONLY)

add_library(Caller OBJECT caller.cpp)
add_library(WasmLifter OBJECT wasm_lifter.cpp)
#set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})

add_executable(map2check map2check.cpp 
  $<TARGET_OBJECTS:Caller>
  $<TARGET_OBJECTS:WasmLifter>
  $<TARGET_OBJECTS:CounterExample>
  $<TARGET_OBJECTS:Log>
  $<TARGET_OBJECTS:GenCryptoHash>
  $<TARGET_OBJECTS:Tools>
  $<TARGET_OBJECTS:Edge>
  $<TARGET_OBJECTS:Graph>
  $<TARGET_OBJECTS:Node>
  $<TARGET_OBJECTS:Witness>
  $<TARGET_OBJECTS:TestSuite>
  $<TARGET_OBJECTS:Sha256>)

target_include_directories(map2check PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

#set_target_properties(map2check PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})
#-lrt -ldl -ltinfo -lpthread -lz -lm
target_link_libraries(map2check ${Boost_LIBRARIES})

# --alternate-engines watches KLEE's progress in its SQLite stats database
# (tacas 3b). Optional: without SQLite -- the CI images do not install it --
# KLEE runs to its window and only the fuzzer stops on stagnation.
find_package(Threads REQUIRED)
target_link_libraries(map2check Threads::Threads)
find_package(SQLite3)
if(SQLite3_FOUND)
  target_compile_definitions(Caller PRIVATE MAP2CHECK_HAVE_SQLITE)
  target_include_directories(Caller PRIVATE ${SQLite3_INCLUDE_DIRS})
  target_link_libraries(map2check ${SQLite3_LIBRARIES} ${CMAKE_DL_LIBS})
  message(STATUS "SQLite3 found: KLEE stagnation detection enabled")
else()
  message(STATUS "SQLite3 not found: KLEE stagnation detection disabled")
endif()
install (TARGETS map2check DESTINATION .)
