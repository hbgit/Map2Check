if(COPY_EXTERNAL)
  set(Boost_USE_STATIC_LIBS        ON) # only find static libs
  set(Boost_USE_MULTITHREADED      ON)    
endif()

find_package(Boost COMPONENTS program_options system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
