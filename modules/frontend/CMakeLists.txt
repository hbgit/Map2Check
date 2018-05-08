add_subdirectory(utils)

add_executable(map2check map2check.cpp  $<TARGET_OBJECTS:AFL>)
target_link_libraries(map2check ${Boost_LIBRARIES})
install (TARGETS map2check DESTINATION .)
