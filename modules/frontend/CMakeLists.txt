set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

find_package(Qt5Core)

set(SOURCES caller.cpp main.cpp mainclass.cpp)
set(HEADERS caller.hpp mainclass.hpp)

add_executable(map2check ${SOURCES} ${HEADERS})

target_link_libraries(map2check Qt5::Core)
SET_TARGET_PROPERTIES(map2check PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_RPATH})

install (TARGETS map2check DESTINATION .)
