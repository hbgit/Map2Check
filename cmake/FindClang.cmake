# FIND LLVM 6.0
find_program(CLANG_CC clang-6.0)
find_program(CLANG_CXX clang++-6.0)
find_program(LLVM_CONFIG llvm-config-6.0 DOC "llvm-config is used to generate flags to link with llvm.")

# Check if CLANG is present and configure LLVM
if(NOT CLANG_CC)
    message(FATAL_ERROR "CLANG not found! (Did you execute the bootstrap script?)")
endif()

if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "LLVM-CONFIG not found! (Did you execute the bootstrap script?)")
endif()

# Set CLANG as the default C/CXX compiler
set(CMAKE_C_COMPILER ${CLANG_CC})
set(CMAKE_CXX_COMPILER ${CLANG_CXX})

# Get Flags
set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get Flags
set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cppflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE CPP_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
