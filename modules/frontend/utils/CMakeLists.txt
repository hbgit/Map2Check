
set(VERIFIER_FLAGS "-fexceptions -fpic ${LLVM_LIBS}")

add_library(Tools OBJECT tools.cpp)
set_target_properties(Tools PROPERTIES COMPILE_FLAGS ${CXX_LLVM_FLAGS})

add_library(GenCryptoHash OBJECT gen_crypto_hash.cpp)
set_target_properties(GenCryptoHash PROPERTIES COMPILE_FLAGS ${CXX_LLVM_FLAGS})
