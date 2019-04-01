#!/usr/bin/env python3

# Usage: ./check_code_style.sh [C|CPP]

import os
import argparse

# Options
parser = argparse.ArgumentParser()
parser.add_argument("-v", "--version", help="Prints script's version", action='store_true')
parser.add_argument("-c", "--checkCodeStyle", help="Checks C code in LLVM style", action='store_true')
parser.add_argument("-p", "--checkCppCodeStyle", help="Checks C++ code Google style", action='store_true')

args = parser.parse_args()

# for options
version = args.version
c_style = args.checkCodeStyle
cpp_style = args.checkCppCodeStyle

if version == True:
  os.system(tool_path + "--version")
  exit(0)

if c_style == True:
  print("C")
  exit(0)

if cpp_style == True:
  print("Formatting C++ codes with clang-format ...")

  # File Extension filter. You can add new extension
  cpp_extensions = (".cxx",".cpp",".c", ".hxx", ".hh", ".cc", ".hpp")

  # Apply the clang formatting
  # Please note "-style" is for standard style options
  # and "-i" is in-place editing
  for root, dirs, files in os.walk("modules/frontend"):
      for file in files:
          if file.endswith(cpp_extensions):
              print(root + "/" + file)
              #os.system("clang-format -i -style=google " + root + "/" + file)

  print("Check issues related to code style ...")
  for root, dirs, files in os.walk("modules/frontend"):
      for file in files:
          if file.endswith(cpp_extensions):
              #print()
              os.system("python utils/cpplint.py --headers=hpp,h --linelength=120 --counting=detailed " + root + "/" + file )

  exit(0)



#if [ "$1" == "CPP" ]; then
#  python
#else
#  clang-format -i -style=.clang-format-llvm src/locuspocus.c
#fi
