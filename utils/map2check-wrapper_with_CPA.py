#!/usr/bin/env python

import os
import argparse
import shlex
import subprocess

tool_path = "./map2check "
# default args
extra_tool = "timeout 895s "
command_line = extra_tool + tool_path
clang_path = "bin/clang "
clang_args = "-c -emit-llvm -O0 -g "
clang_command = clang_path + clang_args
bcs_files = "bcs_files"

#remove old tmp files
if not os.path.isdir("witness_file"):
    os.system("mkdir witness_file")
else:
    if os.path.isfile("witness_file/witness.graphml"):
        os.system("rm witness_file/witness.graphml")


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

def check_witness(benchmark_name, typeResult, witnessPath):
    global property_file
    global benchmark
    
    if not os.path.isfile(witnessPath):
        outputwrt = str(benchmark) + ";" + "ERROR"
        os.system("echo \""+outputwrt+"\" >> cpa_witness_check_log.csv")
        exit(1)

    if typeResult == False:
       command_line_cpa = "../../../witness_checker/CPAchecker/scripts/cpa.sh -noout -heap 10000M -predicateAnalysis "+\
                          " -setprop cpa.composite.aggregateBasicBlocks=false "+\
                          " -setprop cfa.simplifyCfa=false "+\
                          " -setprop cfa.allowBranchSwapping=false "+\
                          " -setprop cpa.predicate.ignoreIrrelevantVariables=false "+\
                          " -setprop counterexample.export.assumptions.assumeLinearArithmetics=true "+\
                          " -setprop analysis.traversal.byAutomatonVariable=__DISTANCE_TO_VIOLATION "+\
                          " -setprop cpa.automaton.treatErrorsAsTargets=false "+\
                          " -setprop WitnessAutomaton.cpa.automaton.treatErrorsAsTargets=true "+\
                          " -setprop parser.transformTokensToLines=false "+\
                          " -skipRecursion " +\
                          " -spec " + property_file +\
                          " -spec witness_file/witness.graphml "+\
                          " " +  benchmark
    #elif typeResult == True:


       #print str(command_line_cpa)
       args = shlex.split(command_line_cpa)

       p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
       (stdout, stderr) = p.communicate()

       # Parse output witness checker       
       if "TRUE" in stdout:
            outputwrt = str(benchmark) + ";" + "TRUE"
            os.system("echo \""+outputwrt+"\" >> cpa_witness_check_log.csv")
       elif "FALSE" in stdout:
            print str(stdout)
            outputwrt = str(benchmark) + ";" + "FALSE"
            os.system("echo \""+outputwrt+"\" >> cpa_witness_check_log.csv")
       else:
            outputwrt = str(benchmark) + ";" + "UNKNOWN"
            os.system("echo \""+outputwrt+"\" >> cpa_witness_check_log.csv")




if version == True:
  os.system(tool_path + "--version")
  exit(0)

if property_file is None:
  print "Please, specify a property file"
  exit(1)

if benchmark is None:
  print "Please, specify a benchmark to verify"
  exit(1)

is_memsafety = False
is_reachability = False

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
  print "Unsupported Property"
  exit(1)
elif "CHECK( init(main()), LTL(G ! call(__VERIFIER_error())) )" in property_file_content:
  is_reachability = True
else:
  # We don't support termination
  print "Unsupported Property"
  exit(1)

if is_memsafety:
  command_line += " --generate-witness "
elif is_reachability:
  command_line += " --target-function __VERIFIER_error --generate-witness "

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
free_offset = "\tFALSE-FREE: Operand of free must have zero pointer offset"
deref_offset = "\tFALSE-DEREF: Reference to pointer was lost"
memtrack_offset = "\tFALSE-MEMTRACK"
target_offset = "\tFALSE: Target Reached"

if "VERIFICATION FAILED" in stdout:
    if free_offset in stdout:
      print "FALSE_FREE"
      check_witness(benchmark, False, "witness_file/witness.graphml")
      exit(0)
    
    if deref_offset in stdout:
      print "FALSE_DEREF"
      check_witness(benchmark, False, "witness_file/witness.graphml")
      exit(0)

    if memtrack_offset in stdout:
      print "FALSE_MEMTRACK"
      check_witness(benchmark, False, "witness_file/witness.graphml")
      exit(0)  

    if target_offset in stdout:
      print "FALSE"
      check_witness(benchmark, False, "witness_file/witness.graphml")
      exit(0)

if "VERIFICATION SUCCEEDED" in stdout:
  print "TRUE"
  exit(0)

print "UNKNOWN"
