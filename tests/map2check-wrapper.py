#!/usr/bin/env python

import os
import argparse
import shlex
import subprocess

tool_path = "../release/map2check "

# Options
parser = argparse.ArgumentParser()
parser.add_argument("-t", "--timeout", help="Configure time limit in seconds", type=int, default=60)
parser.add_argument("-v", "--version", help="Prints Map2check's version", action='store_true')
parser.add_argument("-p", "--propertyfile", help="Path to the property file")
parser.add_argument("benchmark", nargs='?', help="Path to the benchmark")

args = parser.parse_args()

# for options
version = args.version
property_file = args.propertyfile
benchmark = args.benchmark
bc_benchmark = args.benchmark

# default args
extra_tool = "timeout -s 9 -k 5 " + str(args.timeout) + " "
command_line = extra_tool + tool_path


if version == True:
  os.system(tool_path + "--version")
  exit(0)

if property_file is None:
  print "Please, specify a property file"
  exit(1)

if benchmark is None:
  print "Please, specify a benchmark to verify"
  exit(1)

is_memsafety    = False
is_reachability = False
is_overflow     = False

f = open(property_file, 'r')
property_file_content = f.read()

if "CHECK( init(main()), LTL(G valid-free) )" in property_file_content:
    is_memsafety = True
elif "CHECK( init(main()), LTL(G valid-deref) )" in property_file_content:
    is_memsafety = True
elif "CHECK( init(main()), LTL(G valid-memtrack) )" in property_file_content:
    is_memsafety = True
elif "CHECK( init(main()), LTL(G ! overflow) )" in property_file_content:
    is_overflow = True  
elif "CHECK( init(main()), LTL(G ! call(__VERIFIER_error())) )" in property_file_content:
    is_reachability = True
elif "CHECK( init(main()), LTL(G valid-assert) )" in property_file_content:
    is_assert = True
else:
    # We don't support termination
    print "Unsupported Property"
    exit(1)

if is_memsafety:    
    command_line += " "
elif is_reachability:    
    command_line += " --target-function "
elif is_overflow:
    command_line += " --check-overflow "
elif is_assert:
    command_line += " --check-asserts "

# Calling MAP2CHECK
command_line += bc_benchmark
# print "Command: " + command_line

args = shlex.split(command_line)

p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
(stdout, stderr) = p.communicate()

# Parse output
if "Timed out" in stdout:
  print "RESULT: Timed out"
  exit(1)


# Error messages
free_offset = "\tFALSE-FREE: Operand of free must have zero pointer offset"
deref_offset = "\tFALSE-DEREF: Reference to pointer was lost"
memtrack_offset = "\tFALSE-MEMTRACK"
target_offset = "\tFALSE: Target Reached"
assert_offset = "FALSE-ASSERT"
overflow_offset = "\tOVERFLOW"

if "VERIFICATION FAILED" in stdout:
    if free_offset in stdout:
      print "RESULT: FALSE_FREE"
      exit(0)
    
    if deref_offset in stdout:
      print "RESULT: FALSE_DEREF"
      exit(0)

    if memtrack_offset in stdout:
      print "RESULT: FALSE_MEMTRACK"
      exit(0)  

    if target_offset in stdout:
      print "RESULT: FALSE"
      exit(0)
      
    if overflow_offset in stdout:
      print "RESULT: FALSE_OVERFLOW"
      exit(0)
      
    if assert_offset in stdout:
      print "RESULT: FALSE_ASSERT"
      exit(0)
      

if "VERIFICATION SUCCEEDED" in stdout:
  print "RESULT: TRUE"
  exit(0)

print "RESULT: UNKNOWN"
exit(0)
