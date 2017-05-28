
set(CALLER_FLAGS "-fexceptions -fpic ${LLVM_INCLUDE} -fexceptions")
set(VERIFIER_FLAGS "-fexceptions -fpic ${LLVM_LIBS}")

add_library(Tools OBJECT tools.cpp)
set_target_properties(Tools PROPERTIES COMPILE_FLAGS ${CALLER_FLAGS})

add_library(GenCryptoHash OBJECT gen_crypto_hash.cpp)
set_target_properties(GenCryptoHash PROPERTIES COMPILE_FLAGS "${VERIFIER_FLAGS}")