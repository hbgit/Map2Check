#!/usr/bin/env python
from __future__ import print_function
# -*- coding: latin1 -*-

__author__ = 'Herbert OLiveira Rocha'


# Python
import unittest
import sys
from termcolor import colored

sys.path.append("../")


from generate_map import create_map
from unit_tests import get_data_test_csv


class OverviewTest(unittest.TestCase):

    def setUp(self):
        self.verificationErrors = []

        # Data from map execution
        self.toTest_TotalNumObjMapped = []
        self.toTest_TotalNumMalloc = []
        self.toTest_TotalNumFree = []
        self.toTest_TotalNumGlobalVar = []
        self.toTest_AllLinesMapped = []

        # Data gather from oracle test file
        self.dataOracle = get_data_test_csv.CsvReader()
        self.oracle_list_programs = self.dataOracle.getNameFile()
        self.oracle_totalNumObjMapped = self.dataOracle.getTotalNumObjMapped()
        self.oracle_totalNumMalloc = self.dataOracle.getTotalNumMalloc()
        self.oracle_totalNumFree = self.dataOracle.getTotalNumFree()
        self.oracle_totalNumGlobalVar = self.dataOracle.getTotalNumGlobalVar()
        self.oracle_allLinesMapped = self.dataOracle.getAllLinesMapped()
        self.oracle_comments = self.dataOracle.getComments()

        # In this part we aims to generate the test cases, as following:
        # [[test,oracle],[test,oracle]....] <- for each value to be tested
        self.list_test_cases = []
        self.list_test_cases.append([self.oracle_list_programs,[]])
        self.list_test_cases.append([self.oracle_totalNumObjMapped,[]])
        self.list_test_cases.append([self.oracle_totalNumMalloc,[]])
        self.list_test_cases.append([self.oracle_totalNumFree,[]])
        self.list_test_cases.append([self.oracle_totalNumGlobalVar,[]])
        self.list_test_cases.append([self.oracle_allLinesMapped,[]])
        self.list_test_cases.append([self.oracle_comments,[]])


        #for i in self.list_test_cases:
        #    print(i)
        #print("-------------------------------")
        #print()


        # Running the parse for each program and them save each result
        # in a list. The tests are based in compare the list
        # self.toTest_* (come from parse) to self.oracle_* (come from oracle csv)
        for file in self.oracle_list_programs:
            # The parse
            self.parse = create_map.ParseAstPy()

            pathTest = "test_cases/"+file

            # Run parse
            self.parse.load2Parse(pathTest)
            self.parse.searchTypedef()
            self.parse.mapGlobalVars()
            self.parse.mapEachFunction()
            self.parse.sortListMapByNumLine()
            self.parse.createOverviewResult(False)

            # Gathering the data from execution map
            # Add the exexcution data in the test case list
            self.list_test_cases[1][1].append(self.parse.getUTestTotalNumObjMapped())
            self.list_test_cases[2][1].append(self.parse.getUTestTotalNumMalloc())
            self.list_test_cases[3][1].append(self.parse.getUTestTotalNumFree())
            self.list_test_cases[4][1].append(self.parse.getUTestTotalNumGlobalVar())
            self.list_test_cases[5][1].append(self.parse.getUTestLinesMapped())


    def test_TotalNumMalloc(self):
        count_test = 0
        while count_test < len(self.list_test_cases[2][0]):
            msg = "\n\n Program:  "+colored(self.list_test_cases[0][0][count_test],'red',attrs=['bold'])+"\n \t Expected value: "+str(self.list_test_cases[2][0][count_test])+\
                  "\n \t Actual value  : "+str(self.list_test_cases[2][1][count_test])+\
                  "\n \t Comments      : "+str(self.list_test_cases[6][0][count_test])
            self.assertEqual(int(self.list_test_cases[2][0][count_test]),int(self.list_test_cases[2][1][count_test]),msg)
            count_test += 1


    def test_TotalNumFree(self):
        count_test = 0
        while count_test < len(self.list_test_cases[3][0]):
            msg = "\n\n Program:  "+colored(self.list_test_cases[0][0][count_test],'red',attrs=['bold'])+"\n \t Expected value: "+str(self.list_test_cases[3][0][count_test])+\
                  "\n \t Actual value  : "+str(self.list_test_cases[3][1][count_test])+\
                  "\n \t Comments      : "+str(self.list_test_cases[6][0][count_test])
            self.assertEqual(int(self.list_test_cases[3][0][count_test]),int(self.list_test_cases[3][1][count_test]),msg)
            count_test += 1


    def test_TotalNumObjMapped(self):
        count_test = 0
        while count_test < len(self.list_test_cases[1][0]):
            msg = "\n\n Program:  "+colored(self.list_test_cases[0][0][count_test],'red',attrs=['bold'])+"\n \t Expected value: "+str(self.list_test_cases[1][0][count_test])+\
                  "\n \t Actual value  : "+str(self.list_test_cases[1][1][count_test])+\
                  "\n \t Comments      : "+str(self.list_test_cases[6][0][count_test])
            self.assertEqual(int(self.list_test_cases[1][0][count_test]),int(self.list_test_cases[1][1][count_test]),msg)
            count_test += 1


    def test_AllLinesMapped(self):
        count_test = 0
        while count_test < len(self.list_test_cases[5][0]):
            msg = "\n\n Program:  "+colored(self.list_test_cases[0][0][count_test],'red',attrs=['bold'])+"\n \t Expected value: "+str(self.list_test_cases[5][0][count_test])+\
                  "\n \t Actual value  : "+str(self.list_test_cases[5][1][count_test])+\
                  "\n \t Comments      : "+str(self.list_test_cases[6][0][count_test])
            self.assertListEqual(self.list_test_cases[5][0][count_test],self.list_test_cases[5][1][count_test],msg)
            #try:
            #except AssertionError as e:
            #    self.verificationErrors.append(e)
            count_test += 1








# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__":
    unittest.main()
