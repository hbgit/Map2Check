#!/usr/bin/env python
from __future__ import print_function
# -*- coding: latin1 -*-
from collections import defaultdict

__author__ = 'Herbert OLiveira Rocha'


# Python Imports
import sys
import csv


class ReaderCsv(object):
    """
    This Class aims to read a CSV file and the create a list for each column identify in the file.
    """

    def __init__(self):
        self.flag_csv_reader = False
        self.columns = defaultdict(list) #we want a list to append each value in each column to


    def loadCsvFile(self, pathCsvFile, _opdelimiter):
        """
        Read the csv file and split the csv column using the delimiter ; where each
        csv column is added in separeted list.

        :param pathCsvFile: path of the csv file
        :type pathCsvFile: str
        """

        with open(pathCsvFile) as file:
            reader = csv.DictReader(file,delimiter=_opdelimiter)
            for row in reader:
                for (k,v) in row.items():
                    self.columns[k].append(v)

        return self.columns

    def load_csv_list(self, _listcsvdata, _opdelimiter):
        """
        Read the csv file and split the csv column using the delimiter ; where each
        csv column is added in separeted list.

        :param pathCsvFile: path of the csv file
        :type pathCsvFile: str
        """
        reader = csv.DictReader(_listcsvdata,delimiter=_opdelimiter)
        for row in reader:
            for (k,v) in row.items():
                #print(k.strip())
                #sys.exit()
                self.columns[k.strip()].append(v.strip())

        return self.columns


    def getCsvColummns(self):
        """
        Get all columns from a csv file where now each column were saved in a list.

        :return: a list of lists where each list is a column from the csv file.
        """
        return self.columns
