#!/bin/bash

set -x

RUNDIR=`pwd`

map2check() {
    cd $RUNDIR
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=../release/
    make all
    make test
    make install
    # make all
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
