# include_directories Update project to use

add_subdirectory(utils)
add_subdirectory(witness)
add_subdirectory(counter_example)
include_directories(${LLVM_SOURCE}/include)
set(CALLER_FLAGS "-fexceptions -fpic ${LLVM_INCLUDE} -fexceptions")
# set(CMAKE_CXX_FLAGS  "${CALLER_FLAGS}" )
add_library(Caller OBJECT caller.cpp)
set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CALLER_FLAGS})



add_library(Exceptions OBJECT exceptions.cpp)
set_target_properties(Exceptions PROPERTIES COMPILE_FLAGS ${CALLER_FLAGS})


set(LLVM_LIB_PATH "${LLVM_BUILD}/lib/")

set(VERIFIER_FLAGS "-fexceptions -fpic ${LLVM_LIBS}")
set(CMAKE_CXX_FLAGS  "${VERIFIER_FLAGS}" )
# add_library(Verifier OBJECT verifier.cpp)


add_executable(map2check map2check.cpp $<TARGET_OBJECTS:MemoryTrackPass> 
				$<TARGET_OBJECTS:Witness> $<TARGET_OBJECTS:Caller> 
				$<TARGET_OBJECTS:Tools> 
				$<TARGET_OBJECTS:CounterExample> 
				$<TARGET_OBJECTS:GenCryptoHash> 
				$<TARGET_OBJECTS:Exceptions> utils/log.cpp )


target_link_libraries(map2check ${Boost_LIBRARIES} ${CUSTOM_LLVM_LIBS} ${LLVM_SYSTEMFLAGS})


install (TARGETS map2check DESTINATION .)
