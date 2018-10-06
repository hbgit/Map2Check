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
# export LLVM_DIR=/llvm/release/llvm391/lib/cmake/llvm/
export LLVM_DIR=/home/map2check/devel_tool/mygitclone/clang/lib/cmake/llvm
export CXX=$LLVM_DIR/../../../bin/clang++
export CC=$LLVM_DIR/../../../bin/clang
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DCMAKE_INSTALL_PREFIX=../release/

ninja
ninja install

cd ../release
# cp -r /llvm/release/llvm391/lib/clang/3.9.1/include/* ./include
# cp -r ../clang/lib/clang ./lib
# cp ../clang/lib/libc++* ./lib
# cp /usr/bin/ld ./bin/
# cp /usr/lib/x86_64-linux-gnu/libbfd-2.26.1-system.so ./lib
# cp /lib/x86_64-linux-gnu/libpthread.so.0 ./lib/libpthread.so
# cp /lib/x86_64-linux-gnu/libdl.so.2 ./lib/libdl.so
# cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6 ./lib/libstdc++.so
# cp /lib/x86_64-linux-gnu/libm.so.6 ./lib/libm.so
# cp /lib/x86_64-linux-gnu/libgcc_s.so.1 ./lib/libgcc_s.so
# cp /usr/lib/gcc/x86_64-linux-gnu/5/libgcc.a ./lib/
# cp /lib/x86_64-linux-gnu/libc.so.6 ./lib/libc.so

# cp /usr/lib/gcc/x86_64-linux-gnu/5/crt* ./lib/
# cp /usr/lib/x86_64-linux-gnu/crt* ./lib
# cp /lib/x86_64-linux-gnu/librt.so.1 ./lib/librt.so

cd ..
echo ""
echo "Building Crab-LLVM ..."
# TODO: Not working
# ./utils/build_crabllvm.py
echo ""

echo ""
echo "DONE"
echo ""
