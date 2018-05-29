add_subdirectory(utils)

# add_library(Caller OBJECT caller.cpp)
# set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})
# target_link_libraries(Caller $<TARGET_OBJECTS:AFL>)


# message(${LLVM_INCLUDE})
add_executable(map2check map2check.cpp 
  # $<TARGET_OBJECTS:Caller> 
  $<TARGET_OBJECTS:AFL>
  $<TARGET_OBJECTS:Log> )
# add_executable(map2check map2check.cpp $<TARGET_OBJECTS:Caller>)
# set_target_properties(map2check PROPERTIES COMPILE_FLAGS ${LLVM_INCLUDE})
set_target_properties(map2check PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})
target_link_libraries(map2check ${Boost_LIBRARIES})
# install (TARGETS map2check DESTINATION .)
