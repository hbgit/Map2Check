#!/usr/bin/env python
from __future__ import print_function
# -*- coding: latin1 -*-

__author__ = 'Herbert OLiveira Rocha'

#Python
import sys
import os
import re
#import commands

# From PycParser
import pycparser.c_parser
import pycparser.c_ast
from pycparser.c_ast import *
import pycparser.c_generator


# -------------------------------------------------
# Global Variables
# Portable cpp path for Windows and Linux/Unix
CPPPATH = '../map2check/utils/cpp.exe' if sys.platform == 'win32' else 'cpp'
# -------------------------------------------------


class SearchFuncts(NodeVisitor):

    def __init__(self, _namefunct):
        self.namefunct = _namefunct
        self.resultsearch = False


    def visit_FuncDef(self, node):
        if node.decl.name == self.namefunct:
            self.resultsearch = True



class VisitFuncts(NodeVisitor):
    def __init__(self):
        self.actualfunctname = ''
        self.listresult = []


    @staticmethod
    def getNumberOfLine(nodecoord):
        #print(nodeVar)
        txt = str(nodecoord)
        matchNumLine = re.search(r'(.[^:]+)$', txt)
        if matchNumLine:
            onlyNumber = matchNumLine.group(1).replace(":","")
            return onlyNumber


    def visit_FuncDef(self, node):
        #print(type(node))
        #print('===================> %s at %s' % (node.decl.name, node.decl.coord))
        self.listresult.append([node.decl.name, self.getNumberOfLine(node.decl.coord)])



class GetDataFuncts(object):

    def __init__(self):
        self.actualfunctname = ''
        self.ast = ''



    def generate_ast(self, _cfilepath):
        path_cpp_args = os.path.join(os.path.dirname(__file__), "../map2check/utils/fake_libc_include")
        self.ast = pycparser.parse_file(_cfilepath, use_cpp=True, cpp_path=CPPPATH, cpp_args=r'-I'+path_cpp_args)
        #ast.show()
        #sys.exit()


    def getlasnumline(self, _startnumlinefunct, _cfilepath):
        index2search = _startnumlinefunct - 1
        cfile = open(_cfilepath,'r')
        linesfile = cfile.readlines()
        cfile.close()

        for index in range(index2search, len(linesfile)):
            matchendfct = re.search(r'^}$',linesfile[index])
            if index > index2search and matchendfct:
                return index





    def overview_data_functs(self, _cfilepath, _pathwriteresult):
        visitrun = VisitFuncts()
        visitrun.visit(self.ast)

        fileresult = ''
        flagwrite = False
        if not _pathwriteresult == None:
            fileresult = open(_pathwriteresult,'w')
            flagwrite = True

        for index, result in enumerate(visitrun.listresult):
            #print(result[0])
            lastlinefct = self.getlasnumline(int(result[1]), _cfilepath)
            if flagwrite:
                fileresult.write(result[0]+";" + str(int(result[1])-1) + ";" + str(lastlinefct) + "\n")
            else:
                print(result[0]+";" + str(int(result[1])-1) + ";" + str(lastlinefct))


    def hasfunction(self, _namefunct):
        visitrun = SearchFuncts(_namefunct)
        visitrun.visit(self.ast)
        if visitrun.resultsearch:
            return True





# Only for tests
if __name__ == "__main__":
    if len(sys.argv) > 1:
        filename = sys.argv[1]
        #func = sys.argv[2]
        runind = GetDataFuncts()
        runind.generate_ast(filename)
        #runind.overview_data_functs(filename, "test.txt")
        print(runind.hasfunction("zzzz"))



