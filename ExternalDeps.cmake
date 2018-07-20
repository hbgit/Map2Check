# This module copies all shared libs into the install folder it should be executed by the docker, it should be fine to install on /opt after this

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

foreach(L ${MAP2CHECK_EXTERNAL_LIBS})
  set(LIB ${DOCKER_LIB_PATH}${L})
  INSTALL_LINK_FILE(${LIB} lib ${L})
endforeach()
