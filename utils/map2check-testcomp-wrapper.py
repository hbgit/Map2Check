#!/usr/bin/env python3
"""Test-Comp entry point for Map2Check.

Separate from map2check-wrapper.py on purpose. That wrapper answers an SV-COMP
question -- "does this program satisfy the property" -- and prints a verdict.
Test-Comp asks a different one: produce a test suite, and let TestCov decide
what it is worth. The verdict vocabulary, the exit codes and the artefact are
all different, and folding both into one script would mean a wrapper whose
behaviour depends on a property string in two unrelated ways.

Contract with the competition harness:

  * the suite is written to ./test-suite/ in the current working directory
  * the suite is written whether or not a violation was found -- an absent
    directory reads as a crashed tool, an empty suite as an honest zero
  * exit 0 means "I ran"; it does not claim the suite covers anything
"""

import argparse
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

# Resolved against this file rather than "./map2check". BenchExec's BaseTool2
# dropped the working_directory() hook that the old wrapper relied on, so the
# process starts in the benchmark's directory, not the tool's. map2check itself
# derives MAP2CHECK_PATH from /proc/self/exe, so an absolute path is fine --
# what is NOT fine is renaming the binary, which makes that derivation produce
# a garbage prefix.
MAP2CHECK = os.path.join(HERE, "map2check")

# Test-Comp 2026 coverage properties. Matched on the FQL body rather than on
# the file name: the competition names these files differently across years and
# the body is what actually defines the goal.
COVER_ERROR = "COVER EDGES(@CALL(reach_error))"
COVER_BRANCHES = "COVER EDGES(@DECISIONEDGE)"

# 900s is the Test-Comp per-task budget. The tool gets slightly less so that it
# is Map2Check that stops and writes its suite, rather than the harness killing
# it mid-write and leaving a truncated metadata.xml behind.
DEFAULT_BUDGET = 900
SHUTDOWN_MARGIN = 20


def parse_args(argv):
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument("-v", "--version", action="store_true",
                        help="print Map2Check's version")
    parser.add_argument("-p", "--propertyfile", help="path to the property file")
    parser.add_argument("--data-model", choices=["ILP32", "LP64"], default="LP64",
                        help="machine model of the task (Test-Comp: ILP32 or LP64)")
    parser.add_argument("--budget", type=int, default=DEFAULT_BUDGET,
                        help="total wall-clock budget in seconds")
    parser.add_argument("benchmark", nargs="?", help="path to the C program")
    return parser.parse_args(argv)


def read_property(path):
    with open(path, "r") as handle:
        return handle.read()


def main(argv):
    args = parse_args(argv)

    if args.version:
        return subprocess.call([MAP2CHECK, "--version"])

    if args.propertyfile is None:
        print("Please, specify a property file")
        return 1
    if args.benchmark is None:
        print("Please, specify a benchmark")
        return 1

    prop = read_property(args.propertyfile)

    if COVER_ERROR in prop:
        goal_flags = ["--target-function", "--target-function-name", "reach_error"]
    elif COVER_BRANCHES in prop:
        # Not a silent zero. Branch coverage needs one test case per input
        # vector, and the runtime currently writes a single nondet log per run
        # (H1.3 in docs/TESTCOMP-CHECKLIST.md). Emitting a one-case suite here
        # would look like participation while scoring nothing and hiding why.
        print("Unsupported Property: cover-branches needs per-input test cases "
              "(see H1.3 in docs/TESTCOMP-CHECKLIST.md)")
        return 1
    else:
        print("Unsupported Property")
        return 1

    architecture = "32bit" if args.data_model == "ILP32" else "64bit"

    # The inner budget is what Map2Check divides between LibFuzzer and KLEE; the
    # outer one is the backstop for a shutdown that hangs.
    inner = max(1, args.budget - SHUTDOWN_MARGIN)

    command = [MAP2CHECK] + goal_flags + [
        "--generate-test-suite",
        "--property-file", args.propertyfile,
        "--architecture", architecture,
        "--timeout", str(inner),
        args.benchmark,
    ]

    print("Verifying with MAP2CHECK")
    print("Command: " + " ".join(command))
    sys.stdout.flush()

    try:
        completed = subprocess.run(command, timeout=args.budget)
        rc = completed.returncode
    except subprocess.TimeoutExpired:
        print("Timed out")
        rc = 0

    suite = os.path.join(os.getcwd(), "test-suite")
    if not os.path.isdir(suite):
        # Reported, not repaired. Fabricating a metadata.xml here would hide a
        # Map2Check that died before it could write one, and the whole point of
        # always emitting the suite is that its absence means something.
        print("ERROR: no test-suite/ directory was produced")
        return 1

    print("Test suite in " + suite)
    return 0 if rc in (0, 1) else rc


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
