#!/usr/bin/env python3
"""Builds a stratified Test-Comp corpus manifest from sv-benchmarks.

Stratified rather than random, and by SUBCATEGORY rather than over the whole
pool, for the same reason the Juliet runs are sharded by CWE: a uniform sample
of 33k tasks is dominated by whichever families happen to be largest (Floats
and ECA alone are 2.6k), and a result computed over it says more about the
benchmark's shape than about the tool. Fixing a quota per subcategory makes
"Map2Check does badly on Recursive" a statement the numbers can support.

Selection inside a subcategory is deterministic and NESTED: the candidates are
sorted by path, then ordered by recursive bisection, and the first N are taken.
No RNG, no seed to record, and two properties fall out that a plain stride does
not give:

  * every prefix is evenly spread over the family, so a run cut short by its
    deadline still sampled the whole range rather than one alphabetical corner;
  * a larger quota is a SUPERSET of a smaller one. That matters as soon as the
    corpus grows: with an even stride, raising the quota from 40 to 150
    preserved only 12 of 40 tasks in Arrays and 10 of 40 in Loops, Floats and
    ECA -- 70% overall, but the big families almost entirely resampled, so the
    measurements already taken could not be reused.

The manifest itself is written round-robin across subcategories, not grouped by
them. A run that stops at its deadline stops after a balanced prefix; grouped
by category it would have finished Arrays and never reached XCSP, which throws
away the stratification the sampling exists to provide.

Usage:
  build_corpus.py --property cover-error   --per-category 40 --out manifest.tsv
  build_corpus.py --property cover-branches --per-category 40 --out manifest.tsv
"""

import argparse
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
BENCH = os.path.join(HERE, "bench", "sv-benchmarks", "c")

# The subcategories, and why these. They span the shapes a test generator is
# actually challenged by -- array reasoning, bit-level operations, control-flow
# depth, heap shape, loop invariants, recursion, floating point, event-condition
# -action tables, feature interaction -- rather than the largest directories.
# Concurrency and the Termination sets are excluded: Map2Check declares no
# support for either, and running them would report zeros that mean nothing.
CATEGORIES = [
    "Arrays",
    "BitVectors",
    "ControlFlow",
    "Heap",
    "Loops",
    "Recursive",
    "Sequentialized",
    "Floats",
    "ECA",
    "ProductLines",
    "XCSP",
    "LinkedLists",
]

PROPERTY_FILE = {
    "cover-error": "coverage-error-call.prp",
    "cover-branches": "coverage-branches.prp",
}


def expand_set(name):
    """Every task definition a .set file names, in sorted order."""
    import glob

    paths = []
    set_path = os.path.join(BENCH, name + ".set")
    with open(set_path) as handle:
        for line in handle:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            paths.extend(glob.glob(os.path.join(BENCH, line)))
    return sorted(set(paths))


def task_info(yml_path, wanted_property):
    """Reads the fields the runner needs, or None if the task does not apply.

    Parsed by hand rather than with PyYAML: the fields wanted are three flat
    scalars and one list of maps, the format is stable and version-stamped, and
    a dependency here would have to exist inside the runner container too.
    """
    input_file = None
    data_model = "ILP32"  # the sv-benchmarks default when unstated
    has_property = False
    expected = ""
    in_properties = False
    current_property = None

    with open(yml_path, errors="replace") as handle:
        for raw in handle:
            line = raw.rstrip("\n")
            stripped = line.strip()
            if stripped.startswith("input_files:"):
                input_file = stripped.split(":", 1)[1].strip().strip("'\"")
            elif stripped.startswith("data_model:"):
                data_model = stripped.split(":", 1)[1].strip().strip("'\"")
            elif stripped.startswith("properties:"):
                in_properties = True
            elif in_properties and stripped.startswith("- property_file:"):
                current_property = stripped.split(":", 1)[1].strip()
            elif in_properties and stripped.startswith("expected_verdict:"):
                if current_property and current_property.endswith(
                        "unreach-call.prp"):
                    expected = stripped.split(":", 1)[1].strip()
            if current_property and current_property.endswith(
                    PROPERTY_FILE[wanted_property]):
                has_property = True

    if input_file is None or not has_property:
        return None
    # A task definition may name several input files; Map2Check takes one.
    if " " in input_file or "," in input_file:
        return None
    program = os.path.join(os.path.dirname(yml_path), input_file)
    if not os.path.isfile(program):
        return None
    return program, data_model, expected


def spread_order(items):
    """Deterministic permutation whose every prefix is evenly spread.

    Breadth-first recursive bisection: the middle element first, then the
    middles of the two halves, then of the four quarters, and so on. Two
    properties follow, and both are why this replaced an even stride.

    Any prefix covers the whole range. Taking the first N of a sorted list
    would give N variations of one program, since task names inside a family
    are ordered; taking a bisection prefix gives N spread across it.

    Prefixes nest. Because the order does not depend on N, the first 40 of this
    sequence are the first 40 of any longer one -- so raising the quota ADDS
    tasks instead of substituting them, and measurements already taken at the
    smaller quota stay valid. An even stride does not have that property: at
    the real pool sizes here, going from 40 to 150 kept 12 of 40 tasks in
    Arrays and 10 of 40 in Loops, Floats and ECA.
    """
    from collections import deque

    order = []
    queue = deque([(0, len(items) - 1)])
    while queue:
        low, high = queue.popleft()
        if low > high:
            continue
        middle = (low + high) // 2
        order.append(items[middle])
        queue.append((low, middle - 1))
        queue.append((middle + 1, high))
    return order


def nested_sample(items, quota):
    """The first `quota` of the spread order."""
    if quota <= 0:
        return []
    return spread_order(items)[:quota]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--property", choices=sorted(PROPERTY_FILE),
                        required=True)
    parser.add_argument("--per-category", type=int, default=40)
    parser.add_argument("--out", required=True)
    args = parser.parse_args()

    if not os.path.isdir(BENCH):
        sys.exit("sv-benchmarks not found at %s -- run fetch-benchmarks.sh"
                 % BENCH)

    per_category = {}
    summary = []
    for category in CATEGORIES:
        applicable = []
        for yml in expand_set(category):
            info = task_info(yml, args.property)
            if info is not None:
                applicable.append((yml,) + info)
        chosen = nested_sample(applicable, args.per_category)
        summary.append((category, len(applicable), len(chosen)))
        per_category[category] = [
            (category, os.path.relpath(program, BENCH), data_model,
             expected or "unknown")
            for yml, program, data_model, expected in chosen
        ]

    # Round-robin, not grouped. A run that stops at its deadline should stop
    # after a balanced prefix of every subcategory; written grouped, it would
    # finish Arrays and never reach XCSP.
    rows = []
    depth = max((len(v) for v in per_category.values()), default=0)
    for index in range(depth):
        for category in CATEGORIES:
            bucket = per_category.get(category, [])
            if index < len(bucket):
                rows.append(bucket[index])

    with open(args.out, "w") as handle:
        handle.write("# category\tprogram\tdata_model\texpected_unreach\n")
        for row in rows:
            handle.write("\t".join(row) + "\n")

    for category, available, taken in summary:
        print("  %-16s %5d applicable, %3d taken" % (category, available, taken))
    print("  %-16s %5d tasks written to %s" % ("TOTAL", len(rows), args.out))


if __name__ == "__main__":
    main()
