# This module copies all shared libs into the install folder it should be executed by the docker, it should be fine to install on /opt after this

#TODO: Check if creating build from alpine wont be more compatible/better

# Works on: Ubuntu 14.04, 16.04, 18.04, Fedora, OpenSuse
# Did not work on: Alpine

include(ExternalProject)

if(NOT EXTERNAL_PROJECTS_BASE)
	set(EXTERNAL_PROJECTS_BASE "${CMAKE_BINARY_DIR}/dependencies")
	set_property(DIRECTORY PROPERTY "EP_BASE" ${EXTERNAL_PROJECTS_BASE})
endif()

if(DEV_CONTAINER)
  if(${LLVM_VERSION EQUAL "3.8.0")
    set(PRE_BUILT_CLANG "/llvm/release/380")
  else()
    set(PRE_BUILT_CLANG "/llvm/release/391")
  endif()
  
else()
  #TODO: Maybe we should manually compile LLVM/Clang?
  set(PRE_BUILT_CLANG "clang+llvm-${LLVM_VERSION}-x86_64-linux-gnu-ubuntu-16.04")
  set(PRE_BUILT_CLANG_FOLDER dependencies/${PRE_BUILT_CLANG})

  if(EXISTS dependencies/${PRE_BUILT_CLANG}.tar.xz)
    message("Found pre-built clang for ubuntu")
  else()
    message("Downloading pre-built clang for ubuntu")
    file(DOWNLOAD http://releases.llvm.org/${LLVM_VERSION}/${PRE_BUILT_CLANG}.tar.xz dependencies/${PRE_BUILT_CLANG}.tar.xz
      SHOW_PROGRESS)
  endif()
  
  if(NOT EXISTS dependencies/${PRE_BUILT_CLANG})
    message("Extracting pre-built clang")
    execute_process(COMMAND tar xf ${PRE_BUILT_CLANG}.tar.xz
      WORKING_DIRECTORY dependencies)
  endif()

endif()

