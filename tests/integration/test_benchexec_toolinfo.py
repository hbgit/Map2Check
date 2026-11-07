#!/usr/bin/env python3
"""The BenchExec tool-info contract, for SV-COMP and Test-Comp.

Map2Check cannot be submitted to a competition without a tool-info module that
BenchExec accepts, and the failure mode of a wrong one is nasty: it surfaces on
competition infrastructure, days after submission, as every task erroring out.
Nothing else in this repository exercises that module, so this file does --
against the real benchexec package, not a stub, because the whole risk is that
the API is not what we assumed.

There is one module, utils/moduleBenchExec/map2check.py, a copy of
benchexec/tools/map2check.py: the BenchExec maintainers asked for Test-Comp
support to live in the existing module rather than in a second one
(sosy-lab/benchexec#1352). So the old archives must keep working with it too:
the v7 wrapper of SV-COMP 2018-2020 rejects --data-model and --budget.

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

import map2check                                        # noqa: E402

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


def fake_tool(version):
    """A tool directory whose wrapper only answers --version."""
    directory = tempfile.mkdtemp()
    wrapper = write(directory, "map2check-wrapper.py",
                    "#!/bin/sh\necho %s\n" % version)
    os.chmod(wrapper, 0o755)
    tool = map2check.Tool()
    executable = tool.executable(template.BaseTool2.ToolLocator(
        tool_directory=directory, use_path=False, use_current=False))
    return tool, executable


def run_with(exit_value, output, termination_reason=None):
    if exit_value is None:
        exit_code = util.ProcessExitCode(raw=9, value=None, signal=9)
    else:
        exit_code = util.ProcessExitCode(raw=exit_value << 8, value=exit_value, signal=None)
    return template.BaseTool2.Run(
        cmdline=["map2check-wrapper.py"],
        exit_code=exit_code,
        output=template.BaseTool2.RunOutput(output),
        termination_reason=termination_reason,
    )


def main():
    print("=== BenchExec tool-info (SV-COMP and Test-Comp) ===")

    tool, executable = fake_tool("v9.0.0-Songbirds:2026-10-13")

    check("the module is a BaseTool2, not the deprecated BaseTool",
          isinstance(tool, template.BaseTool2))
    check("the tool names itself Map2Check", tool.name() == "Map2Check",
          "got %r" % tool.name())
    check("the tool publishes a project URL",
          tool.project_url().startswith("https://"), tool.project_url())
    check("the wrapper is the executable, not the raw binary",
          executable.endswith("map2check-wrapper.py"), executable)
    check("the archive paths include the wrapper",
          "map2check-wrapper.py" in tool.program_files(executable))

    work = tempfile.mkdtemp()
    cover_error = write(work, "coverage-error-call.prp",
                        "COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )\n")
    cover_branches = write(work, "coverage-branches.prp",
                           "COVER( init(main()), FQL(COVER EDGES(@DECISIONEDGE)) )\n")
    unreachability = write(work, "unreach-call.prp",
                           "CHECK( init(main()), LTL(G ! call(reach_error())) )\n")
    program = write(work, "prog.c", "int main(void) { return 0; }\n")

    limits = template.BaseTool2.ResourceLimits(walltime=900)

    # --- Map2Check 9: the task options reach the wrapper ---------------------
    task = template.BaseTool2.Task.with_files([program], property_file=cover_error)
    cmd = tool.cmdline(executable, [], task, limits)

    check("the property file is passed through",
          "-p" in cmd and cmd[cmd.index("-p") + 1] == cover_error)
    check("the program is the last argument", cmd[-1] == program, cmd[-1])
    check("the walltime limit becomes the tool's own budget",
          "--budget" in cmd and cmd[cmd.index("--budget") + 1] == "900")
    check("a task without a data model leaves the wrapper's default (LP64)",
          "--data-model" not in cmd, cmd)

    # The machine model has to come from the task, not a guess. BenchExec only
    # reads it from a task that declares its language, as sv-benchmarks do.
    for model in ("ILP32", "LP64"):
        task_model = template.BaseTool2.Task.with_files(
            [program], property_file=cover_error,
            options={"language": "C", "data_model": model})
        cmd_model = tool.cmdline(executable, [], task_model, limits)
        check("a task declaring %s is honoured" % model,
              "--data-model" in cmd_model
              and cmd_model[cmd_model.index("--data-model") + 1] == model, cmd_model)

    # cover-branches used to be refused, because there was no source of
    # per-path vectors. KLEE's .ktest output is now that source, so a
    # regression to the refusal would silently drop a whole category.
    cmd_branches = tool.cmdline(executable, [], template.BaseTool2.Task.with_files(
        [program], property_file=cover_branches), limits)
    check("cover-branches passes its own property file",
          cmd_branches[cmd_branches.index("-p") + 1] == cover_branches)

    # One module now: an SV-COMP property is the wrapper's business, not a
    # reason for the module to refuse the task.
    cmd_check = tool.cmdline(executable, [], template.BaseTool2.Task.with_files(
        [program], property_file=unreachability), limits)
    check("an SV-COMP CHECK property is accepted", cmd_check[-1] == program)

    raises("a task with no property file is refused",
           lambda: tool.cmdline(executable, [], template.BaseTool2.Task.with_files(
               [program]), limits))

    # --- older archives: no options their wrapper would reject --------------
    old_tool, old_executable = fake_tool("v7.3.1-Flock")
    old_cmd = old_tool.cmdline(old_executable, [], task, limits)
    check("a Map2Check 7 wrapper gets neither --data-model nor --budget",
          "--data-model" not in old_cmd and "--budget" not in old_cmd, old_cmd)
    check("a Map2Check 7 command line is unchanged",
          old_cmd == [old_executable, "-p", cover_error, program], old_cmd)

    silent_tool, silent_executable = fake_tool("")
    silent_cmd = silent_tool.cmdline(silent_executable, [], task, limits)
    check("a wrapper that does not tell its version is taken as the current one",
          "--budget" in silent_cmd, silent_cmd)

    # --- determine_result ----------------------------------------------------
    check("DONE as the last line is DONE",
          tool.determine_result(run_with(0, ["Test suite in /x/test-suite", "DONE"]))
          == result.RESULT_DONE)
    check("a failed generation is ERROR",
          tool.determine_result(run_with(1, ["ERROR: no test-suite/ directory was produced"]))
          == "ERROR")
    check("a crashed map2check is ERROR, not DONE",
          tool.determine_result(run_with(2, ["Test suite in /x/test-suite",
                                             "ERROR: map2check exited with 2"]))
          == "ERROR")
    check("a run killed by the harness is TIMEOUT",
          tool.determine_result(run_with(None, ["Generating tests with MAP2CHECK"],
                                         termination_reason="walltime"))
          == result.RESULT_TIMEOUT)
    check("SV-COMP verdicts are untouched: TRUE",
          tool.determine_result(run_with(0, ["TRUE"])) == result.RESULT_TRUE_PROP)
    check("SV-COMP verdicts are untouched: FALSE_DEREF",
          tool.determine_result(run_with(0, ["FALSE_DEREF"])) == result.RESULT_FALSE_DEREF)
    check("SV-COMP verdicts are untouched: FALSE",
          tool.determine_result(run_with(0, ["FALSE"])) == result.RESULT_FALSE_REACH)

    print("  ---")
    print("  Results: %d passed, %d failed" % (PASSED, FAILED))
    return 1 if FAILED else 0


if __name__ == "__main__":
    sys.exit(main())
