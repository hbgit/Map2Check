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
mkdir -p ./release/bin

echo ""
echo "Copying external dependencies... "
echo ""

# Copying files from dependencies folders
cp ./dependencies/clang/bin/clang ./release/bin/clang
cp ./dependencies/clang/bin/opt ./release/bin/opt
cp ./dependencies/clang/bin/llvm-link ./release/bin/llvm-link
cp -r ./dependencies/clang/lib/clang/6.0.0/include ./release

cp -r ./dependencies/afl-2.52b/ ./release/afl

echo ""
echo "Creating default dictionaries... "
echo ""

mkdir -p ./release/in
echo "0" > ./release/in/dict0

echo ""
echo "DONE"
echo ""