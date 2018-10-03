# FIND LLVM
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

function(find_local_llvm VAR_NAME PROGRAM)
  message(STATUS "Searching ${PROGRAM} in ${SEARCH_PATH}")
  find_program(${VAR_NAME} NAMES ${PROGRAM} PATHS ${SEARCH_PATH}
    NO_DEFAULT_PATH)
  message(STATUS "Found ${${VAR_NAME}}")
endfunction(find_local_llvm)

# if(USE_PREBUILT_CLANG)  
  # 
  # link_directories(${PRE_BUILT_CLANG_FOLDER}/lib)
  # find_local_llvm(CLANG_CC clang)    
  # find_local_llvm(CLANG_CXX clang++)
  # find_local_llvm(LLVM_CONFIG llvm-config)
  #
  
# else()
  
# endif()
find_package(LLVM REQUIRED CONFIG)
set(SEARCH_PATH ${LLVM_TOOLS_BINARY_DIR})
find_local_llvm(CLANG_CC clang)    
find_local_llvm(CLANG_CXX clang++)
find_local_llvm(LLVM_CONFIG_BIN llvm-config)

# Have to find a better way to copy
# install(DIRECTORY ${PROJECT_BINARY_DIR}/${PRE_BUILT_CLANG_FOLDER}/lib/clang/3.9.1/include/ DESTINATION include)

# Check if CLANG is present and configure LLVM
if(NOT CLANG_CC)
  message(FATAL_ERROR "CLANG not found! (Did you execute the bootstrap script?)")
endif()

# Set CLANG as the default C/CXX compiler
set(CMAKE_C_COMPILER ${CLANG_CC})
set(CMAKE_CXX_COMPILER ${CLANG_CXX})

#Confirm clang version
execute_process( COMMAND ${CMAKE_C_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
message(${clang_full_version_string})

if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "LLVM-CONFIG not found! (Did you execute the bootstrap script?)")
endif()

# Get Flags
set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG_BIN} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG_BIN} --cppflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE CPP_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

list(APPEND MAP2CHECK_EXTERNAL_CLANG_BIN "clang")
list(APPEND MAP2CHECK_EXTERNAL_CLANG_BIN "opt")
list(APPEND MAP2CHECK_EXTERNAL_CLANG_BIN "llvm-link")

#Clang/LLVM libs
#list(APPEND MAP2CHECK_EXTERNAL_CLANG_LIBS "clang/6.0.0/lib/linux/libclang_rt.fuzzer-x86_64.a")
#list(APPEND MAP2CHECK_EXTERNAL_CLANG_LIBS "clang/6.0.0/lib/linux/libclang_rt.ubsan_standalone-x86_64.a")

foreach(B ${MAP2CHECK_EXTERNAL_CLANG_BIN})
  set(BIN ${LLVM_TOOLS_BINARY_DIR}/${B})
  install_exec_file(${BIN} bin ${B})
endforeach()

foreach(L ${MAP2CHECK_EXTERNAL_CLANG_LIBS})
  set(LIB ${PRE_BUILT_CLANG_FOLDER}/lib/${L})
  install_link_file(${LIB} lib ${L})
endforeach()
