# include_directories Update project to use

include_directories(${LLVM_SOURCE}/include)

set(CALLER_FLAGS "-fexceptions -fpic ${LLVM_INCLUDE} -fexceptions")
# set(CMAKE_CXX_FLAGS  "${CALLER_FLAGS}" )
add_library(Caller OBJECT caller.cpp)
set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CALLER_FLAGS})

add_library(Tools OBJECT tools.cpp)
set_target_properties(Tools PROPERTIES COMPILE_FLAGS ${CALLER_FLAGS})

add_library(Exceptions OBJECT exceptions.cpp)
set_target_properties(Exceptions PROPERTIES COMPILE_FLAGS ${CALLER_FLAGS})



set(LLVM_LIB_PATH "${LLVM_BUILD}/lib/")

set(VERIFIER_FLAGS "-fexceptions -fpic ${LLVM_LIBS}")
set(CMAKE_CXX_FLAGS  "${VERIFIER_FLAGS}" )
add_library(Verifier OBJECT verifier.cpp)

add_library(CounterExample OBJECT counter_example.cpp)
set_target_properties(CounterExample PROPERTIES COMPILE_FLAGS "${VERIFIER_FLAGS}")

add_library(Witness OBJECT witness.cpp)
set_target_properties(Witness PROPERTIES COMPILE_FLAGS ${VERIFIER_FLAGS})

add_library(GenCryptoHash OBJECT gen_crypto_hash.cpp)
set_target_properties(GenCryptoHash PROPERTIES COMPILE_FLAGS "${VERIFIER_FLAGS}")

add_executable(map2check map2check.cpp $<TARGET_OBJECTS:MemoryTrackPass> 
				$<TARGET_OBJECTS:Witness> $<TARGET_OBJECTS:Caller> 
				$<TARGET_OBJECTS:Tools> 
				$<TARGET_OBJECTS:CounterExample> 
				$<TARGET_OBJECTS:GenCryptoHash> 
				$<TARGET_OBJECTS:Exceptions> log.cpp )


target_link_libraries(map2check ${Boost_LIBRARIES} ${CUSTOM_LLVM_LIBS} ${LLVM_SYSTEMFLAGS})


install (TARGETS map2check DESTINATION .)
