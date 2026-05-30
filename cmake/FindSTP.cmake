# FindSTP.cmake — Locate STP solver (pre-installed in container)
#
# In the Dockerfile.dev environment, STP 2.3.4 is built from source:
#   git clone -b 2.3.4 https://github.com/stp/stp.git
#   cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_SHARED_LIBS=ON && make install
#
# Sets:
#   STP_INCLUDE_DIRS  — STP header directory
#   STP_LIBRARIES     — STP library path

find_library(STP_LIBRARIES NAMES stp
  PATHS /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu)
find_path(STP_INCLUDE_DIRS NAMES stp/c_interface.h
  PATHS /usr/local/include /usr/include)

if(STP_LIBRARIES AND STP_INCLUDE_DIRS)
  message(STATUS "Found STP: ${STP_LIBRARIES} (headers: ${STP_INCLUDE_DIRS})")
else()
  message(FATAL_ERROR "STP not found! Build from source: https://github.com/stp/stp.git tag 2.3.4")
endif()
