mkdir build
mkdir release
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../release/
make
make install
cd ..
mkdir -p ./release/bin
cp ./dependencies/clang/bin/clang ./release/bin/clang
cp ./dependencies/clang/bin/opt ./release/bin/opt
cp ./dependencies/clang/bin/llvm-link ./release/bin/llvm-link
