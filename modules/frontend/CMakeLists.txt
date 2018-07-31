add_subdirectory(utils)
add_subdirectory(counter_example)
add_library(Caller OBJECT caller.cpp)
set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})

add_executable(map2check map2check.cpp 
  $<TARGET_OBJECTS:Caller>
  $<TARGET_OBJECTS:CounterExample>
  $<TARGET_OBJECTS:Log>
  $<TARGET_OBJECTS:GenCryptoHash>
  $<TARGET_OBJECTS:Tools>)

set_target_properties(map2check PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})

target_link_libraries(map2check ${Boost_LIBRARIES})
install (TARGETS map2check DESTINATION .)
