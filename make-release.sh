#!/bin/sh

if [ ! -d "build" ]; then
    mkdir build
    cd build
    cmake .. -G Ninja -DCOPY_EXTERNAL=ON -DBUILD_DOC=OFF -DREGRESSION=OFF -DCMAKE_INSTALL_PREFIX=../release/
    cd ..
fi

if [ ! -d "release" ]; then
	mkdir release
fi

echo ""
echo "Building Map2Check release ... "
echo ""

cd build
ninja
ninja install

echo ""
echo "DONE"
echo ""
