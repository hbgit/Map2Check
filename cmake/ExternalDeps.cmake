# This module copies all shared libs into the install folder it should be executed by the docker, it should be fine to install on /opt after this

#TODO: Check if creating build from alpine wont be more compatible/better

# Works on: Ubuntu 14.04, 16.04, 18.04, Fedora, OpenSuse
# Did not work on: Alpine

include(ExternalProject)

if(NOT EXTERNAL_PROJECTS_BASE)
	set(EXTERNAL_PROJECTS_BASE "${CMAKE_BINARY_DIR}/dependencies")
	set_property(DIRECTORY PROPERTY "EP_BASE" ${EXTERNAL_PROJECTS_BASE})
endif()

function(install_link_file LINK DEST NAME)
  execute_process(COMMAND readlink -f ${LINK}
    OUTPUT_VARIABLE EXTERNAL_LIB
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  install(FILES ${EXTERNAL_LIB}
    DESTINATION ${DEST}
    RENAME ${NAME})
endfunction(install_link_file)

function(install_exec_file LINK DEST NAME)
  execute_process(COMMAND readlink -f ${LINK}
    OUTPUT_VARIABLE EXTERNAL_LIB
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  install(FILES ${EXTERNAL_LIB}
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${DEST}
    RENAME ${NAME})
endfunction(install_exec_file)

# Download and install Pre-built clang for ubuntu 14.04
#TODO: Maybe we should manually compile LLVM/Clang?
set(PRE_BUILT_CLANG "clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04")

if(EXISTS dependencies/${PRE_BUILT_CLANG}.tar.xz)
  message("Found pre-built clang for ubuntu")
else()
  message("Will download pre-built clang for ubuntu")
  file(DOWNLOAD http://releases.llvm.org/6.0.0/${PRE_BUILT_CLANG}.tar.xz dependencies/${PRE_BUILT_CLANG}.tar.xz
    SHOW_PROGRESS)
endif()

if(NOT EXISTS dependencies/${PRE_BUILT_CLANG})
  message("Extracting pre-built clang")
  execute_process(COMMAND tar xf ${PRE_BUILT_CLANG}.tar.xz
    WORKING_DIRECTORY dependencies)
endif()

set(PRE_BUILT_CLANG_FOLDER dependencies/${PRE_BUILT_CLANG})

# Clang/LLVM binaries
list(APPEND MAP2CHECK_EXTERNAL_CLANG_BIN "clang")
list(APPEND MAP2CHECK_EXTERNAL_CLANG_BIN "opt")
list(APPEND MAP2CHECK_EXTERNAL_CLANG_BIN "llvm-link")

#Clang/LLVM libs
list(APPEND MAP2CHECK_EXTERNAL_CLANG_LIBS "clang/6.0.0/lib/linux/libclang_rt.fuzzer-x86_64.a")
list(APPEND MAP2CHECK_EXTERNAL_CLANG_LIBS "clang/6.0.0/lib/linux/libclang_rt.ubsan_standalone-x86_64.a")

foreach(B ${MAP2CHECK_EXTERNAL_CLANG_BIN})
  set(BIN ${PRE_BUILT_CLANG_FOLDER}/bin/${B})
  install_exec_file(${BIN} bin ${B})
endforeach()

foreach(L ${MAP2CHECK_EXTERNAL_CLANG_LIBS})
  set(LIB ${PRE_BUILT_CLANG_FOLDER}/lib/${L})
  install_link_file(${LIB} lib ${L})
endforeach()

# Download and compiles static version of Qt5Core 5.11
set(QT_PROJECT "Qt5.11.1-Static")

set(QT_PKG qtbase-everywhere-src-5.11.1)
set(QT_SRC_LINK http://download.qt.io/official_releases/qt/5.11/5.11.1/submodules/${QT_PKG}.tar.xz)
set(QT_SRC_MD5 c656471f138d3810187a523293e2cc28)
set(QT_CONFIGURE "-opensource;-confirm-license;-release;-static;-no-compile-examples;-qt-zlib;-qt-freetype;-qt-pcre;-qt-xcb;-qt-xkbcommon;-no-directfb;-no-linuxfb;-no-gui;-no-opengl")

message(STATUS "Configuring ${QT_PROJECT}")
# TODO: check ninja
# TODO: Specify qt prefix
ExternalProject_ADD(${QT_PROJECT}
  URL ${QT_SRC_LINK}
  URL_MD5 ${QT_SRC_MD5}
  CONFIGURE_COMMAND "${EXTERNAL_PROJECTS_BASE}/Source/${QT_PROJECT}/configure" ${QT_CONFIGURE}
  BUILD_COMMAND "make" "-j4"
  INSTALL_COMMAND "make" install ""
  BUILD_IN_SOURCE 1
  LOG_BUILD 1
  LOG_INSTALL 1
 )
