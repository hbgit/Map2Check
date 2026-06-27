#!/usr/bin/env python3
"""
Map2Check v8 wrapper for TestComp 2026.
Translates TestComp properties into Map2Check CLI flags.

Changes from v7.3.1 wrapper:
  - SMT solver: z3 (was yices2)
  - Target function: --target-function-name reach_error (direct, no translation)
  - Timeout: 295s (was 895s)
  - Coverage-branches: returns UNKNOWN (not supported)
"""

import os
import argparse
import shlex
import subprocess

tool_path = "./map2check "
extra_tool = "timeout 295s "
command_line = extra_tool + tool_path
timemapsplit = "290"

# Options
parser = argparse.ArgumentParser()
parser.add_argument("-v", "--version", help="Prints Map2check's version", action='store_true')
parser.add_argument("-p", "--propertyfile", help="Path to the property file")
parser.add_argument("benchmark", nargs='?', help="Path to the benchmark")

args = parser.parse_args()

version = args.version
property_file = args.propertyfile
benchmark = args.benchmark

if version:
    os.system(tool_path + "--version")
    exit(0)

if property_file is None:
    print("Please, specify a property file")
    exit(1)

if benchmark is None:
    print("Please, specify a benchmark to verify")
    exit(1)

is_memsafety = False
is_reachability = False
is_overflow = False
is_memcleanup = False
is_coverage_branches = False

f = open(property_file, 'r')
property_file_content = f.read()
f.close()

if "CHECK( init(main()), LTL(G valid-free) )" in property_file_content:
    is_memsafety = True
elif "CHECK( init(main()), LTL(G valid-deref) )" in property_file_content:
    is_memsafety = True
elif "CHECK( init(main()), LTL(G valid-memtrack) )" in property_file_content:
    is_memsafety = True
elif "CHECK( init(main()), LTL(G valid-memcleanup) )" in property_file_content:
    is_memcleanup = True
elif "CHECK( init(main()), LTL(G ! overflow) )" in property_file_content:
    is_overflow = True
elif "COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )" in property_file_content:
    is_reachability = True
elif "COVER( init(main()), FQL(COVER EDGES(@DECISIONEDGE)) )" in property_file_content:
    is_coverage_branches = True
else:
    print("Unsupported Property")
    exit(1)

# Coverage-branches is not supported by Map2Check
if is_coverage_branches:
    print("UNKNOWN")
    print("Coverage-branches property not supported")
    exit(0)

# Set options
command_line_bkp = ""
if is_memsafety:
    command_line += " --timeout " + timemapsplit + " --memtrack --smt-solver z3 --generate-witness "
elif is_memcleanup:
    command_line += " --timeout " + timemapsplit + " --memcleanup-property --smt-solver z3 --generate-witness "
elif is_reachability:
    # v8: use reach_error directly as target function name (PR #44)
    command_line += (" --timeout " + timemapsplit
                     + " --smt-solver z3"
                     + " --target-function"
                     + " --target-function-name reach_error"
                     + " --generate-witness ")
elif is_overflow:
    command_line += " --timeout " + timemapsplit + " --check-overflow --smt-solver z3 --generate-witness "

print("Verifying with MAP2CHECK v8 (LLVM 16)")

# Set KLEE runtime library path (needed for klee-uclibc.bca)
script_dir = os.path.dirname(os.path.abspath(__file__))
klee_runtime = os.path.join(script_dir, "lib", "klee", "runtime")
os.environ["KLEE_RUNTIME_LIBRARY_PATH"] = klee_runtime

# Call MAP2CHECK
command_line += benchmark
print("Command: " + command_line)
args_cmd = shlex.split(command_line)
p = subprocess.Popen(args_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
(stdout, stderr) = p.communicate()

# Parse output
if b'Timed out' in stdout:
    print("Timed out")
    exit(1)

# Error messages
free_offset     = "\tFALSE-FREE: Operand of free must have zero pointer offset"
deref_offset    = "\tFALSE-DEREF: Reference to pointer was lost"
memtrack_offset = "\tFALSE-MEMTRACK"
memcleanup_offset = "\tFALSE-MEMCLEANUP"
target_offset   = "\tFALSE: Target Reached"
overflow_offset = "\tOVERFLOW"

if "VERIFICATION FAILED" in stdout.decode():
    if free_offset in stdout.decode():
        print("FALSE_FREE")
        exit(0)

    if deref_offset in stdout.decode():
        print("FALSE_DEREF")
        exit(0)

    if memtrack_offset in stdout.decode():
        print("FALSE_MEMTRACK")
        exit(0)

    if memcleanup_offset in stdout.decode():
        print("FALSE_MEMCLEANUP")
        exit(0)

    if target_offset in stdout.decode():
        print("FALSE")
        exit(0)

    if overflow_offset in stdout.decode():
        print("FALSE_OVERFLOW")
        exit(0)

if "VERIFICATION SUCCEEDED" in stdout.decode():
    print("TRUE")
    exit(0)

print("UNKNOWN")
exit(0)
