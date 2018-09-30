# FIND LLVM 3.8
function(find_local_llvm VAR_NAME PROGRAM)
  message("Searching ${PROGRAM} in ${SEARCH_PATH}")
  find_program(${VAR_NAME} NAMES ${PROGRAM} PATHS ${SEARCH_PATH}
    NO_DEFAULT_PATH)
  message("Found ${${VAR_NAME}}")
endfunction(find_local_llvm)

if(USE_PREBUILT_CLANG)
  if(NOT EXISTS dependencies/${PRE_BUILT_CLANG})
    message(FATAL_ERROR "Could not found pre-built directory")
  else()
    set(SEARCH_PATH ${PROJECT_BINARY_DIR}/${PRE_BUILT_CLANG_FOLDER}/bin)
    link_directories(${PROJECT_BINARY_DIR}/${PRE_BUILT_CLANG_FOLDER}/lib)
    find_local_llvm(CLANG_CC clang-3.8)    
    find_local_llvm(CLANG_CXX clang++)
    find_local_llvm(LLVM_CONFIG llvm-config)
    install(DIRECTORY ${PROJECT_BINARY_DIR}/${PRE_BUILT_CLANG_FOLDER}/lib/clang/3.8.1/include/ DESTINATION include)
  endif()  
else()
  find_program(CLANG_CC clang-3.8)
  find_program(CLANG_CXX clang++-3.8)
  find_program(LLVM_CONFIG llvm-config-3.8 DOC "llvm-config is used to generate flags to link with llvm.")
endif()

# Check if CLANG is present and configure LLVM
if(NOT CLANG_CC)
  message(FATAL_ERROR "CLANG not found! (Did you execute the bootstrap script?)")
endif()

# Set CLANG as the default C/CXX compiler
set(CMAKE_C_COMPILER ${CLANG_CC})
set(CMAKE_CXX_COMPILER ${CLANG_CXX})

#Confirm clang version
execute_process( COMMAND ${CMAKE_C_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
# message(${clang_full_version_string})

if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "LLVM-CONFIG not found! (Did you execute the bootstrap script?)")
endif()

# Get Flags
set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)


set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cppflags --link-static)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE CPP_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)



