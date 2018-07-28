set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(SOURCES caller.cpp main.cpp mainclass.cpp)
set(HEADERS caller.hpp mainclass.hpp)

add_executable(map2check ${SOURCES})

# TODO: fix this hack
if(COPY_EXTERNAL)  
  set(FRONTEND_FLAGS "-static")
else()
    set(FRONTEND_FLAGS "")  
endif()

target_link_libraries(map2check Qt5::Core)

set_target_properties(map2check PROPERTIES COMPILE_FLAGS ${FRONTEND_FLAGS})
set_target_properties(map2check PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_RPATH})



install (TARGETS map2check DESTINATION bin)
