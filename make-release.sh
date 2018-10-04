#!/bin/sh

if [ ! -d "build" ]; then
    mkdir build
fi

if [ ! -d "release" ]; then
	mkdir release
fi

echo ""
echo "Building Map2Check release ... "
echo ""

cd build
cmake .. -G Ninja -DLLVM_DIR=/llvm/release/llvm391/lib/cmake/llvm/ -DCMAKE_INSTALL_PREFIX=../release/
ninja
ninja install
cp -r /llvm/release/llvm391/lib/clang/3.9.1/include/* ./include

cd ../release
cp -r 

cd ..
echo ""
echo "Building Crab-LLVM ..."
# TODO: Not working
# ./utils/build_crabllvm.py
echo ""

echo ""
echo "DONE"
echo ""
