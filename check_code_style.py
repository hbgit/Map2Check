#!/usr/bin/env python3

# Usage: ./check_code_style.sh [c|p]

import os
import argparse
import re
import subprocess

# Options
parser = argparse.ArgumentParser()
parser.add_argument("-v", "--version",
                    help="Prints script's version", action='store_true')
parser.add_argument("-c", "--checkCodeStyle",
                    help="Checks C code in LLVM style", action='store_true')
parser.add_argument("-p", "--checkCppCodeStyle",
                    help="Checks C++ code Google style", action='store_true')
parser.add_argument("-t", "--checkInTravis",
                    help="Checks the code style in Travis service", action='store_true')

args = parser.parse_args()

# for options
version = args.version
c_style = args.checkCodeStyle
cpp_style = args.checkCppCodeStyle
travis_flag = args.checkInTravis


def apply_clang_format(style, path, extensions):
    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith(extensions):
                root = re.escape(root)
                file = re.escape(file)
                os.system("clang-format -i -style="
                          + style + " " + root + "/" + file)


if version == True:
    os.system(tool_path + "--version")
    exit(0)

if c_style == True:
    if not travis_flag:
        print("Formatting C codes with clang-format adopting LLVM style ...")
        # File Extension filter. You can add new extension
        extensions = (".c", ".h", ".hh")
        apply_clang_format('llvm', 'modules/backend/library', extensions)

if cpp_style == True:
    # File Extension filter. You can add new extension
    cpp_extensions = (".cxx", ".cpp", ".c", ".hxx", ".hh", ".cc", ".hpp")
    paths_to_check = ["modules/frontend", "modules/backend/pass"]

    if not travis_flag:
        print("Formatting CPP codes with clang-format adopting Google style ...")

        for path in paths_to_check:
            apply_clang_format('google', path, cpp_extensions)

    if travis_flag:
        print("Check issues related to code style ...")

        for path in paths_to_check:
            if path == "modules/backend/pass":
                subprocess.run(['python', 'utils/cpplint.py', '--recursive', '--filter=-runtime/references',
                                '--linelength=120', '--counting=detailed', path], check=True)
            else:
                subprocess.run(['python', 'utils/cpplint.py', '--recursive', '--filter=-runtime/printf',
                                '--linelength=120', '--counting=detailed', path], check=True)
