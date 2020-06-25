#!/bin/sh
export LLVM_VERSION=8.0.0
if [ ! -d "build" ]; then
    mkdir build
fi


echo ""
echo "Building Map2Check test ... "
echo ""

cd build


export LLVM_DIR=/usr/lib/llvm-8/lib/cmake/llvm/
export CXX=/usr/bin/clang++-8
export CC=/usr/bin/clang-8
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DENABLE_TEST=ON

ninja && ninja install && ctest
