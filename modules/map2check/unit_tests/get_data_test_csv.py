#!/usr/bin/env python
from __future__ import print_function
# -*- coding: latin1 -*-

__author__ = 'Herbert OLiveira Rocha'

# Python
import csv
from collections import defaultdict


class CsvReader(object):

    def __init__(self):
        self.columns_csv = defaultdict(list) #we want a list to append each value in each column to
        self.pathCsvFile = "test_cases/oracle_test.csv"
        self.loadDataTestCases()

        # For Unit Test
        self.list_name_program =[]
        self.list_utest_total_num_malloc = []
        self.list_utest_total_num_free = []
        self.list_utest_total_num_global_var = []
        self.list_utest_total_num_obj_mapped = []
        self.list_utest_lines_mapped = []
        self.list_utest_comments = []


    def loadDataTestCases(self):
        with open(self.pathCsvFile) as file:
            reader = csv.DictReader(file,delimiter=';')
            for row in reader:
                for (k,v) in row.items():
                    self.columns_csv[k].append(v)


    def getNameFile(self):
        for col in self.columns_csv['Name']:
            self.list_name_program.append(col)
        return self.list_name_program


    def getTotalNumMalloc(self):
        for col in self.columns_csv['Num_Malloc']:
            self.list_utest_total_num_malloc.append(col)
        return self.list_utest_total_num_malloc


    def getTotalNumFree(self):
        for col in self.columns_csv['Num_Free']:
            self.list_utest_total_num_free.append(col)
        return self.list_utest_total_num_free


    def getTotalNumGlobalVar(self):
        for col in self.columns_csv['Num_Global_Var']:
            self.list_utest_total_num_global_var.append(col)
        return self.list_utest_total_num_global_var


    def getTotalNumObjMapped(self):
        for col in self.columns_csv['Total_Num_Obj_Mapped']:
            self.list_utest_total_num_obj_mapped.append(col)
        return self.list_utest_total_num_obj_mapped


    def getAllLinesMapped(self):
        for col in self.columns_csv['Lines_Mapped']:
            tmp_list = map(int,col.rsplit(','))
            self.list_utest_lines_mapped.append(tmp_list)
        return self.list_utest_lines_mapped


    def getComments(self):
        for col in self.columns_csv['Comments']:
            self.list_utest_comments.append(col)
        return self.list_utest_comments


# if __name__ == "__main__":
#     a = CsvReader()
#     print(a.getNameFile())
