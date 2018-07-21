# This module copies all shared libs into the install folder it should be executed by the docker, it should be fine to install on /opt after this

#TODO: Check if creating build from alpine wont be more compatible/better

# Works on: Ubuntu 14.04, 16.04, 18.04, Fedora, OpenSuse
# Did not work on: Alpine

set(DOCKER_USR_LIB_PATH "/usr/lib/x86_64-linux-gnu/")

function(INSTALL_LINK_FILE link dest name)
  execute_process(COMMAND readlink -f ${link}
    OUTPUT_VARIABLE EXTERNAL_LIB
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  install(FILES ${EXTERNAL_LIB}
    DESTINATION ${dest}
    RENAME ${name})
endfunction(INSTALL_LINK_FILE)
  
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libQt5Core.so.5")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libstdc++.so.6")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libicui18n.so.55")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libicuuc.so.55")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libpcre2-16.so.0")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libdouble-conversion.so.1")
list(APPEND MAP2CHECK_EXTERNAL_USR_LIBS "libicudata.so.55")

foreach(L ${MAP2CHECK_EXTERNAL_USR_LIBS})
  set(LIB ${DOCKER_USR_LIB_PATH}${L})
  INSTALL_LINK_FILE(${LIB} lib ${L})
endforeach()

set(DOCKER_LIB_PATH "/lib/x86_64-linux-gnu/")
list(APPEND MAP2CHECK_EXTERNAL_LIBS "libglib-2.0.so.0")

# Probably not needed
#list(APPEND MAP2CHECK_EXTERNAL_LIBS "libpcre.so.3")
#list(APPEND MAP2CHECK_EXTERNAL_LIBS "ld-linux-x86-64.so.2")
#list(APPEND MAP2CHECK_EXTERNAL_LIBS "libgcc_s.so.1")

foreach(L ${MAP2CHECK_EXTERNAL_LIBS})
  set(LIB ${DOCKER_LIB_PATH}${L})
  INSTALL_LINK_FILE(${LIB} lib ${L})
endforeach()
