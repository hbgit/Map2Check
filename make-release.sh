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
cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=../release/
ninja
ninja install

cd ..
echo ""
echo "Building Crab-LLVM ..."
./utils/build_crabllvm.py
echo ""

echo ""
echo "DONE"
echo ""
