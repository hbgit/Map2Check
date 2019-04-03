#!/usr/bin/env python3

# Usage: ./check_code_style.sh [c|p]

import os
import argparse
import re
import subprocess

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

def apply_clang_format(style, path, extensions):
  for root, dirs, files in os.walk(path):
    for file in files:
      if file.endswith(extensions):
        root = re.escape(root)
        file = re.escape(file)
        os.system("clang-format -i -style=" + style + " " + root + "/" + file)

if version == True:
  os.system(tool_path + "--version")
  exit(0)

if c_style == True:
  print("Formatting C codes with clang-format adopting LLVM style ...")
  # File Extension filter. You can add new extension
  extensions = (".c", ".h", ".hh")
  apply_clang_format('llvm', 'modules/backend/library', extensions)

if cpp_style == True:
  print("Formatting CPP codes with clang-format adopting Google style ...")
  # File Extension filter. You can add new extension
  cpp_extensions = (".cxx",".cpp",".c", ".hxx", ".hh", ".cc", ".hpp")
  paths_to_check = ["modules/frontend", "modules/backend/pass"]
  for path in paths_to_check:
      apply_clang_format('google', path, cpp_extensions)
  print("Check issues related to code style ...")
  for path in paths_to_check:
    subprocess.run(['python', 'utils/cpplint.py', '--recursive', '--linelength=120', '--counting=detailed', path], check=True)
