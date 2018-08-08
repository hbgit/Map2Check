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

cmake .. -DCOPY_EXTERNAL=ON -DBUILD_DOC=OFF -DREGRESSION=OFF -DCMAKE_INSTALL_PREFIX=../release/
make
make install

echo ""
echo "DONE"
echo ""
