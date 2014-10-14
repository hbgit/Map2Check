#!/usr/bin/env python
# -*- coding: latin1 -*-

from __future__ import print_function

import sys
import os
import commands
import re


# From PycParser
import pycparser.c_parser
import pycparser.c_ast
from pycparser.c_ast import *
import pycparser.c_generator


# -------------------------------------------------
# Global Variables
# Portable cpp path for Windows and Linux/Unix
CPPPATH = '../map2check/utils/cpp.exe' if sys.platform == 'win32' else 'cpp'
ABS_PATH = os.path.dirname(__file__)
# -------------------------------------------------


# A visitor with some state information (the funcname it's
# looking for)
#
class PointCallMainVisitor(NodeVisitor):
    def __init__(self):
        self.filepathcsv2write = ''
        self.listlinenumresult = []


    def searchendmain(self, node):
        nodetype = type(node)
        if(nodetype in [If,While,For,DoWhile,Switch,Case]):
            if nodetype == If:
                self.searchendmain(node.iftrue.block_items[-1])
                if node.iffalse:
                    self.searchendmain(node.iffalse.block_items[-1])
            else:
                self.searchendmain(node.stmt.block_items[-1])
        else:
            self.listlinenumresult.append(node.coord)


    def visit_FuncDef(self, node):
        #print(node.decl.name,node.coord)
        if node.decl.name == "main":
            # search by return
            #print(node.body.block_items)
            for item in node.body.block_items:
                if type(item) == Return:
                    #print("Here")
                    self.listlinenumresult.append(item.expr.coord)

            if(type(node.body.block_items[-1]) in [If,While,For,DoWhile,Switch,Case]):
                self.searchendmain(node.body.block_items[-1])
            else:
                self.listlinenumresult.append(node.body.block_items[-1].coord)



class PointCallOtherFuncVisitor(NodeVisitor):
    def __init__(self):
        self.filepathcsv2write = ''
        self.funcsname2point = ['abort','exit']
        self.listlinenumresult = []


    def visit_FuncCall(self, node):
        if node.name.name in self.funcsname2point:
            #print('called at %s' % ( node.name.coord))
            self.listlinenumresult.append(node.name.coord)


class IdentifyLeakPoints(object):
    def __init__(self, _cfilepath):
        self.cfilepath = _cfilepath


    @staticmethod
    def getNumberOfLine(nodecoord):
        #print(nodeVar)
        txt = str(nodecoord)
        matchNumLine = re.search(r'(.[^:]+)$', txt)
        if matchNumLine:
            onlyNumber = matchNumLine.group(1).replace(":","")
            return onlyNumber


    def identify_points(self):

        path_cpp_args = os.path.join(os.path.dirname(__file__), "../map2check/utils/fake_libc_include")
        ast = pycparser.parse_file(self.cfilepath, use_cpp=True, cpp_path=CPPPATH, cpp_args=r'-I'+path_cpp_args)
        #ast.show()
        #sys.exit()

        # Write in a tmp file the lines to test a possible leak
        tmpfileleakpoints = open("/tmp/tmp_leakpoints.map2check",'w')

        v = PointCallOtherFuncVisitor()
        v.visit(ast)
        for i in v.listlinenumresult:
            #print(self.getNumberOfLine(i))
            tmpfileleakpoints.write(self.getNumberOfLine(i)+"\n")

        v = PointCallMainVisitor()
        v.visit(ast)
        for i in v.listlinenumresult:
            tmpfileleakpoints.write(self.getNumberOfLine(i)+"\n")

        tmpfileleakpoints.close()


if __name__ == "__main__":
    if len(sys.argv) > 1:
        filename = sys.argv[1]
        #func = sys.argv[2]
        runind = IdentifyLeakPoints(filename)
        runind.identify_points()
