# include_directories Update project to use
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)

include_directories(${LLVM_SOURCE}/include)

set(CALLER_FLAGS "-fexceptions -fpic ${LLVM_INCLUDE}")
# set(CMAKE_CXX_FLAGS  "${CALLER_FLAGS}" )
add_library(Caller OBJECT caller.cpp)
set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CALLER_FLAGS})

set(LLVM_LIB_PATH "${LLVM_BUILD}/lib/")

set(VERIFIER_FLAGS "-fexceptions -fpic ${LLVM_LIBS}")
set(CMAKE_CXX_FLAGS  "${VERIFIER_FLAGS}" )
add_library(Verifier OBJECT verifier.cpp)
  
find_package(Boost REQUIRED COMPONENTS log log_setup program_options system filesystem thread)

add_executable(map2check map2check.cpp $<TARGET_OBJECTS:MemoryTrackPass>  $<TARGET_OBJECTS:Caller> $<TARGET_OBJECTS:Verifier> log.cpp )

target_link_libraries(map2check ${Boost_LIBRARIES} ${LLVM_LIBSFLAGS} ${LLVM_SYSTEMFLAGS})

install (TARGETS map2check DESTINATION .)
