#!/bin/sh
export LLVM_DIR_BASE=/llvm/release/llvm600
export LLVM_VERSION=6.0.0
if [ ! -d "build" ]; then
    mkdir build
fi


echo ""
echo "Building Map2Check test ... "
echo ""

cd build


export LLVM_DIR=$LLVM_DIR_BASE/lib/cmake/llvm
export CXX=$LLVM_DIR_BASE/bin/clang++
export CC=$LLVM_DIR_BASE/bin/clang
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DSKIP_LIB_FUZZER=ON -DSKIP_KLEE=ON -DENABLE_TEST=ON

ninja
