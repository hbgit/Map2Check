#!/usr/bin/env python3
"""Validates Map2Check test suites the way Test-Comp 2026 does.

Runs inside the reproduction image (see Dockerfile and validate.sh). For each
of the five validators the category structure of the testcomp26 tag lists --
TestCov with gcc and clang, formatted and unformatted, and TestCoCa -- it takes
the official benchmark definition from bench-defs/, keeps its limits, options
and columns as they are, and rewrites only two things:

  - the task set, replaced by the tasks given on the command line;
  - the location of the suites, ../results-verified/LOGDIR/ in the original,
    replaced by --suites.

It drops the <require cpuModel> line, which pins the competition's hardware.
BenchExec then runs each validator, and the results are merged as the
competition merges them (competition-scripts,
prepare_tables/export_adjusted_csvs.py, update_score_tester):

  - coverage-branches: the score is the BEST coverage any validator measured;
  - coverage-error-call: 1 point if ANY validator answers TRUE.

Suites are expected at <suites>/<rundefinition>/<taskdef_name>/test-suite.zip,
for instance suites/Test-Comp26_coverage-branches/AllInterval-011.yml/test-suite.zip
(BenchExec's ${taskdef_name} keeps the .yml).
"""

import argparse
import bz2
import csv
import glob
import os
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET

HERE = os.path.dirname(os.path.abspath(__file__))
DEFS = os.path.join(HERE, "bench-defs")

VALIDATORS = {
    "testcov-validate-test-suites-gcc-unformatted": "/opt/testcov",
    "testcov-validate-test-suites-gcc-formatted": "/opt/testcov",
    "testcov-validate-test-suites-clang-unformatted": "/opt/testcov",
    "testcov-validate-test-suites-clang-formatted": "/opt/testcov",
    "testcoca-validate-test-suites-clang-formatted": "/opt/testcoca",
}

PROPERTIES = {
    "cover-branches": "Test-Comp26_coverage-branches",
    "cover-error": "Test-Comp26_coverage-error-call",
}

OFFICIAL_LOGDIRS = ("../../results-verified/LOGDIR/", "../results-verified/LOGDIR/")


def task_files(args, staging):
    """The .yml task definitions to validate, as absolute paths into `staging`.

    Each task's inputs -- the .yml, the programs it names, and the property
    files -- are copied there, keeping their paths relative to sv-benchmarks.
    TestCov writes beside the program (<program>.c.i), and on the competition's
    VerifierCloud the inputs sit in the run's own writable directory; a copy
    reproduces that and leaves the corpus untouched."""
    import yaml

    shutil.rmtree(staging, ignore_errors=True)
    shutil.copytree(os.path.join(args.sv_benchmarks, "properties"),
                    os.path.join(staging, "properties"))
    tasks = []
    with open(args.tasks) as listing:
        for line in listing:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            # A manifest row (category<TAB>program<TAB>...) or a bare path.
            path = line.split("\t")[1] if "\t" in line else line
            stem, _ = os.path.splitext(path)
            yml = os.path.join(args.sv_benchmarks, stem + ".yml")
            if not os.path.isfile(yml):
                sys.exit(f"no task definition for {path}: {yml}")
            with open(yml) as handle:
                inputs = yaml.safe_load(handle)["input_files"]
            inputs = [inputs] if isinstance(inputs, str) else inputs
            target_dir = os.path.join(staging, os.path.dirname(stem))
            os.makedirs(target_dir, exist_ok=True)
            for name in [os.path.basename(yml)] + inputs:
                shutil.copy2(os.path.join(os.path.dirname(yml), name),
                             os.path.join(target_dir, name))
            tasks.append(os.path.join(target_dir, os.path.basename(yml)))
    return tasks


def rewrite_definition(name, rundefinition, tasks, suites, sv_benchmarks, out_dir):
    """The official definition of `name`, narrowed to our tasks and suites."""
    tree = ET.parse(os.path.join(DEFS, name + ".xml"))
    root = tree.getroot()
    for require in root.findall("require"):
        root.remove(require)
    kept = False
    for run in list(root.findall("rundefinition")):
        if run.get("name") != rundefinition:
            root.remove(run)
            continue
        kept = True
        for element in run.iter():
            if element.text:
                for logdir in OFFICIAL_LOGDIRS:
                    element.text = element.text.replace(logdir, suites + "/")
                element.text = element.text.replace("../sv-benchmarks/c/", sv_benchmarks + "/")
        for old in run.findall("tasks"):
            run.remove(old)
        group = ET.SubElement(run, "tasks", name="map2check")
        for task in tasks:
            ET.SubElement(group, "include").text = task
    if not kept:
        sys.exit(f"{name}: no rundefinition {rundefinition}")
    path = os.path.join(out_dir, "defs", name + ".xml")
    os.makedirs(os.path.dirname(path), exist_ok=True)
    tree.write(path, xml_declaration=True, encoding="utf-8")
    return path


def run_benchexec(definition, tool_dir, out_dir, staging, args):
    """Runs one validator as the competition scripts do: from inside a fresh
    copy of the tool (execute-runcollection.sh unpacks it into bin/<tool>-XXXX
    and runs BenchExec there with --tool-directory .), so the tool may write
    beside itself -- TestCov's --format does -- through the overlay on ./.
    The staged task inputs are writable for the same reason (task_files)."""
    work = os.path.join(out_dir, "tool")
    shutil.rmtree(work, ignore_errors=True)
    shutil.copytree(tool_dir, work, symlinks=True)
    command = [
        "benchexec", definition,
        "--tool-directory", ".",
        "--outputpath", out_dir + "/",
        "--numOfThreads", str(args.threads),
        "--no-compress-results",
        "--read-only-dir", "/", "--overlay-dir", "/home/", "--overlay-dir", "./",
        # An overlay, not full access: what one validator writes there (the
        # preprocessed program) must not reach the next.
        "--overlay-dir", staging,
    ]
    command += args.benchexec_args
    print("+", " ".join(command), flush=True)
    code = subprocess.call(command, cwd=work)
    shutil.rmtree(work, ignore_errors=True)
    return code


def read_results(out_dir):
    """task name -> (status, coverage %) from the BenchExec result XMLs."""
    found = {}
    for path in glob.glob(os.path.join(out_dir, "*.results.*.xml")) + glob.glob(
        os.path.join(out_dir, "*.results.*.xml.bz2")
    ):
        opener = bz2.open if path.endswith(".bz2") else open
        with opener(path, "rb") as handle:
            root = ET.parse(handle).getroot()
        for run in root.findall("run"):
            values = {c.get("title"): c.get("value") for c in run.findall("column")}
            coverage = values.get("branches_covered") or ""
            coverage = coverage.replace("%", "").strip()
            found[os.path.basename(run.get("name"))] = (values.get("status", ""), coverage)
    return found


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--property", choices=PROPERTIES, required=True)
    parser.add_argument("--tasks", required=True,
                        help="manifest TSV or list of task paths (.c/.i/.yml)")
    parser.add_argument("--suites", required=True)
    parser.add_argument("--sv-benchmarks", required=True,
                        help="the c/ directory of sv-benchmarks")
    parser.add_argument("--out", required=True)
    parser.add_argument("--threads", type=int, default=3)
    parser.add_argument("--validators", default=",".join(VALIDATORS))
    parser.add_argument("benchexec_args", nargs="*",
                        help="after --, extra BenchExec options (e.g. --timelimit 60 for a smoke test)")
    args = parser.parse_args()

    rundefinition = PROPERTIES[args.property]
    suites = os.path.abspath(args.suites)
    out = os.path.abspath(args.out)
    staging = os.path.join(out, "sv-benchmarks")
    tasks = task_files(args, staging)
    print(f"{len(tasks)} tasks, {rundefinition}", flush=True)

    per_validator = {}
    for name in args.validators.split(","):
        definition = rewrite_definition(name, rundefinition, tasks, suites, staging, out)
        result_dir = os.path.join(out, "results", name)
        os.makedirs(result_dir, exist_ok=True)
        if run_benchexec(definition, VALIDATORS[name], result_dir, staging, args) != 0:
            print(f"warning: benchexec exited non-zero for {name}", flush=True)
        per_validator[name] = read_results(result_dir)

    names = list(per_validator)
    with open(os.path.join(out, "scores.csv"), "w", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(["task"] + [f"{n}:{c}" for n in names for c in ("status", "coverage")]
                        + ["score", "best_validator"])
        for task in tasks:
            key = os.path.basename(task)
            row, best, best_by = [key], 0.0, ""
            for name in names:
                status, coverage = per_validator[name].get(key, ("MISSING", ""))
                row += [status, coverage]
                if args.property == "cover-error":
                    value = 1.0 if status == "true" else 0.0
                else:
                    try:
                        value = float(coverage) / 100
                    except ValueError:
                        value = 0.0
                if value > best:
                    best, best_by = value, name
            writer.writerow(row + [f"{best:.4f}", best_by])
    # BenchExec runs as root in the container; hand the results back.
    owner = os.environ.get("HOST_OWNER")
    if owner:
        subprocess.call(["chown", "-R", owner, out])
    print(f"scores: {os.path.join(out, 'scores.csv')}")


if __name__ == "__main__":
    main()
