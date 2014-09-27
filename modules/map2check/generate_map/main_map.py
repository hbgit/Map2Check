#!/usr/bin/env python
from __future__ import print_function
# -*- coding: latin1 -*-
import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))  

from create_map import ParseAstPy


__author__ = 'Herbert OLiveira Rocha'



# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__":   
    
    path_input_file=""
    option_print = 1

    #if 2 < len(sys.argv):
    path_input_file  = sys.argv[1]
    option_print = sys.argv[2]    

    test = ParseAstPy()
    # tmp - TODO: get this option by argv
    test.flag_track_all = True

    test.load2Parse(path_input_file)

    # Gathering data about all typedef
    test.searchTypedef()

    # Global mapping OKAY
    test.mapGlobalVars()

    # Function  mapping OKAY
    test.mapEachFunction()

    # Sorting data mapped by the line of the program
    test.sortListMapByNumLine()
    test.printMapResult(int(option_print))

    # Generate Overview results and print if True arg
    test.createOverviewResult(False)
