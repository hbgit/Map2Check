#!/usr/bin/env python3

import os
import argparse
import re

# Options
parser = argparse.ArgumentParser()
parser.add_argument("-v", "--version",
                    help="Prints script's version", action='store_true')
parser.add_argument("ceoutput", nargs='?',
                    help="Path to the Map2Check counterexample output")

args = parser.parse_args()

# for options
version = args.version
ceoutput = args.ceoutput

if version == True:
    print("1.0v")
    exit(0)

if ceoutput is None:
    print("Please, specify a Map2Check counterexample output")
    exit(1)


f = open(ceoutput, 'r')
ceoutput_file_content = f.readlines()

listofblocks = []
flag_blk = False

for num, line in enumerate(ceoutput_file_content, start=1):
    if ">>Memory list log" in line:
        flag_blk = True

    if flag_blk and line.strip():
        div_blk = re.search("^-", line)
        if div_blk:
            flag_blk = False
        else:
            line = line.strip()
            split = line.split(":")
            if len(split) > 1:
                if not split[1]:
                    print("[ERROR-CE] in line number < " +
                          str(num) + " > of counterexample")
