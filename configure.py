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
import pwd


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

    #save the name of the user logged
    username = os.getlogin()
    saveusernameid = pwd.getpwnam(str(username)).pw_uid
        
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
            import pycparser
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
    ### Checking nextworkx
    msg_netx = ""

    cwd = os.getcwd()

    try:
        import networkx
    except ImportError, e:
        print("--- Installing: networkx")
        extract_file( os.path.abspath("modules/other_tools/networkx-master.zip"),
                      os.path.abspath("modules/other_tools/") )


        os.chdir(os.path.abspath("modules/other_tools/networkx-master/"))
        msg_netx = commands.getoutput("python setup.py install")

        print("--- Checking installation for: networkx")
        try:
            import networkx
            os.chdir(cwd)
            shutil.rmtree(os.path.abspath("modules/other_tools/networkx-master"))
        except ImportError, e:
            print("--- Error to install networkx")
            print("Details: ")
            print(msg_netx)
            sys.exit


    #-------------------------------------------------------
    ### Unpacking tokenizer
    msg_tk = ""

    cwd = os.getcwd()

    print("--- Unpacking: tokenizer")
    desttok = "modules/tokenizer/"

    if not os.path.exists(desttok):
        os.makedirs(desttok)
        os.chown(desttok, saveusernameid, saveusernameid)

    extract_file( os.path.abspath("modules/other_tools/c-tokenizer-x86_64-linux.zip"),
                  os.path.abspath(desttok) )


    #-------------------------------------------------------
    ### Generating Uncrustify
    msg_pyp = ""

    cwd = os.getcwd()

    print("--- Compiling: uncrustify")
    extract_file( os.path.abspath("modules/other_tools/uncrustify-0.60.tar.gz"),
                  os.path.abspath("modules/other_tools/") )


    #cd dir
    os.chdir(os.path.abspath("modules/other_tools/uncrustify-0.60/"))

    msg_conf_unc = commands.getoutput("./configure")
    #Checking configure
    if not os.path.isfile("Makefile"):
        print("Error: Makefile file was not generated. See LOG: ")
        print(msg_conf_unc)
        sys.exit()

    msg_make_unc = commands.getoutput("make")
    #Checking configure
    if not os.path.isfile("src/uncrustify"):
        print("Error: uncrustity tool could not be generated. See LOG: ")
        print(msg_make_unc)
        sys.exit()

    #Backing to original path
    os.chdir(cwd)

    print("--- Checking installation for: uncrustify")
    #Copy tool to correct place
    # Checking the OS arch
    if platform.machine() == "x86_64":
        dest = os.path.abspath("modules/preprocessor/primary_preprocessing/arch_64")
        if not os.path.exists(dest):
            os.makedirs(dest)
            os.chown(dest, saveusernameid, saveusernameid)
        #64bits
        try:
            shutil.copy2(os.path.abspath("modules/other_tools/uncrustify-0.60/src/uncrustify"),
                        dest )
        except shutil.Error as e:
            print('Error: %s' % e)
            raise
            # eg. source or destination doesn't exist
        except IOError as e:
            print('Error: %s' % e.strerror)
            raise
    else:
        #32bits
        dest = os.path.abspath("modules/preprocessor/primary_preprocessing/arch_32")
        if not os.path.exists(dest):
            os.makedirs(dest)
            os.chown(dest, saveusernameid, saveusernameid)
        try:
            shutil.copy2(os.path.abspath("modules/other_tools/uncrustify-0.60/src/uncrustify"),
                        dest )
        except shutil.Error as e:
            print('Error: %s' % e)
            raise
            # eg. source or destination doesn't exist
        except IOError as e:
            print('Error: %s' % e.strerror)
            raise

    shutil.rmtree(os.path.abspath("modules/other_tools/uncrustify-0.60"))



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
    config.set('MAPFORTES_TOOL', 'networkx', 'installed')
    config.set('MAPFORTES_TOOL', 'uncrustify', 'installed')
    config.set('MAPFORTES_TOOL', 'tokenizer', 'installed')
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
    #print("Futher details use: map2check-fortes.py -h")
    print()
    os.system("./map2check-fortes.py -h")

