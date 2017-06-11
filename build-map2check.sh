#!/bin/bash

set -x

RUNDIR=`pwd`

map2check() {
    cd $RUNDIR
    if [ ! -d build ]; then	
	mkdir build
    fi
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=../release/ -DCMAKE_BUILD_TYPE=Release
    make all
    make test
    make install
    # make release
}

gtest() {
    # gtestfile="/usr/lib/libgtest.a"
    if [ ! -f "$gtestfile" ]
    then
    	cd /usr/src/gtest
	cmake CMakeLists.txt
	make

	# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
	cp *.a /usr/lib

	cd -
   fi
}

# gtest
map2check
