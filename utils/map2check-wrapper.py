#!/usr/bin/env python3
"""Competition entry point for Map2Check, for both SV-COMP and Test-Comp.

One wrapper, because BenchExec has one tool-info module per tool
(benchexec/tools/map2check.py): the property file decides what is asked.

  CHECK(...)  SV-COMP verification. Prints a verdict as the last line
              (TRUE, FALSE, FALSE_DEREF, ..., UNKNOWN).
  COVER(...)  Test-Comp test generation. Writes ./test-suite/ and prints
              DONE as the last line. The suite is written whether or not a
              violation was found -- an absent directory reads as a crashed
              tool, an empty suite as an honest zero -- and DONE does not
              claim the suite covers anything; the validators decide that.

Both Test-Comp goals are supported, with AFL++ and KLEE taking turns:

  cover-error     one test case, the violating input vector of whichever
                  engine reached reach_error first
  cover-branches  up to 50 test cases: the fuzzer's corpus (at most half) and
                  one input vector per path KLEE explored
"""

import argparse
import os
import shlex
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

# Resolved against this file rather than "./map2check", so that the wrapper
# does not depend on the directory BenchExec starts it in. map2check itself
# derives MAP2CHECK_PATH from /proc/self/exe, so an absolute path is fine --
# what is NOT fine is renaming the binary, which makes that derivation produce
# a garbage prefix.
MAP2CHECK = os.path.join(HERE, "map2check")

# --- SV-COMP ------------------------------------------------------------------

SVCOMP_OUTER_TIMEOUT = "897s"
SVCOMP_TIMEOUT = "895"

# --- Test-Comp ----------------------------------------------------------------

# Test-Comp coverage properties. Matched on the FQL body rather than on the
# file name: the competition names these files differently across years and
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
                        help="machine model of the task (ILP32 or LP64)")
    parser.add_argument("--budget", type=int, default=DEFAULT_BUDGET,
                        help="total wall-clock budget in seconds (Test-Comp)")
    parser.add_argument("benchmark", nargs="?", help="path to the C program")
    return parser.parse_args(argv)


def tool_environment():
    """The environment map2check runs with.

    In the competition archive (utils/testcomp-archive/make-archive.sh) AFL++
    travels inside the tool directory, since the competition machines do not
    have it: map2check is pointed at the bundled afl-fuzz and afl-clang-fast,
    afl-cc at its passes and runtime objects (AFL_PATH) and at the bundled
    LLVM 16 clang (AFL_CC). Outside the archive -- a development install --
    none of these exist and the defaults (/usr/local) stay in force."""
    env = dict(os.environ)
    bundled = {
        "MAP2CHECK_AFL_FUZZ": os.path.join(HERE, "bin", "afl-fuzz"),
        "MAP2CHECK_AFL_CC": os.path.join(HERE, "bin", "afl-clang-fast"),
        "AFL_PATH": os.path.join(HERE, "lib", "afl"),
        "AFL_CC": os.path.join(HERE, "bin", "clang"),
    }
    if os.path.exists(bundled["MAP2CHECK_AFL_FUZZ"]):
        for key, value in bundled.items():
            env.setdefault(key, value)
    return env


def architecture(data_model):
    return "32bit" if data_model == "ILP32" else "64bit"


def generate_tests(args, prop):
    """Test-Comp: write ./test-suite/ and end with DONE."""
    if COVER_ERROR in prop:
        # One test case: the violating vector.
        goal_flags = ["--target-function", "--target-function-name", "reach_error"]
    else:
        # The fuzzer's corpus and one test case per path KLEE explored.
        # No target function: the goal is to cover branches, and asking the
        # tool to hunt for reach_error would make it stop at the first error
        # instead of exploring.
        goal_flags = ["--cover-branches"]

    # The inner budget is what Map2Check divides between AFL++ and KLEE; the
    # outer one is the backstop for a shutdown that hangs.
    inner = max(1, args.budget - SHUTDOWN_MARGIN)

    command = [MAP2CHECK] + goal_flags + [
        "--generate-test-suite",
        "--property-file", args.propertyfile,
        "--architecture", architecture(args.data_model),
        "--timeout", str(inner),
        args.benchmark,
    ]

    print("Generating tests with MAP2CHECK")
    print("Command: " + " ".join(command))
    sys.stdout.flush()

    try:
        completed = subprocess.run(command, timeout=args.budget, env=tool_environment())
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
    if rc not in (0, 1):
        print("ERROR: map2check exited with %d" % rc)
        return rc
    print("DONE")
    return 0


def verify(args, prop):
    """SV-COMP: print a verdict as the last line."""
    is_memsafety = False
    is_reachability = False
    is_overflow = False
    is_memcleanup = False

    if "CHECK( init(main()), LTL(G valid-free) )" in prop:
        is_memsafety = True
    elif "CHECK( init(main()), LTL(G valid-deref) )" in prop:
        is_memsafety = True
    elif "CHECK( init(main()), LTL(G valid-memtrack) )" in prop:
        is_memsafety = True
    elif "CHECK( init(main()), LTL(G valid-memcleanup) )" in prop:
        is_memcleanup = True
    elif "CHECK( init(main()), LTL(G ! overflow) )" in prop:
        is_overflow = True
    elif "CHECK( init(main()), LTL(G ! call(__VERIFIER_error())) )" in prop:
        is_reachability = True
    else:
        # We don't support termination
        print("Unsupported Property")
        return 1

    command_line = ("timeout " + SVCOMP_OUTER_TIMEOUT + " " + shlex.quote(MAP2CHECK)
                    + " --architecture " + architecture(args.data_model))

    # Set options
    command_line_bkp = ""
    if is_memsafety:
        command_line += " --timeout " + SVCOMP_TIMEOUT + " --memtrack --smt-solver yices2 --generate-witness "
    elif is_memcleanup:
        command_line += " --timeout " + SVCOMP_TIMEOUT + " --memcleanup-property --smt-solver yices2 --generate-witness "
    elif is_reachability:
        # --add-invariants needs Clam (the renamed crab-llvm), which this build does
        # not ship. map2check now exits 3 rather than ignoring the flag, so this
        # branch fails until Clam lands -- deliberately. Do not drop the flag to
        # silence it: the request is what makes the gap visible, and hiding it is how
        # the flag stayed dead through an entire baseline (issue #54).
        #
        # command_line_bkp below is the pre-existing no-invariants fallback, and it is
        # intentionally NOT wired to exit 3: it fires only on the Crab-specific
        # 'failed external call' signal further down. Wiring it here would restore the
        # silence this change exists to remove.
        # See docs/reports/2026-08-16-crabllvm-review.md.
        command_line_bkp = command_line + " --timeout " + SVCOMP_TIMEOUT + " --smt-solver yices2 --target-function --generate-witness "
        command_line += " --timeout " + SVCOMP_TIMEOUT + " --smt-solver yices2 --add-invariants --target-function --generate-witness "
    elif is_overflow:
        command_line += " --timeout " + SVCOMP_TIMEOUT + " --check-overflow --smt-solver yices2 --generate-witness "

    print("Verifying with MAP2CHECK ")

    # Call MAP2CHECK
    command_line += shlex.quote(args.benchmark)
    command_line_bkp += shlex.quote(args.benchmark)

    print("Command: " + command_line)
    env = tool_environment()
    p = subprocess.Popen(shlex.split(command_line), stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE, env=env)
    (stdout, stderr) = p.communicate()

    # Check invariant conflict
    if b'failed external call: __map2check_main__' in stderr:
        if is_reachability:
            p = subprocess.Popen(shlex.split(command_line_bkp), stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE, env=env)
            (stdout, stderr) = p.communicate()

    output = stdout.decode()

    # Parse output
    if 'Timed out' in output:
        print("Timed out")
        return 1

    # Error messages
    free_offset = "\tFALSE-FREE: Operand of free must have zero pointer offset"
    deref_offset = "\tFALSE-DEREF: Reference to pointer was lost"
    memtrack_offset = "\tFALSE-MEMTRACK"
    memcleanup_offset = "\tFALSE-MEMCLEANUP"
    target_offset = "\tFALSE: Target Reached"
    overflow_offset = "\tOVERFLOW"

    if "VERIFICATION FAILED" in output:
        if free_offset in output:
            print("FALSE_FREE")
            return 0
        if deref_offset in output:
            print("FALSE_DEREF")
            return 0
        if memtrack_offset in output:
            print("FALSE_MEMTRACK")
            return 0
        if memcleanup_offset in output:
            print("FALSE_MEMCLEANUP")
            return 0
        if target_offset in output:
            print("FALSE")
            return 0
        if overflow_offset in output:
            print("FALSE_OVERFLOW")
            return 0

    if "VERIFICATION SUCCEEDED" in output:
        print("TRUE")
        return 0

    print("UNKNOWN")
    return 0


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

    with open(args.propertyfile, "r") as handle:
        prop = handle.read()

    if COVER_ERROR in prop or COVER_BRANCHES in prop:
        return generate_tests(args, prop)
    return verify(args, prop)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
