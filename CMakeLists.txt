project(Map2Check)

cmake_minimum_required(VERSION 3.5)
message("Started Map2Check CMake")

set(LLVM_VERSION 3.8.1)  
set(LLVM_SOURCE ${PROJECT_SOURCE_DIR}/dependencies/llvm-${LLVM_VERSION})
set(LLVM_BUILD ${PROJECT_SOURCE_DIR}/dependencies/llvm-build-cmake)

set(KLEE_INCLUDE ${PROJECT_SOURCE_DIR}/dependencies/klee/include)

#####################################################
## TODO: Separate next functions to another file   ##
##       and accept options from cmake input       ##
####################################################

find_program(LLVM_CONFIG llvm-config
  HINTS ${LLVM_BUILD}/bin
  DOC "llvm-config is used to generate flags to link with llvm.")

find_program(CLANG clang
  HINTS ${LLVM_BUILD}/bin)

if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "LLVM not found!")
endif()

set (EXECUTE_LLVM_CXXFLAGS ${LLVM_CONFIG} --cxxflags)
execute_process(COMMAND ${EXECUTE_LLVM_CXXFLAGS} OUTPUT_VARIABLE LLVM_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)

set (EXECUTE_LLVM_LDFLAGS ${LLVM_CONFIG} --ldflags)
execute_process(COMMAND ${EXECUTE_LLVM_LDFLAGS} OUTPUT_VARIABLE LLVM_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
# message("LD ${LLVM_LDFLAGS")
set (EXECUTE_LLVM_LIBSFLAGS ${LLVM_CONFIG} --libs)
execute_process(COMMAND ${EXECUTE_LLVM_LIBSFLAGS} OUTPUT_VARIABLE LLVM_LIBSFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set (EXECUTE_LLVM_SYSTEMFLAGS ${LLVM_CONFIG} --system-libs)
execute_process(COMMAND ${EXECUTE_LLVM_SYSTEMFLAGS} OUTPUT_VARIABLE LLVM_SYSTEMFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CUSTOM_LLVM_LIBS "-lLLVMLTO -lLLVMObjCARCOpts -lLLVMSymbolize -lLLVMDebugInfoPDB -lLLVMDebugInfoDWARF -lLLVMMIRParser -lLLVMLibDriver -lLLVMOption -lLLVMTableGen -lLLVMOrcJIT -lLLVMPasses -lLLVMipo -lLLVMVectorize -lLLVMLinker -lLLVMIRReader -lLLVMAsmParser -lLLVMX86Disassembler -lLLVMX86AsmParser -lLLVMX86CodeGen -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMX86Desc -lLLVMMCDisassembler -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils -lLLVMMCJIT -lLLVMExecutionEngine -lLLVMRuntimeDyld -lLLVMCodeGen -lLLVMTarget -lLLVMScalarOpts -lLLVMInstCombine -lLLVMInstrumentation -lLLVMProfileData -lLLVMObject -lLLVMMCParser -lLLVMTransformUtils -lLLVMMC -lLLVMBitWriter -lLLVMBitReader -lLLVMAnalysis -lLLVMCore -lLLVMSupport")


set(LLVM_LIBS "${LLVM_LDFLAGS} ${CUSTOM_LLVM_LIBS} ${LLVM_SYSTEMFLAGS}")

###########################################################################



add_subdirectory(modules)

