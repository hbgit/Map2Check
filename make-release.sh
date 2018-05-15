mkdir build
mkdir release
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../release/
make
make install