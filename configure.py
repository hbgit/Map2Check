#!/usr/bin/env python
# -*- coding: latin1 -*-

from __future__ import print_function
import subprocess
import sys
import os
import ConfigParser

VERSION = 'v1'

# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__":
        
    print()
    print("Checking prerequisites for installing the Map2Check-FORTES ... ")
    print()
    
    count_status_requisites = 0
    
    #-------------------------------------------------------
    ### Checking Pycparse
    try:
        import pycparser
        count_status_requisites += 1
    except ImportError, e:
        print("Error: ", end="")
        print("Python module {pycparser} not found")
        print("Please install {pycparser} to run Map2Check-FORTES")
        print("Available at https://github.com/eliben/pycparser")
        sys.exit()
        
    
    #-------------------------------------------------------
    ### Checking Pyparsing
    try:
        import pyparsing
        count_status_requisites += 1
    except ImportError, e:
        print("Error: ", end="")
        print("Python module {pyparsing} not found")
        print("Please install {pyparsing} to run Map2Check-FORTES")
        print("Available at http://pyparsing.wikispaces.com/")
        sys.exit()
 
    
    #-------------------------------------------------------
    ### Checking Perl
    try:
        # pipe output to /dev/null for silence
        null = open("/dev/null", "w")
        subprocess.Popen("perl", stdout=null, stderr=null)
        null.close()
        count_status_requisites += 1
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
        count_status_requisites += 1
    except OSError:
        print("Error: ", end="")
        print("Ctags not found")
        print("\t Please install Ctags")
        
        
    #-------------------------------------------------------
    ### Checking ESBMC
    get_esbmc_path = ''
    try:
        # pipe output to /dev/null for silence
        null = open("/dev/null", "w")
        subprocess.Popen("esbmc", stdout=null, stderr=null)
        null.close()
        get_esbmc_path = commands.getoutput("wich esbmc")
        count_status_requisites += 1
    except OSError:
        print("Warnning: ", end="")
        print("ESBMC not found")
        print("  >> Please install ESBMC")
        print("  >> In case we already have the ESBMC installed, please manually set the ESBMC path in the [ESBMC_path] of the { settings.cfg } file")
        
        
    
    # Write CFG file
    #Start to generate the config file
    config = ConfigParser.RawConfigParser()
    
    config.add_section('MAPFORTES_TOOL')        
    config.set('MAPFORTES_TOOL', 'MAPFORTES_path', os.path.dirname(__file__))
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
        
    print()
    
    
    
