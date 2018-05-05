# include_directories Update project to use


add_executable(map2check map2check.cpp )
target_link_libraries(map2check ${Boost_LIBRARIES})

install (TARGETS map2check DESTINATION .)
