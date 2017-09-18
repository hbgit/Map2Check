#!/bin/bash

set -x

RUNDIR=`pwd`
PREFIX=`pwd`/release

map2check() 
{
    cd $RUNDIR
    if [ ! -d build ]; then	
		mkdir build
	else
		rm -rf build
		mkdir build
    fi
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=../release/ -DCMAKE_BUILD_TYPE=Release
    make all
    make test
    make install
    # make release  
    cd - 
    cp_utils_file
}

cp_utils_file() 
{
	cd $RUNDIR
	# Copying licenses
	if [ ! -d $PREFIX/LICENSES ]; then	
		mkdir $PREFIX/LICENSES
		cp utils/*.TXT $PREFIX/LICENSES/		
    fi
    
    # Wrapper script
    if [ ! -f $PREFIX/map2check-wrapper.py ]; then	
		cp utils/map2check-wrapper.py $PREFIX/
    fi
    
    # Samples to test map2check
    if [ ! -d $PREFIX/sample ]; then	
		mkdir $PREFIX/sample
		cp -r sample/* $PREFIX/sample/
    fi
    
    # README FILE
    if [ ! -f $PREFIX/README.md ]; then	
		cp README.md $PREFIX/
	fi
}

gtest() 
{
	tgest=$(locate -c libgtest)
	if [ ! ${tgest} -gt 0  ]; then
		sudo cd /usr/src/gtest
		sudo cmake CMakeLists.txt
		sudo make

		# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
		sudo cp *.a /usr/lib
	
		cd -
	fi
}

gtest
map2check
