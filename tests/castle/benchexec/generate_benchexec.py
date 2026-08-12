#!/usr/bin/env python3
"""Generate BenchExec YML task files, SET files, and XML for CASTLE C250."""

import json
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
JSON_PATH = os.path.join(SCRIPT_DIR, "..", "CASTLE-Benchmark", "datasets", "CASTLE-C250.min.json")
TASKS_DIR = os.path.join(SCRIPT_DIR, "tasks")
SETS_DIR = os.path.join(SCRIPT_DIR, "sets")
PROPS_DIR = os.path.join(SCRIPT_DIR, "properties")
XML_PATH = os.path.join(SCRIPT_DIR, "map2check_castle.xml")

CWE_PROPERTY = {
    125: "valid-memsafety.prp",
    134: "unreach-call.prp",
    190: "no-overflow.prp",
    253: "unreach-call.prp",
    327: "unreach-call.prp",
    362: "unreach-call.prp",
    369: "unreach-call.prp",
    401: "valid-memcleanup.prp",
    415: "valid-memsafety.prp",
    416: "valid-memsafety.prp",
    476: "valid-memsafety.prp",
    522: "unreach-call.prp",
    617: "unreach-call.prp",
    628: "unreach-call.prp",
    674: "unreach-call.prp",
    761: "valid-memsafety.prp",
    770: "unreach-call.prp",
    787: "valid-memsafety.prp",
    798: "unreach-call.prp",
    822: "valid-memsafety.prp",
    835: "unreach-call.prp",
    843: "valid-memsafety.prp",
    22: "unreach-call.prp",
    78: "unreach-call.prp",
    89: "unreach-call.prp",
}

SET_GROUPS = {
    "castle-memsafety": [
        125, 415, 416, 476, 761, 787, 822, 843,
    ],
    "castle-overflow": [190],
    "castle-memcleanup": [401],
    "castle-reachability": [
        134, 253, 362, 369, 617, 628, 674, 770, 835,
        22, 78, 89, 327, 522, 798,
    ],
}

# Load JSON
with open(JSON_PATH) as f:
    data = json.load(f)

tests = data["tests"]
print(f"Loaded {len(tests)} tests from CASTLE-C250.min.json")

# Clear and recreate task directories
for d in [TASKS_DIR, SETS_DIR]:
    os.makedirs(d, exist_ok=True)
    for fname in os.listdir(d):
        os.remove(os.path.join(d, fname))

# CWE → list of test names (for set grouping)
cwe_tests = {cwe: [] for cwe in CWE_PROPERTY}

# Generate YML and SET file content
yml_count = 0
for t in tests:
    name = t["name"]
    cwe = t["cwe"]
    vulnerable = t["vulnerable"]
    test_id = t["id"]

    if cwe not in CWE_PROPERTY:
        cwe = 843  # fallback to memsafety
        print(f"  WARNING: unknown CWE {t['cwe']} for {name}, using CWE-843")

    prop_file = CWE_PROPERTY[cwe]
    expected_verdict = "false" if vulnerable else "true"

    # Path to C file relative to tasks/ directory
    c_file_rel = f"../../CASTLE-Benchmark/datasets/CASTLE-C250/{name}"

    yml_content = f"""format_version: '2.0'
input_files: '{c_file_rel}'

properties:
  - property_file: ../properties/{prop_file}
    expected_verdict: {expected_verdict}
"""

    yml_path = os.path.join(TASKS_DIR, f"CASTLE-{test_id}.yml")
    with open(yml_path, "w") as f:
        f.write(yml_content)
    yml_count += 1
    cwe_tests[cwe].append(f"CASTLE-{test_id}")

print(f"Generated {yml_count} YML task files")

# Generate SET files grouped by verification mode
set_count = 0
for set_name, cwe_list in SET_GROUPS.items():
    lines = []
    for cwe in cwe_list:
        for task_file in cwe_tests.get(cwe, []):
            lines.append(f"../tasks/{task_file}.yml")

    set_path = os.path.join(SETS_DIR, f"{set_name}.set")
    with open(set_path, "w") as f:
        f.write("\n".join(lines) + "\n")
    set_count += 1
    print(f"  {set_name}.set: {len(lines)} tasks")

print(f"Generated {set_count} SET files")

# Map2Check wrapper dir (relative to CASTLE dir inside Docker volume)
# The user mounts the whole repo at e.g. /home/bench/benchexec_files/
# Map2Check binary is in release/ (the CWD when BenchExec runs)

# Generate BenchExec XML
xml_parts = [
    '<?xml version="1.0"?>',
    '<!DOCTYPE benchmark PUBLIC "+//IDN sosy-lab.org//DTD BenchExec benchmark 1.4//EN"',
    '  "http://www.sosy-lab.org/benchexec/benchmark-1.4.dtd">',
    '<benchmark tool="map2check" timelimit="120 s">',
    "",
    '  <rundefinition name="sv-comp19_map2check"></rundefinition>',
    "",
]

BENCHEXEC_BASE = "/home/bench/benchexec_files/tests/castle/benchexec"

for set_name in ["castle-memsafety", "castle-overflow", "castle-memcleanup", "castle-reachability"]:
    prop_file_map = {
        "castle-memsafety": "valid-memsafety.prp",
        "castle-overflow": "no-overflow.prp",
        "castle-memcleanup": "valid-memcleanup.prp",
        "castle-reachability": "unreach-call.prp",
    }
    prop_file = prop_file_map[set_name]
    display_name = set_name.replace("castle-", "CASTLE-")

    xml_parts.append(f'  <tasks name="{display_name}">')
    xml_parts.append(f'    <includesfile>{BENCHEXEC_BASE}/sets/{set_name}.set</includesfile>')
    xml_parts.append(f'    <propertyfile>{BENCHEXEC_BASE}/properties/{prop_file}</propertyfile>')
    xml_parts.append("  </tasks>")
    xml_parts.append("")

xml_parts.append("</benchmark>")

with open(XML_PATH, "w") as f:
    f.write("\n".join(xml_parts))

print(f"\nBenchExec XML written to {XML_PATH}")
print(f"\nTo run:")
print(f"  cd Map2Check")
print(f"  docker run -it --rm -v /sys/fs/cgroup:/sys/fs/cgroup:rw \\")
print(f"    -v $(pwd):/home/bench/benchexec_files:Z hrocha/benchexec \\")
print(f'    /bin/bash -c "cd release && python3 -m benchexec.benchexec --no-container ../tests/castle/benchexec/map2check_castle.xml"')
