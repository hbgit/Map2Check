#!/usr/bin/env python3
"""Runs Map2Check through Test-Comp 2026 the way the competition does.

Runs inside the reproduction image (Dockerfile; run.sh starts it). Two steps,
each one a BenchExec run of an official-style benchmark definition:

  generate   the Map2Check tool archive (utils/testcomp-archive/make-archive.sh)
             under bench-defs/map2check.xml: 15 min of CPU time, 15 GB, 4 cores,
             like every Test-Comp 2026 test generator. Each task's test-suite/
             is then zipped into the layout the validators read.

  validate   the five validators of the testcomp26 category structure -- TestCov
             with gcc and clang, formatted and unformatted, and TestCoCa -- from
             their official definitions in bench-defs/, unchanged except for
             the task set and the location of the suites. The scores are merged
             as the competition merges them (competition-scripts,
             prepare_tables/export_adjusted_csvs.py, update_score_tester):
               coverage-branches    the BEST coverage any validator measured;
               coverage-error-call  1 point if ANY validator answers TRUE.

What is rewritten in a definition, and why:
  - the tasks, replaced by --tasks;
  - ../sv-benchmarks/c/ and results-verified/LOGDIR/, replaced by our paths;
  - <require cpuModel>, dropped: it pins the competition's hardware.

As the competition scripts do (execute-runcollection.sh), BenchExec runs from
inside a fresh copy of the tool with --tool-directory . and the overlays
--read-only-dir / --overlay-dir /home/ --overlay-dir ./. Task inputs are
staged into a copy per step, because tools write beside the program (TestCov's
<program>.c.i), and on the competition's VerifierCloud the inputs sit in the
run's own writable directory.

Suites live at <suites>/<rundefinition>/<taskdef_name>/test-suite.zip, e.g.
suites/Test-Comp26_coverage-branches/AllInterval-011.yml/test-suite.zip
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
import zipfile

import yaml

HERE = os.path.dirname(os.path.abspath(__file__))
DEFS = os.path.join(HERE, "bench-defs")
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))

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


# --- shared -------------------------------------------------------------------

def stage_tasks(task_list, sv_benchmarks, staging):
    """Copies each task's inputs -- the .yml, the files it names, and the
    property files -- into `staging`, keeping their paths relative to
    sv-benchmarks. Returns the staged .yml paths.

    `task_list` holds manifest rows (category<TAB>program<TAB>...) or bare paths
    to a .c, .i or .yml."""
    shutil.rmtree(staging, ignore_errors=True)
    shutil.copytree(os.path.join(sv_benchmarks, "properties"),
                    os.path.join(staging, "properties"))
    tasks = []
    with open(task_list) as listing:
        for line in listing:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            path = line.split("\t")[1] if "\t" in line else line
            stem, _ = os.path.splitext(path)
            yml = os.path.join(sv_benchmarks, stem + ".yml")
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
            # Some tasks carry property files of their own (eca-rers2018/), named
            # relative to the task definition. BenchExec resolves every property
            # a definition lists, so one missing file aborts the whole run -- and
            # with it every task's result.
            local_properties = os.path.join(os.path.dirname(yml), "properties")
            if os.path.isdir(local_properties):
                shutil.copytree(local_properties,
                                os.path.join(target_dir, "properties"),
                                dirs_exist_ok=True)
            tasks.append(os.path.join(target_dir, os.path.basename(yml)))
    return tasks


def rewrite_definition(source, rundefinition, tasks, staging, suites, out_path):
    """`source`, narrowed to one run definition, our tasks and our paths."""
    tree = ET.parse(source)
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
                element.text = element.text.replace("../sv-benchmarks/c/", staging + "/")
        for old in run.findall("tasks"):
            run.remove(old)
        group = ET.SubElement(run, "tasks", name="map2check")
        for task in tasks:
            ET.SubElement(group, "include").text = task
    if not kept:
        sys.exit(f"{source}: no rundefinition {rundefinition}")
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    tree.write(out_path, xml_declaration=True, encoding="utf-8")
    return out_path


def run_benchexec(definition, tool_dir, out_dir, staging, args, env=None):
    """BenchExec from inside `tool_dir`, with the competition's overlays."""
    command = [
        "benchexec", definition,
        "--tool-directory", ".",
        "--outputpath", out_dir + "/",
        "--numOfThreads", str(args.threads),
        # Results are read once and the whole output directory is removed after
        # each validator (see validate); compression is BenchExec's default.
        "--read-only-dir", "/", "--overlay-dir", "/home/", "--overlay-dir", "./",
        # An overlay, not full access: what one run writes beside a program
        # must not reach the next.
        "--overlay-dir", staging,
    ]
    command += args.benchexec_args
    print("+", " ".join(command), flush=True)
    return subprocess.call(command, cwd=tool_dir, env=env)


def read_results(out_dir):
    """task -> (status, coverage %) from the BenchExec result XMLs in out_dir."""
    found = {}
    paths = glob.glob(os.path.join(out_dir, "*.results.*.xml"))
    paths += glob.glob(os.path.join(out_dir, "*.results.*.xml.bz2"))
    for path in paths:
        opener = bz2.open if path.endswith(".bz2") else open
        with opener(path, "rb") as handle:
            root = ET.parse(handle).getroot()
        for run in root.findall("run"):
            values = {c.get("title"): c.get("value") for c in run.findall("column")}
            coverage = (values.get("branches_covered") or "").replace("%", "").strip()
            found[os.path.basename(run.get("name"))] = (values.get("status", ""), coverage)
    return found


def hand_back(path):
    """BenchExec runs as root in the container; give the results back."""
    owner = os.environ.get("HOST_OWNER")
    if owner:
        subprocess.call(["chown", "-R", owner, path])


# --- generate -----------------------------------------------------------------

def generate(args):
    rundefinition = PROPERTIES[args.property]
    out = os.path.abspath(args.out)
    os.makedirs(out, exist_ok=True)
    staging = os.path.join(out, "sv-benchmarks")
    tasks = stage_tasks(args.tasks, os.path.abspath(args.sv_benchmarks), staging)
    print(f"{len(tasks)} tasks, {rundefinition}", flush=True)

    definition = rewrite_definition(os.path.join(DEFS, "map2check.xml"), rundefinition,
                                    tasks, staging, "", os.path.join(out, "defs", "map2check.xml"))

    # A fresh copy of the tool, unpacked as the competition unpacks archives.
    tool_root = os.path.join(out, "tool")
    shutil.rmtree(tool_root, ignore_errors=True)
    subprocess.check_call(["unzip", "-q", os.path.abspath(args.archive), "-d", tool_root])
    tool_dir = os.path.join(tool_root, "map2check")

    # The tool-info module lives in the repository, not in BenchExec.
    env = dict(os.environ)
    env["PYTHONPATH"] = os.path.join(REPO, "utils") + os.pathsep + env.get("PYTHONPATH", "")
    result_dir = os.path.join(out, "results")
    os.makedirs(result_dir, exist_ok=True)
    code = run_benchexec(definition, tool_dir, result_dir, staging, args, env=env)

    # <result>/<benchmark>.<date>.files/<rundefinition>/<task>.yml/test-suite/
    suites = os.path.abspath(args.suites)
    count = 0
    for suite_dir in glob.glob(os.path.join(result_dir, "*.files", rundefinition, "*", "test-suite")):
        taskdef = os.path.basename(os.path.dirname(suite_dir))
        target = os.path.join(suites, rundefinition, taskdef)
        os.makedirs(target, exist_ok=True)
        with zipfile.ZipFile(os.path.join(target, "test-suite.zip"), "w",
                             zipfile.ZIP_DEFLATED) as archive:
            for name in sorted(os.listdir(suite_dir)):
                archive.write(os.path.join(suite_dir, name), name)
        count += 1
    shutil.rmtree(tool_root, ignore_errors=True)
    # The suites are zipped out; the tool tree, the staged sv-benchmarks and the
    # BenchExec output (whose .files hold every test-suite) are scratch.
    shutil.rmtree(result_dir, ignore_errors=True)
    shutil.rmtree(staging, ignore_errors=True)
    hand_back(out)
    hand_back(suites)
    print(f"{count} suites in {os.path.join(suites, rundefinition)}")
    return code


# --- validate -----------------------------------------------------------------

def validate(args):
    rundefinition = PROPERTIES[args.property]
    suites = os.path.abspath(args.suites)
    out = os.path.abspath(args.out)
    staging = os.path.join(out, "sv-benchmarks")
    tasks = stage_tasks(args.tasks, os.path.abspath(args.sv_benchmarks), staging)
    print(f"{len(tasks)} tasks, {rundefinition}", flush=True)

    per_validator = {}
    for name in args.validators.split(","):
        definition = rewrite_definition(os.path.join(DEFS, name + ".xml"), rundefinition,
                                        tasks, staging, suites,
                                        os.path.join(out, "defs", name + ".xml"))
        result_dir = os.path.join(out, "results", name)
        os.makedirs(result_dir, exist_ok=True)
        # A fresh copy of the validator: TestCov's --format writes beside itself.
        work = os.path.join(out, "tool")
        shutil.rmtree(work, ignore_errors=True)
        shutil.copytree(VALIDATORS[name], work, symlinks=True)
        if run_benchexec(definition, work, result_dir, staging, args) != 0:
            print(f"warning: benchexec exited non-zero for {name}", flush=True)
        shutil.rmtree(work, ignore_errors=True)
        per_validator[name] = read_results(result_dir)
        # Keep the scratch when debugging (MAP2CHECK_KEEP_RESULTS=1); otherwise
        # drop it, or a few-thousand-task corpus fills the disk.
        if not os.environ.get("MAP2CHECK_KEEP_RESULTS"):
            shutil.rmtree(result_dir, ignore_errors=True)
    if not os.environ.get("MAP2CHECK_KEEP_RESULTS"):
        shutil.rmtree(staging, ignore_errors=True)

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
    hand_back(out)
    print(f"scores: {os.path.join(out, 'scores.csv')}")
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="step", required=True)
    for step in ("generate", "validate"):
        p = sub.add_parser(step)
        p.add_argument("--property", choices=PROPERTIES, required=True)
        p.add_argument("--tasks", required=True,
                       help="manifest TSV or list of task paths (.c/.i/.yml)")
        p.add_argument("--suites", required=True)
        p.add_argument("--sv-benchmarks", required=True,
                       help="the c/ directory of sv-benchmarks")
        p.add_argument("--out", required=True)
        p.add_argument("--threads", type=int, default=3)
        p.add_argument("benchexec_args", nargs="*",
                       help="after --, extra BenchExec options "
                            "(e.g. --timelimit 300s, a deliberate deviation)")
        if step == "generate":
            p.add_argument("--archive", required=True, help="the Map2Check tool archive")
        else:
            p.add_argument("--validators", default=",".join(VALIDATORS))
    args = parser.parse_args()
    sys.exit(generate(args) if args.step == "generate" else validate(args))


if __name__ == "__main__":
    main()
