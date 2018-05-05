project(Map2Check)

cmake_minimum_required(VERSION 3.5)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)

find_package(Boost QUIET REQUIRED COMPONENTS unit_test_framework program_options system)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})  
endif()

###########################################################################

add_subdirectory(modules)
enable_testing()
add_subdirectory(test)
