#!/bin/sh
export LLVM_DIR_BASE=/home/map2check/devel_tool/mygitclone/clang
export LLVM_VERSION=6.0.0
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

export LLVM_DIR=$LLVM_DIR_BASE/lib/cmake/llvm
export CXX=$LLVM_DIR/../../../bin/clang++
export CC=$LLVM_DIR/../../../bin/clang
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DCMAKE_INSTALL_PREFIX=../release/

ninja
ninja install

cd ../release
cp -r $LLVM_DIR_BASE/lib/clang/$LLVM_VERSION/include/* ./include
cp -r $LLVM_DIR_BASE/lib/clang ./lib

# Copying external libraries and binaries
cp /usr/bin/ld ./bin/
cp /usr/lib/x86_64-linux-gnu/libbfd-2.26.1-system.so ./lib
cp /usr/lib/gcc/x86_64-linux-gnu/5/libstdc++.a ./lib/

cd ..
echo ""
echo "Building Crab-LLVM ..."
# TODO: Not working
# ./utils/build_crabllvm.py
echo ""

echo ""
echo "DONE"
echo ""
