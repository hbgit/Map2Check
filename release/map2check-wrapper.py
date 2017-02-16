#!/usr/bin/env python

import os
import argparse
import shlex
import subprocess

tool_path = "./map2check "
# default args
extra_tool = "timeout 895s "
command_line = extra_tool + tool_path
clang_path = "../dependencies/llvm-build-cmake/bin/clang "
clang_args = "-c -emit-llvm -O0 -g "
clang_command = clang_path + clang_args
bcs_files = "bcs_files"

# Options

parser = argparse.ArgumentParser()
#parser.add_argument("-a", "--arch", help="Either 32 or 64 bits", type=int, choices=[32, 64], default=64)
parser.add_argument("-v", "--version", help="Prints Map2check's version", action='store_true')
parser.add_argument("-p", "--propertyfile", help="Path to the property file")
parser.add_argument("benchmark", nargs='?', help="Path to the benchmark")

args = parser.parse_args()

# for options
#arch = args.arch
version = args.version
property_file = args.propertyfile
benchmark = args.benchmark
bc_benchmark = args.benchmark

if version == True:
  os.system(tool_path + "--version")
  exit(0)

if property_file is None:
  print "Please, specify a property file"
  exit(1)

if benchmark is None:
  print "Please, specify a benchmark to verify"
  exit(1)
else:
  # generating .bc
  bc_benchmark = os.path.splitext(str(os.path.basename(bc_benchmark)))[0]+'.bc'
  if not os.path.exists(bcs_files):
    os.makedirs(bcs_files)

  bc_benchmark = bcs_files + "/" + bc_benchmark
  clang_command = clang_command + benchmark + " -o " + bc_benchmark
  print "Building .bc"
  print clang_command
  
  clang_args = shlex.split(clang_command)
  pclang = subprocess.Popen(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  (stdout, stderr) = pclang.communicate()

# Add witness path
#command_line += "--witness-output " + os.path.basename(benchmark) + ".graphml "

# Add arch
#if arch == 32:
#  command_line += "--32 "
#else:
#  command_line += "--64 "

# Parse property files
is_memsafety = False
is_reachability = False

f = open(property_file, 'r')
property_file_content = f.read()

if "CHECK( init(main()), LTL(G valid-free) )" in property_file_content:
  is_memsafety = True
elif "CHECK( init(main()), LTL(G ! overflow) )" in property_file_content:
  is_overflow = True
elif "CHECK( init(main()), LTL(G ! call(__VERIFIER_error())) )" in property_file_content:
  is_reachability = True
else:
  # We don't support termination
  print "Unsupported Property"
  exit(1)

if is_memsafety:
  command_line += " "
elif is_reachability:
  command_line += " --target-function __VERIFIER_error "

print "Verifying with MAP2CHECK "
# Call MAP2CHECK
command_line += bc_benchmark
print "Command: " + command_line

args = shlex.split(command_line)

p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
(stdout, stderr) = p.communicate()

# Parse output
if "Timed out" in stdout:
  print "Timed out"
  exit(1)


# Error messages
free_offset = "FALSE-FREE: Operand of free must have zero pointer offset"

if "VERIFICATION FAILED" in stdout:
    if free_offset in stdout:
      print "FALSE_FREE"
      exit(0)
    
    if is_reachability:
      print "FALSE"
      exit(0)

if "VERIFICATION SUCCESSFUL" in stdout:
  print "TRUE"
  exit(0)

print "UNKNOWN"
