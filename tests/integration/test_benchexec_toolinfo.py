#!/usr/bin/env python3
"""The BenchExec tool-info contract for Test-Comp.

Map2Check cannot be submitted to Test-Comp without a tool-info module that
BenchExec accepts, and the failure mode of a wrong one is nasty: it surfaces on
competition infrastructure, days after submission, as every task erroring out.
Nothing else in this repository exercises that module, so this file does --
against the real benchexec package, not a stub, because the whole risk is that
the API is not what we assumed.

Run with: python3 tests/integration/test_benchexec_toolinfo.py
Requires: pip3 install benchexec (already present wherever testcov is).
"""

import os
import sys
import tempfile

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(REPO, "utils", "moduleBenchExec"))

import benchexec.result as result                       # noqa: E402
import benchexec.tools.template as template             # noqa: E402
import benchexec.util as util                           # noqa: E402

import map2check_testcomp                               # noqa: E402

PASSED = 0
FAILED = 0


def ok(what):
    global PASSED
    print("  PASS %s" % what)
    PASSED += 1


def fail(what, why):
    global FAILED
    print("  FAIL %s: %s" % (what, why))
    FAILED += 1


def check(what, condition, why=""):
    ok(what) if condition else fail(what, why)


def raises(what, fn, exc=template.UnsupportedFeatureException):
    try:
        fn()
    except exc:
        ok(what)
    except Exception as other:                          # noqa: BLE001
        fail(what, "raised %s instead of %s" % (type(other).__name__, exc.__name__))
    else:
        fail(what, "did not raise")


def write(directory, name, text):
    path = os.path.join(directory, name)
    with open(path, "w") as handle:
        handle.write(text)
    return path


def run_with(exit_value):
    return template.BaseTool2.Run(
        cmdline=["map2check"],
        exit_code=util.ProcessExitCode(raw=exit_value << 8, value=exit_value, signal=None),
        output=[],
        termination_reason=None,
    )


def main():
    tool = map2check_testcomp.Tool()

    print("=== BenchExec tool-info (Test-Comp) ===")

    check("the module is a BaseTool2, not the deprecated BaseTool",
          isinstance(tool, template.BaseTool2))
    check("the tool names itself Map2Check", tool.name() == "Map2Check",
          "got %r" % tool.name())
    check("the tool publishes a project URL",
          tool.project_url().startswith("https://"), tool.project_url())

    work = tempfile.mkdtemp()
    cover_error = write(work, "coverage-error-call.prp",
                        "COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )\n")
    cover_branches = write(work, "coverage-branches.prp",
                           "COVER( init(main()), FQL(COVER EDGES(@DECISIONEDGE)) )\n")
    unreachability = write(work, "unreach-call.prp",
                           "CHECK( init(main()), LTL(G ! call(reach_error())) )\n")
    program = write(work, "prog.c", "int main(void) { return 0; }\n")

    limits = template.BaseTool2.ResourceLimits(walltime=900)

    # --- cover-error: the one property Map2Check claims to support -----------
    task = template.BaseTool2.Task.with_files([program], property_file=cover_error)
    cmd = tool.cmdline("/opt/m2c/map2check-testcomp-wrapper.py", [], task, limits)

    check("the wrapper is the executable, not the raw binary",
          cmd[0].endswith("map2check-testcomp-wrapper.py"), cmd[0])
    check("the property file is passed through",
          "-p" in cmd and cmd[cmd.index("-p") + 1] == cover_error)
    check("the program is the last argument", cmd[-1] == program, cmd[-1])
    check("the walltime limit becomes the tool's own budget",
          "--budget" in cmd and cmd[cmd.index("--budget") + 1] == "900")
    check("LP64 is the default machine model",
          "--data-model" in cmd and cmd[cmd.index("--data-model") + 1] == "LP64")

    # --- the machine model has to come from the task, not a guess ------------
    task32 = template.BaseTool2.Task.with_files(
        [program], property_file=cover_error, options={"data_model": "ILP32"})
    cmd32 = tool.cmdline("/opt/m2c/map2check-testcomp-wrapper.py", [], task32, limits)
    check("a task declaring ILP32 is honoured",
          cmd32[cmd32.index("--data-model") + 1] == "ILP32")

    # --- unsupported inputs must be refused, not quietly mishandled ----------
    # A tool-info that accepts cover-branches would produce runs that score
    # zero while looking like participation. Refusing makes BenchExec skip.
    raises("cover-branches is refused rather than faked",
           lambda: tool.cmdline("x", [], template.BaseTool2.Task.with_files(
               [program], property_file=cover_branches), limits))
    raises("an SV-COMP CHECK property is refused by the Test-Comp module",
           lambda: tool.cmdline("x", [], template.BaseTool2.Task.with_files(
               [program], property_file=unreachability), limits))
    raises("a task with no property file is refused",
           lambda: tool.cmdline("x", [], template.BaseTool2.Task.with_files(
               [program]), limits))

    # --- determine_result ----------------------------------------------------
    check("a clean exit is DONE",
          tool.determine_result(run_with(0)) == result.RESULT_DONE)
    check("a failing exit is ERROR",
          tool.determine_result(run_with(1)) == result.RESULT_ERROR)

    # A timeout is DONE because the wrapper budgets itself below the harness
    # limit and writes the suite before returning.
    timed_out = template.BaseTool2.Run(
        cmdline=["map2check"],
        exit_code=util.ProcessExitCode(raw=9, value=None, signal=9),
        output=[],
        termination_reason="walltime",
    )
    check("a timeout still counts as DONE, because the suite was written",
          tool.determine_result(timed_out) == result.RESULT_DONE)

    print("  ---")
    print("  Results: %d passed, %d failed" % (PASSED, FAILED))
    return 1 if FAILED else 0


if __name__ == "__main__":
    sys.exit(main())
