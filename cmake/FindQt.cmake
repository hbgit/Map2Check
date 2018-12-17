#Download and compile Qt, this has to happen after configuring static clang (since we will use it)
if(COPY_EXTERNAL)
  # Configure static version of Qt5Core 5.11
  set(QT_PROJECT "Qt5.11.1-Static")

  set(QT_PKG qtbase-everywhere-src-5.11.1)
  set(QT_SRC_LINK http://download.qt.io/official_releases/qt/5.11/5.11.1/submodules/${QT_PKG}.tar.xz)
  set(QT_SRC_MD5 c656471f138d3810187a523293e2cc28)
  set(QT_CONFIGURE "-opensource;-confirm-license;-release;-static;-no-compile-examples;-qt-freetype;-qt-pcre;-qt-xcb;-qt-xkbcommon;-no-gui;-no-opengl")
  set(QT_FOLDER ${EXTERNAL_PROJECTS_BASE}/Install/${QT_PROJECT})

  # TODO: check ninja
  ExternalProject_ADD(${QT_PROJECT}
    URL ${QT_SRC_LINK}
    URL_MD5 ${QT_SRC_MD5}
    CONFIGURE_COMMAND "${EXTERNAL_PROJECTS_BASE}/Source/${QT_PROJECT}/configure" ${QT_CONFIGURE} -prefix ${QT_FOLDER} -platform linux-clang
    BUILD_COMMAND "make" "-j4"
    INSTALL_COMMAND "make" install ""
    BUILD_IN_SOURCE 1
    LOG_BUILD 1
    LOG_INSTALL 1)
  
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_FOLDER})
  message("Set prefix to ${CMAKE_PREFIX_PATH}")
  include_directories(${QT_FOLDER}/include/)
  link_directories(${QT_FOLDER}/lib)

  include(${QT_FOLDER}/lib/cmake/Qt5Core/Qt5CoreConfig.cmake)
endif()

find_package(Qt5Core REQUIRED)
get_target_property(QtCore_location Qt5::Core LOCATION)
message("Using qt core from: ${QtCore_location}")
