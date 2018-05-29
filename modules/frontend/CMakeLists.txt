add_subdirectory(utils)

#option(USE_CXX_EXCEPTIONS "Enable C++ exception support" ON)

# add_library(Caller OBJECT caller.cpp)
# set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CXX_FLAGS})
# target_link_libraries(Caller $<TARGET_OBJECTS:AFL>)

set (EXECUTE_LLVM_HEADERS_FLAGS ${LLVM_CONFIG} --cppflags)
execute_process(COMMAND ${EXECUTE_LLVM_HEADERS_FLAGS} OUTPUT_VARIABLE LLVM_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
set (EXECUTE_LLVM_HEADERS_FLAGS ${LLVM_CONFIG} --libs)
execute_process(COMMAND ${EXECUTE_LLVM_HEADERS_FLAGS} OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(CONCAT LLVM_FLAGS ${LLVM_INCLUDE} " " ${LLVM_LIBS})

# message(${LLVM_INCLUDE})
add_executable(map2check map2check.cpp 
  # $<TARGET_OBJECTS:Caller> 
  $<TARGET_OBJECTS:AFL>
  $<TARGET_OBJECTS:Log> )
# add_executable(map2check map2check.cpp $<TARGET_OBJECTS:Caller>)
#set_target_properties(map2check PROPERTIES COMPILE_FLAGS ${LLVM_INCLUDE})
set_target_properties(map2check PROPERTIES COMPILE_FLAGS "${CXX_FLAGS} -fexceptions")
target_link_libraries(map2check ${Boost_LIBRARIES})
install (TARGETS map2check DESTINATION .)
