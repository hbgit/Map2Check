# This file is part of Map2Check <https://github.com/hbgit/Map2Check>.
#
# SPDX-License-Identifier: Apache-2.0
"""BenchExec tool-info module for Map2Check as a Test-Comp test generator.

Kept separate from map2check.py, which is the SV-COMP verification adaptor and
still targets the deprecated BaseTool (1.x) API. The two describe genuinely
different tools to BenchExec: one reports a verification verdict, the other
produces an artefact and lets TestCov score it. Merging them would mean a
determine_result whose meaning depends on a property string.
"""

import benchexec.result as result
import benchexec.tools.template

# The FQL bodies, matched rather than the file names -- the competition renames
# the property files between editions but the goal expression is what defines
# the task.
COVER_ERROR = "COVER EDGES(@CALL(reach_error))"
COVER_BRANCHES = "COVER EDGES(@DECISIONEDGE)"


class Tool(benchexec.tools.template.BaseTool2):
    """Tool adaptor for Map2Check (https://github.com/hbgit/Map2Check).

    Both Test-Comp goals are supported. The wrapper picks the source of test
    vectors from the property: cover-error takes the single violating vector
    from the runtime's nondet log, cover-branches takes one vector per path
    from KLEE's own .ktest output.
    """

    REQUIRED_PATHS = [
        "map2check",
        "map2check-testcomp-wrapper.py",
        "bin",
        "include",
        "lib",
    ]

    def executable(self, tool_locator):
        return tool_locator.find_executable("map2check-testcomp-wrapper.py")

    def name(self):
        return "Map2Check"

    def project_url(self):
        return "https://github.com/hbgit/Map2Check"

    def version(self, executable):
        return self._version_from_tool(executable)

    def program_files(self, executable):
        return self._program_files_from_executable(executable, self.REQUIRED_PATHS)

    def cmdline(self, executable, options, task, rlimits):
        if task.property_file is None:
            raise benchexec.tools.template.UnsupportedFeatureException(
                "Map2Check needs a property file to know what to cover"
            )

        with open(task.property_file, "r") as handle:
            spec = handle.read()

        if COVER_ERROR not in spec and COVER_BRANCHES not in spec:
            raise benchexec.tools.template.UnsupportedFeatureException(
                "unsupported property: " + spec.strip()
            )

        # LP64 is the Test-Comp default when a task definition says nothing.
        data_model = (task.options or {}).get("data_model", "LP64")

        cmd = [executable, "-p", task.property_file, "--data-model", data_model]
        if rlimits.walltime:
            # Handing the tool its own budget is what lets it stop and finish
            # writing the suite. Left to the harness's kill, metadata.xml can be
            # truncated mid-write, which is worse than an empty suite.
            cmd += ["--budget", str(rlimits.walltime)]
        return cmd + list(options) + [task.single_input_file]

    def determine_result(self, run):
        # Test-Comp scores the artefact, not a verdict, so the only thing this
        # has to distinguish is "ran" from "broke". Saying anything stronger
        # would be this module claiming to know whether the suite covers the
        # error -- which is TestCov's job and precisely what the competition
        # does not let the tool self-report.
        #
        # A timeout still counts as DONE: the tool budgets itself below the
        # harness limit and writes the suite before returning, so a run killed
        # at the wall clock has normally already produced one.
        if run.was_timeout:
            return result.RESULT_DONE
        if run.exit_code.value == 0:
            return result.RESULT_DONE
        return result.RESULT_ERROR
