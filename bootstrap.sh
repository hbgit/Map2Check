#!/bin/bash

#set -x

RUNDIR=`pwd`
PREFIX=`pwd`/release

map2check_get_deps() 
{
    cd $RUNDIR
    if [ ! -d dependencies ]; then	
		mkdir dependencies    
    fi
    cd dependencies
   
	# Check if LLVM is already downloaded
	# TODO: Check for distro before downloading
	if [ ! -d clang ]; then	
		echo "Clang not found, downloading it"		
		curl http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz | tar -xJ
		mv clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04 clang
    fi
    cd -
}

cp_utils_file() 
{
	cd $RUNDIR
	
	# Copying Map2Check license
	if [ ! -f $PREFIX/LICENSE ]; then			
		cp LICENSE $PREFIX/
	else
		rm $PREFIX/LICENSE		
		cp LICENSE $PREFIX/
    fi
	
	# Copying licenses tools
	if [ ! -d $PREFIX/LICENSES ]; then	
		mkdir $PREFIX/LICENSES
		cp utils/*.TXT $PREFIX/LICENSES/
	else
		rm -rf $PREFIX/LICENSES
		mkdir $PREFIX/LICENSES
		cp utils/*.TXT $PREFIX/LICENSES/
    fi
    
    # Wrapper script
    if [ ! -f $PREFIX/map2check-wrapper.py ]; then	
		cp utils/map2check-wrapper.py $PREFIX/
	else
		rm $PREFIX/map2check-wrapper.py
		cp utils/map2check-wrapper.py $PREFIX/
    fi
    
    # Samples to test map2check
    if [ ! -d $PREFIX/sample ]; then	
		mkdir $PREFIX/sample
		cp -r sample/* $PREFIX/sample/
	else
		rm -rf $PREFIX/sample
		mkdir $PREFIX/sample
		cp -r sample/* $PREFIX/sample/
    fi
    
    # tool-info benchexec
    if [ ! -d $PREFIX/moduleBenchExec ]; then	
		mkdir $PREFIX/moduleBenchExec/
		cp -r utils/moduleBenchExec/* $PREFIX/moduleBenchExec/
	else
		rm -rf $PREFIX/moduleBenchExec
		mkdir $PREFIX/moduleBenchExec/
		cp -r utils/moduleBenchExec/* $PREFIX/moduleBenchExec/
	fi    
    
    # README FILE
    if [ ! -f $PREFIX/README.md ]; then	
		cp README.md $PREFIX/
	else
		rm $PREFIX/README.md
		cp README.md $PREFIX/
	fi
	
	
}

map2check_get_deps
