# set(CMAKE_CXX_COMPILER g++)
add_subdirectory(utils)
add_subdirectory(counter_example)
add_subdirectory(witness)
add_library(Caller OBJECT caller.cpp)
#set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})

add_executable(map2check map2check.cpp 
  $<TARGET_OBJECTS:Caller>
  $<TARGET_OBJECTS:CounterExample>
  $<TARGET_OBJECTS:Log>
  $<TARGET_OBJECTS:GenCryptoHash>
  $<TARGET_OBJECTS:Tools>
  $<TARGET_OBJECTS:Edge>
  $<TARGET_OBJECTS:Graph>
  $<TARGET_OBJECTS:Node>
  $<TARGET_OBJECTS:Witness>)

#set_target_properties(map2check PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})
#-lrt -ldl -ltinfo -lpthread -lz -lm
target_link_libraries(map2check ${Boost_LIBRARIES} -D_GLIBCXX_USE_CXX11_ABI=0)
install (TARGETS map2check DESTINATION .)
