#!/bin/sh
export LLVM_DIR_BASE=/llvm/release/llvm600
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
export CXX=$LLVM_DIR_BASE/bin/clang++
export CC=$LLVM_DIR_BASE/bin/clang
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DCMAKE_INSTALL_PREFIX=../release/

ninja
ninja install

if [ ! -d "/home/map2check/devel_tool/clang600" ]; then
   CURRENT_DIR=`pwd`
   cd /home/map2check/devel_tool/
   wget http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
   tar xf clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
   mv clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04 clang600
   rm clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
   cd $CURRENT_DIR
fi
   
cd ../release   
cp -r Z3/lib/* lib/
cp -r /home/map2check/devel_tool/clang600/lib/clang/$LLVM_VERSION/include/* ./include
cp -r /home/map2check/devel_tool/clang600/lib/clang ./lib

# Copying external libraries and binaries
cp /usr/bin/ld ./bin/
cp /usr/lib/x86_64-linux-gnu/libbfd-2.26.1-system.so ./lib
cp /usr/lib/gcc/x86_64-linux-gnu/5/libstdc++.a ./lib/
cp /lib/x86_64-linux-gnu/libpthread.so.0 ./lib/libpthread.so
cp /lib/x86_64-linux-gnu/libdl.so.2 ./lib/libdl.so
cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6 ./lib/libstdc++.so
cp /lib/x86_64-linux-gnu/libm.so.6 ./lib/libm.so
cp /lib/x86_64-linux-gnu/libgcc_s.so.1 ./lib/libgcc_s.so
cp /usr/lib/gcc/x86_64-linux-gnu/5/libgcc.a ./lib/
cp /lib/x86_64-linux-gnu/libc.so.6 ./lib/libc.so
cp /usr/lib/gcc/x86_64-linux-gnu/5/crt* ./lib/
cp /usr/lib/x86_64-linux-gnu/crt* ./lib
cp /lib/x86_64-linux-gnu/librt.so.1 ./lib/librt.so
cp /usr/lib/x86_64-linux-gnu/libgomp.so.1 ./lib/

cd ..
echo ""
echo "Building Crab-LLVM ..."
export CXX=""
export CC=""
./utils/build_crabllvm.py
cp /usr/lib/x86_64-linux-gnu/libbfd-2.26.1-system.so release/bin/crabllvm/lib/
cp /usr/lib/x86_64-linux-gnu/libgomp.so.1 release/bin/crabllvm/lib/
cp release/lib/libz3.so release/bin/crabllvm/lib/

echo ""

echo ""
echo "Copying extra files ..."
./utils/cp_utils_file.sh
echo ""

echo ""
echo "DONE"
echo ""

