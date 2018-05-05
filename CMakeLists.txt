project(Map2Check)

cmake_minimum_required(VERSION 3.5)
message(${CMAKE_SOURCE_DIR})
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

find_package(Boost REQUIRED COMPONENTS program_options system)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})  
endif()

###########################################################################

add_subdirectory(modules)
#enable_testing()
#add_subdirectory(test)
