#!/bin/sh

if [ ! -d "build" ]; then
	mkdir build
fi

if [ ! -d "release" ]; then
	mkdir release
fi

cd build

echo ""
echo "Building Map2Check release ... "
echo ""

cmake .. -DCMAKE_INSTALL_PREFIX=../release/
make
make install
cd ..
mkdir -p ./release/bin

# Copying files from dependencies folders
cp ./dependencies/clang/bin/clang ./release/bin/clang
cp ./dependencies/clang/bin/opt ./release/bin/opt
cp ./dependencies/clang/bin/llvm-link ./release/bin/llvm-link
