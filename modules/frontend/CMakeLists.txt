# include_directories Update project to use

# static linking
#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBRARIES OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")
set(CXX_LLVM_FLAGS "-fpic ${LLVM_INCLUDE} -fexceptions -std=c++1z")
add_subdirectory(utils)
add_subdirectory(witness)
add_subdirectory(counter_example)
include_directories(${LLVM_SOURCE}/include)

# set(CMAKE_CXX_FLAGS  "${CALLER_FLAGS}" )
add_library(Caller OBJECT caller.cpp)
set_target_properties(Caller PROPERTIES COMPILE_FLAGS ${CXX_LLVM_FLAGS})

add_library(Exceptions OBJECT exceptions.cpp)
set_target_properties(Exceptions PROPERTIES COMPILE_FLAGS ${CXX_LLVM_FLAGS})


# Handle NCURSES
#set(CURSES_NEED_NCURSES TRUE)
#if(CURSES_FOUND)
#  include_directories(${CURSES_INCLUDE_DIR})  
#  set(CURSES_LIBRARIES ${CURSES_LIBRARY})
#endif()


set(LLVM_LIB_PATH "${LLVM_BUILD}/lib/")

set(VERIFIER_FLAGS "-fexceptions -fpic ${LLVM_LIBS}")
set(CMAKE_CXX_FLAGS  "${VERIFIER_FLAGS}" )
# add_library(Verifier OBJECT verifier.cpp)


add_executable(map2check map2check.cpp $<TARGET_OBJECTS:MemoryTrackPass>
				$<TARGET_OBJECTS:GenerateAutomataTruePass>
				$<TARGET_OBJECTS:TrackBasicBlockPass>
                $<TARGET_OBJECTS:NonDetPass> $<TARGET_OBJECTS:DebugInfo>
		$<TARGET_OBJECTS:Map2CheckLibrary> $<TARGET_OBJECTS:TargetPass>
		$<TARGET_OBJECTS:OverflowPass>
		$<TARGET_OBJECTS:Witness> $<TARGET_OBJECTS:Edge> $<TARGET_OBJECTS:Graph> $<TARGET_OBJECTS:Node>
		$<TARGET_OBJECTS:Caller> $<TARGET_OBJECTS:Tools>
		$<TARGET_OBJECTS:CounterExample> $<TARGET_OBJECTS:GenCryptoHash>
		$<TARGET_OBJECTS:Exceptions> utils/log.cpp )


target_link_libraries(map2check ${Boost_LIBRARIES} ${CURSES_LIBRARIES} ${CUSTOM_LLVM_LIBS} ${LLVM_SYSTEMFLAGS})


install (TARGETS map2check DESTINATION .)
