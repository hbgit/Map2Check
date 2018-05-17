add_subdirectory(utils)

set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE LLVM_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)

add_library(Caller OBJECT caller.cpp)
set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${LLVM_INCLUDE})

add_executable(map2check map2check.cpp  $<TARGET_OBJECTS:AFL>)
set_target_properties(map2check PROPERTIES COMPILE_FLAGS ${LLVM_INCLUDE})
target_link_libraries(map2check ${Boost_LIBRARIES})
install (TARGETS map2check DESTINATION .)
