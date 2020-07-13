#!/bin/bash

PREFIX="."

cp_utils_file() 
{
	# Copying Map2Check license
	# Licenses from metaSMT
	# from:
	# /deps/src/metaSMT/deps/build/boolector-2.2.0-with-lingeling-bal/boolector-2.2.0-8209e6e-151129/COPYING 
	# /deps/src/metaSMT/deps/build/lingeling-ayv-86bf266-140429/license.txt
	# /deps/src/metaSMT/deps/build/yices-2.5.1/LICENSE
	echo "> Map2Check license"
	if [ ! -f $PREFIX/LICENSE ]; then
			cp ../utils/LICENSE.TXT $PREFIX/
	else
			rm $PREFIX/LICENSE
			cp ../utils/LICENSE.TXT $PREFIX/
	fi

	echo "> Install CSeq v1.9"
	if [ ! -f $PREFIX/bin/cseq-1.9 ]; then
			cp -r ../utils/cseq-1.9 $PREFIX/bin/
	else
			rm -r $PREFIX/bin/cseq-1.9
			cp -r ../utils/cseq-1.9 $PREFIX/bin/
	fi

	# Copying licenses tools
	echo "> Extra tools licenses"
	if [ ! -d $PREFIX/LICENSES ]; then      
        	mkdir $PREFIX/LICENSES
        	cp ../utils/*.TXT $PREFIX/LICENSES/
	else
        	rm -rf $PREFIX/LICENSES
                mkdir $PREFIX/LICENSES
                cp ../utils/*.TXT $PREFIX/LICENSES/
	fi
    
	# Wrapper script
	echo "> Wrapper script"
	if [ ! -f $PREFIX/map2check-wrapper.py ]; then     
        	cp ../utils/map2check-wrapper.py $PREFIX/
	else
                rm $PREFIX/map2check-wrapper.py
                cp ../utils/map2check-wrapper.py $PREFIX/
	fi
 
	# tool-info benchexec
	echo "> Module tool-info benchexec"
	if [ ! -d $PREFIX/moduleBenchExec ]; then  
               mkdir $PREFIX/moduleBenchExec/
               cp -r ../utils/moduleBenchExec/* $PREFIX/moduleBenchExec/
        else
               rm -rf $PREFIX/moduleBenchExec
               mkdir $PREFIX/moduleBenchExec/
               cp -r ../utils/moduleBenchExec/* $PREFIX/moduleBenchExec/
        fi    
    
	# README FILE
	echo "> README file"
	if [ ! -f $PREFIX/README.md ]; then        
               cp ../README.md $PREFIX/
        else
               rm $PREFIX/README.md
               cp ../README.md $PREFIX/
        fi


}

cp_utils_file
