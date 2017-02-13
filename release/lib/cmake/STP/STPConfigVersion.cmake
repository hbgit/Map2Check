set(PACKAGE_VERSION "2.1.2")

# STP is API-compatible only with matching major.minor versions
if("2.1" VERSION_EQUAL
    "${PACKAGE_FIND_VERSION_MAJOR}.${PACKAGE_FIND_VERSION_MINOR}")
  set(PACKAGE_VERSION_COMPATIBLE 1)
  if("2" VERSION_EQUAL
      "${PACKAGE_FIND_VERSION_PATCH}")
    set(PACKAGE_VERSION_EXACT 1)
  endif()
endif()

