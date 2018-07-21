# This module copies all shared libs into the install folder it should be executed by the docker, it should be fine to install on /opt after this

#TODO: Check if creating build from alpine wont be more compatible/better

# Works on: Ubuntu 14.04, 16.04, 18.04, Fedora, OpenSuse
# Did not work on: Alpine

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


set(DOCKER_USR_LIB_PATH "/usr/lib/x86_64-linux-gnu/")

#Qt deps
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libQt5Core.so.5")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libstdc++.so.6")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libicui18n.so.55")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libicuuc.so.55")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libpcre2-16.so.0")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libdouble-conversion.so.1")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libicudata.so.55")

#Clang deps
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libLLVM-6.0.so.1")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libjsoncpp.so.1")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libedit.so.2")


foreach(L ${MAP2CHECK_EXTERNAL_USR_LIBS})
  set(LIB ${DOCKER_USR_LIB_PATH}${L})
  install_link_file(${LIB} lib ${L})
endforeach()

set(DOCKER_LIB_PATH "/lib/x86_64-linux-gnu/")
#Qt libs
list(APPEND MAP2CHECK_EXTERNAL_LIBS "libglib-2.0.so.0")

#Clang libs
list(APPEND MAP2CHECK_EXTERNAL_LIBS "libtinfo.so.5")

# Probably not needed
#list(APPEND MAP2CHECK_EXTERNAL_LIBS "libpcre.so.3")
#list(APPEND MAP2CHECK_EXTERNAL_LIBS "ld-linux-x86-64.so.2")
#list(APPEND MAP2CHECK_EXTERNAL_LIBS "libgcc_s.so.1")

foreach(L ${MAP2CHECK_EXTERNAL_LIBS})
  set(LIB ${DOCKER_LIB_PATH}${L})
  install_link_file(${LIB} lib ${L})
endforeach()


set(DOCKER_USR_BIN_PATH "/usr/bin/")
list(APPEND MAP2CHECK_EXTERNAL_USR_BIN "clang-6.0")
list(APPEND MAP2CHECK_EXTERNAL_USR_BIN "opt-6.0")

foreach(B ${MAP2CHECK_EXTERNAL_USR_BIN})
  set(BIN ${DOCKER_USR_BIN_PATH}${B})
  install_exec_file(${BIN} bin ${B})
endforeach()
