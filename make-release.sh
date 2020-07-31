#!/bin/sh
export LLVM_VERSION=8.0.0

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

export LLVM_DIR=/usr/lib/llvm-8/lib/cmake/llvm/
export CXX=/usr/bin/clang++-8
export CC=/usr/bin/clang-8
cmake .. -G Ninja -DLLVM_DIR=$LLVM_DIR -DCMAKE_INSTALL_PREFIX=../release/ 

ninja
ninja install

cd ../release   

mkdir -p ./include

cp -r /usr/include/clang/8/include/* ./include
mkdir -p ./lib/clang
cp -r /usr/lib/llvm-8/lib/clang/8.0.1 ./lib/clang/

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
cp /lib/x86_64-linux-gnu/ld-2.27.so ./lib/

# Supporting LLVM 8
cp /usr/lib/x86_64-linux-gnu/libLLVM-8.so.1 ./lib/
cp /usr/lib/x86_64-linux-gnu/libffi.so.6.0.4 ./lib/libffi.so.6
cp /lib/x86_64-linux-gnu/libtinfo.so.5.9 ./lib/libtinfo.so.5

echo ""
echo "Copying external tools"
# LibFuzzer
cp /usr/lib/llvm-8/lib/libFuzzer.a ./lib

# Z3
if [ ! -d "./z3" ]; then
    cp -r /deps/install/z3 .
    cp -r z3/lib/* lib/
fi

# Klee and klee_uclib 
cp -r /deps/install/klee/bin/* ./bin/
cp -r /deps/install/klee/include/* ./include/
cp -r /deps/install/klee/lib/* ./lib/


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
# minisat
cp -r /deps/src/metaSMT/deps/minisat-git/lib/libminisat.a lib/
cp -r /deps/src/metaSMT/deps/minisat-git/lib/libminisat.so lib/
cp -r /deps/src/metaSMT/deps/minisat-git/lib/libminisat.so.2 lib/
cp -r /deps/src/metaSMT/deps/minisat-git/lib/libminisat.so.2.1.0 lib/

# Crab
if [ ! -d "./bin/crabllvm" ]; then
    cp -r /deps/install/crab ./bin/
    mv ./bin/crab ./bin/crabllvm
fi

echo "> Crab-LLVM replacing PATH"
sed -i '54s/\"PATH\"/\"CLANG_PATH\"/' ./bin/crabllvm/bin/clam.py
cp /usr/lib/x86_64-linux-gnu/libbfd-* ./bin/crabllvm/lib/
cp /usr/lib/x86_64-linux-gnu/libgomp.so.1 ./bin/crabllvm/lib/
cp /usr/lib/x86_64-linux-gnu/libstdc++* ./bin/crabllvm/lib/
# cp ./lib/libz3.so ./bin/crabllvm/lib/


../utils/cp_utils_file.sh


if [ "$export_svcomp" = true ] ; then
	echo ""
	echo "Cleaning for SVCOMP"
	rm -rf ./z3/include
	#rm -rf ./z3/lib/python2.7
	rm -rf ./z3/lib
	#rm -rf ./lib/python2.7
	#rm -rf ./lib/clang/6.0.0/include
	rm -rf ./moduleBenchExec
	rm ./bin/kleaver
	rm -rf ./bin/crabllvm/ldd

    rm ./bin/crabllvm/lib/libz3.so
	cd bin/crabllvm/lib/
	ln -s ../../../lib/libz3.so .
	cd ../../../ # go back to release	
fi





echo ""
echo "DONE"
echo ""

