#!/usr/bin/env python
# -*- coding: latin1 -*-

from __future__ import print_function
import subprocess
import sys
import os
import commands
import ConfigParser
import platform
import tarfile
import zipfile
import shutil

VERSION = 'v1'

def extract_file(_filepathtar, _to_directory):

    if _filepathtar.endswith('.zip'):
        opener, mode = zipfile.ZipFile, 'r'
    elif _filepathtar.endswith('.tar.gz') or _filepathtar.endswith('.tgz'):
        opener, mode = tarfile.open, 'r:gz'
    elif _filepathtar.endswith('.tar.bz2') or _filepathtar.endswith('.tbz'):
        opener, mode = tarfile.open, 'r:bz2'
    else:
        raise ValueError, "Could not extract `%s` as no appropriate extractor is found" % _filepathtar


    cwd = os.getcwd()
    os.chdir(_to_directory)

    try:
        file = opener(_filepathtar, mode)
        try: file.extractall()
        finally: file.close()
    finally:
        os.chdir(cwd)


# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__":

    # Identify if is the user is root
    if not os.getenv("USER") == "root":
        print("Sorry about that!")
        print("  >> We need to be root to execute this script")
        sys.exit()
        
    print()
    print("Checking prerequisites for installing the Map2Check-FORTES ... ")
    print()
    

    #-------------------------------------------------------
    ### Checking Pycparse
    msg_pyc = ""

    cwd = os.getcwd()

    try:
        import pycparser
    except ImportError, e:
        print("--- Installing: pycparser")
        extract_file( os.path.abspath("modules/other_tools/pycparser-master.zip"),
                      os.path.abspath("modules/other_tools/") )


        os.chdir(os.path.abspath("modules/other_tools/pycparser-master/"))
        msg_pyp = commands.getoutput("python setup.py install")

        print("--- Checking installation for: pycparser")
        try:
            import pyparsing
            os.chdir(cwd)
            shutil.rmtree(os.path.abspath("modules/other_tools/pycparser-master"))
        except ImportError, e:
            print("--- Error to install pycparser")
            print("Details: ")
            print(msg_pyp)
            #print("Available at https://github.com/eliben/pycparser")
            sys.exit

    
    #-------------------------------------------------------
    ### Checking Pyparsing
    msg_pyp = ""

    cwd = os.getcwd()

    try:
        import pyparsing
    except ImportError, e:
        print("--- Installing: pyparsing")
        extract_file( os.path.abspath("modules/other_tools/pyparsing-2.0.2.zip"),
                      os.path.abspath("modules/other_tools/") )


        os.chdir(os.path.abspath("modules/other_tools/pyparsing-2.0.2/"))
        msg_pyp = commands.getoutput("python setup.py install")

        print("--- Checking installation for: pyparsing")
        try:
            import pyparsing
            os.chdir(cwd)
            shutil.rmtree(os.path.abspath("modules/other_tools/pyparsing-2.0.2"))
        except ImportError, e:
            print("--- Error to install pyparsing")
            print("Details: ")
            print(msg_pyp)
            sys.exit

    #-------------------------------------------------------
    ### Checking Perl
    try:
        # pipe output to /dev/null for silence
        null = open("/dev/null", "w")
        subprocess.Popen("perl", stdout=null, stderr=null)
        null.close()

    except OSError:
        print("Error: ", end="")
        print("Perl not found")
        print("\t Please install Perl")
        sys.exit()
        
        
    #-------------------------------------------------------
    ### Checking Ctags
    try:
        # pipe output to /dev/null for silence
        null = open("/dev/null", "w")
        subprocess.Popen("ctags", stdout=null, stderr=null)
        null.close()

    except OSError:
        print("Error: ", end="")
        print("Ctags not found")
        print("\t Please install Ctags")
        
        
    #-------------------------------------------------------
    # Unpacking ESBMC
    # Checking the OS arch
    if platform.machine() == "x86_64":
        #64bits
        extract_file( os.path.abspath("modules/bmc_tool/esbmc_tool/64/bin.tar.bz2"),
                      os.path.abspath("modules/bmc_tool/esbmc_tool/64/") )
        get_esbmc_path = os.path.abspath("modules/bmc_tool/esbmc_tool/64/bin/esbmc")
    else:
        #32bits
        extract_file( os.path.abspath("modules/bmc_tool/esbmc_tool/32/bin.tar.bz2"),
                      os.path.abspath("modules/bmc_tool/esbmc_tool/32/") )
        get_esbmc_path = os.path.abspath("modules/bmc_tool/esbmc_tool/32/bin/esbmc")


    
    # Write CFG file
    #Start to generate the config file
    config = ConfigParser.RawConfigParser()
    
    config.add_section('MAPFORTES_TOOL')        
    #config.set('MAPFORTES_TOOL', 'MAPFORTES_path', os.path.dirname(__file__))
    config.set('MAPFORTES_TOOL', 'MAPFORTES_path', os.path.abspath(''))
    config.set('MAPFORTES_TOOL', 'pyparsing', 'installed')
    config.set('MAPFORTES_TOOL', 'pycparser', 'installed')
    config.set('MAPFORTES_TOOL', 'perl', 'installed')
      
    
    config.add_section('ESBMC_TOOL')
    if get_esbmc_path == '':
        config.set('ESBMC_TOOL', 'ESBMC_path', 'empty')
    else:
        config.set('ESBMC_TOOL', 'ESBMC_path', get_esbmc_path)
        
    
    # Writing our configuration file to 'example.cfg'
    with open('settings.cfg', 'wb') as configfile:
        config.write(configfile)


    # Finished
    print("Installation finished")
    print("Futher details use: map2check-fortes.py -h")
    print()

