#!/usr/bin/env python3

# Usage: ./check_code_style.sh [c|p]

import os
import argparse
import re

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
  print("Formatting C codes with clang-format adopting LLVM style ...")

  # File Extension filter. You can add new extension
  cpp_extensions = (".c", ".h", ".hh")

  # Apply the clang formatting
  # Please note "-style" is for standard style options
  # and "-i" is in-place editing
  for root, dirs, files in os.walk("modules/backend"):
      for file in files:
          if file.endswith(cpp_extensions):
              root = re.escape(root)
              file = re.escape(file)
              # print(root + "/" + file)
              os.system("clang-format -i -style=google " + root + "/" + file)
  exit(0)

if cpp_style == True:
  print("Formatting C++ codes with clang-format adopting Google Style ...")

  # File Extension filter. You can add new extension
  cpp_extensions = (".cxx",".cpp",".c", ".hxx", ".hh", ".cc", ".hpp")

  # Apply the clang formatting
  # Please note "-style" is for standard style options
  # and "-i" is in-place editing
  for root, dirs, files in os.walk("modules/frontend"):
      for file in files:
          if file.endswith(cpp_extensions):
              root = re.escape(root)
              file = re.escape(file)
              # print(root + "/" + file)
              os.system("clang-format -i -style=google " + root + "/" + file)

  print("Check issues related to code style ...")
  for root, dirs, files in os.walk("modules/frontend"):
      for file in files:
          if file.endswith(cpp_extensions):
              #print()
              root = re.escape(root)
              file = re.escape(file)
              os.system("python utils/cpplint.py --headers=hpp,h --linelength=120 --counting=detailed " + root + "/" + file )

  exit(0)
