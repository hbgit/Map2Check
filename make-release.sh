#!/bin/sh
export LLVM_DIR_BASE=/llvm/release/llvm600
export LLVM_VERSION=6.0.0

export_svcomp=true

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
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DSKIP_LIB_FUZZER=ON -DSKIP_KLEE=ON  -DCMAKE_INSTALL_PREFIX=../release/

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

mkdir -p ./include
cp -r /home/map2check/devel_tool/clang600/lib/clang/$LLVM_VERSION/include/* ./include
cp -r /home/map2check/devel_tool/clang600/lib/clang ./lib

# Copying external libraries and binaries
cp /usr/bin/ld ./bin
cp /usr/lib/x86_64-linux-gnu/libbfd-* ./lib
cp /usr/lib/gcc/x86_64-linux-gnu/5/libstdc++.a ./lib/
cp /lib/x86_64-linux-gnu/libpthread.so.0 ./lib/libpthread.so
cp /lib/x86_64-linux-gnu/libdl.so.2 ./lib/libdl.so
cp /usr/lib/x86_64-linux-gnu/libstdc++* ./lib/
cp /lib/x86_64-linux-gnu/libm.so.6 ./lib/libm.so
cp /lib/x86_64-linux-gnu/libgcc_s.so.1 ./lib/libgcc_s.so
cp /usr/lib/gcc/x86_64-linux-gnu/5/libgcc.a ./lib/
cp /lib/x86_64-linux-gnu/libc.so.6 ./lib/libc.so
cp /usr/lib/gcc/x86_64-linux-gnu/5/crt* ./lib/
cp /usr/lib/x86_64-linux-gnu/crt* ./lib
cp /lib/x86_64-linux-gnu/librt.so.1 ./lib/librt.so
cp /usr/lib/x86_64-linux-gnu/libgomp.so.1 ./lib/

echo ""
echo "Copying external tools"
# LibFuzzer
cp /deps/install/fuzzer/libFuzzer.a ./lib

# Z3
if [ ! -d "./z3" ]; then
    cp -r /deps/install/z3 .
    cp -r z3/lib/* lib/
fi

# Klee and klee_uclib 
cp -r /deps/install/klee/* .

# metasmt
echo "> Copying metaSMT deps ..."
cp -r /deps/install/metasmt/lib/* lib/
cp -r /deps/install/metasmt/include/* include/
# library from metasmt
cp -r /deps/src/metaSMT/deps/boolector-2.2.0/lib/libboolector.so lib/
cp -r /deps/src/metaSMT/deps/lingeling-ayv-86bf266-140429/lib/liblingeling.so lib/
cp -r /deps/src/metaSMT/deps/yices-2.5.1/lib/libyices.so.2.5.1 lib/
cp -r /deps/src/metaSMT/deps/yices-2.5.1/lib/libyices.so.2.5 lib/
cp -r /deps/src/metaSMT/deps/yices-2.5.1/lib/libyices.so lib/
cp -r /deps/src/metaSMT/deps/minisat-git/lib/libminisat.a lib/
cp -r /deps/src/metaSMT/deps/minisat-git/lib/libminisat.so lib/
cp -r /deps/src/metaSMT/deps/minisat-git/lib/libminisat.so.2 lib/
cp -r /deps/src/metaSMT/deps/minisat-git/lib/libminisat.so.2.1.0 lib/

# minisat
# stp

# Crab
if [ ! -d "./bin/crabllvm" ]; then
    cp -r /deps/install/crab ./bin
    mv ./bin/crab ./bin/crabllvm
fi

echo "> Crab-LLVM replacing PATH"
sed -i '54s/\"PATH\"/\"CLANG_PATH\"/' ./bin/crabllvm/bin/crabllvm.py
cp /usr/lib/x86_64-linux-gnu/libbfd-* ./bin/crabllvm/lib/
cp /usr/lib/x86_64-linux-gnu/libgomp.so.1 ./bin/crabllvm/lib/
cp /usr/lib/x86_64-linux-gnu/libstdc++* ./bin/crabllvm/lib/
cp ./lib/libz3.so ./bin/crabllvm/lib/

../utils/cp_utils_file.sh
# #echo ""

if [ "$export_svcomp" = true ] ; then
	echo ""
	echo "Cleaning for SVCOMP"
	rm -rf ./z3/include
	rm -rf ./z3/lib/python2.7
	rm -rf ./z3/lib
	rm -rf ./lib/python2.7
	rm -rf ./lib/clang/6.0.0/include
	rm -rf ./moduleBenchExec
	rm ./bin/kleaver
	rm -rf ./bin/crabllvm/ldd

    rm ./bin/crabllvm/lib/libz3.so
	ln -s ./z3/lib/libz3.so ./bin/crabllvm/lib/libz3.so
	#rm ./lib/libz3.so
fi


#echo ""
#echo "Generating archive ..."
#if [ ! -d "map2check" ]; then
#        mkdir map2check
#else
	#rm -rf map2check
	#mkdir map2check
#fi
#cp -r release/* map2check/
#7z a map2check.zip map2check
#rm -rf map2check
#echo ""


echo ""
echo "DONE"
echo ""

