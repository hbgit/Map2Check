#!/usr/bin/env python3
"""Builds a PROPORTIONAL stratified Test-Comp corpus from the official
Test-Comp 2026 definitions.

`build_corpus.py` stratifies by a FIXED quota per category (equal
representation). This script does the other thing the rerun asks for: the quota
of each category is proportional to how many applicable tasks it has, so the
corpus mirrors the benchmark's shape while still guaranteeing every category a
seat.

The categories are not a hand-kept list here: they are read from the official
benchmark definition (the same file the validators use), including each
category's `<includesfile>`/`<excludesfile>`, so the corpus tracks the
competition's own task structure. A category that expands to no applicable task
gets no seat.

Selection inside a category is the deterministic, nested bisection order of
`build_corpus.spread_order`, and the manifest is written round-robin, so a run
cut short by its deadline stays balanced.

Usage:
  build_corpus_official.py --property cover-error    --total 0    --out ce.tsv
  build_corpus_official.py --property cover-branches --total 3500 --out cb.tsv

`--total 0` means "all applicable tasks" (no sampling).
"""

import argparse
import glob
import os
import sys
import xml.etree.ElementTree as ET

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import build_corpus as bc  # noqa: E402

OFFICIAL_DEF = os.path.join(HERE, "official", "bench-defs",
                            "testcov-validate-test-suites-gcc-unformatted.xml")
DEF_DIR = os.path.dirname(OFFICIAL_DEF)

RUNDEF = {
    "cover-error": "Test-Comp26_coverage-error-call",
    "cover-branches": "Test-Comp26_coverage-branches",
}


def category_name(rawname):
    return rawname.rsplit(".", 1)[-1]


def set_file(entry, include):
    """Where a definition's includesfile/excludesfile lives.

    Includes are all under sv-benchmarks/c.  Excludes are either there
    (DeviceDriversLinux64Large.set) or beside the definition (the ECA one),
    referenced relative to it.
    """
    entry = entry.strip()
    if include or entry.startswith("../sv-benchmarks/c/"):
        return os.path.join(bc.BENCH, os.path.basename(entry))
    return os.path.join(DEF_DIR, entry)


def read_set(path):
    """Every .yml a set file names, resolved relative to sv-benchmarks/c."""
    paths = set()
    if not os.path.isfile(path):
        print("   (missing set file: %s)" % path, file=sys.stderr)
        return paths
    with open(path) as handle:
        for line in handle:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            for hit in glob.glob(os.path.join(bc.BENCH, line)):
                paths.add(os.path.realpath(hit))
    return paths


def applicable_categories(rundefinition, property_name):
    """category -> applicable task tuples, in the definition's own order."""
    root = ET.parse(OFFICIAL_DEF).getroot()
    run = next((r for r in root.findall("rundefinition")
                if r.get("name") == rundefinition), None)
    if run is None:
        sys.exit("no rundefinition %s in %s" % (rundefinition, OFFICIAL_DEF))
    pools = {}
    for tasks in run.findall("tasks"):
        category = category_name(tasks.get("name"))
        included = set()
        for e in tasks.findall("includesfile"):
            included |= read_set(set_file(e.text, True))
        excluded = set()
        for e in tasks.findall("excludesfile"):
            excluded |= read_set(set_file(e.text, False))
        rows = []
        for yml in sorted(included - excluded):
            info = bc.task_info(yml, property_name)
            if info is not None:
                rows.append((yml,) + info)
        if rows:
            pools[category] = rows
    return pools


def proportional_alloc(sizes, total):
    """Quota per category: proportional to size, at least 1, at most size.

    Largest-remainder rounding, so the quotas add up to `total` (or to the whole
    pool when that is smaller).
    """
    pool = sum(sizes.values())
    target = pool if total <= 0 else min(total, pool)
    categories = list(sizes)
    if target <= 0:
        return {c: 0 for c in categories}
    if target < len(categories):
        # Fewer seats than categories: keep the largest categories.
        keep = set(sorted(categories, key=lambda c: (-sizes[c], c))[:target])
        return {c: (1 if c in keep else 0) for c in categories}

    ideal = {c: target * sizes[c] / pool for c in categories}
    quotas = {}
    for c in categories:
        floor = int(ideal[c])
        quotas[c] = min(sizes[c], max(1, floor))
    short = target - sum(quotas.values())

    # Largest fractional remainder takes the surplus, smallest gives back the
    # overshoot, never below one seat and never above the pool.
    if short != 0:
        if short > 0:
            order = sorted(categories,
                           key=lambda c: (-(ideal[c] - int(ideal[c])), -sizes[c], c))
            move = 1
        else:
            order = sorted(categories,
                           key=lambda c: (ideal[c] - int(ideal[c]), sizes[c], c))
            move = -1
        guard = 0
        limit = len(order) * (max(sizes.values()) + 2)
        while short != 0 and guard < limit:
            for c in order:
                if short == 0:
                    break
                if move > 0 and quotas[c] < sizes[c]:
                    quotas[c] += 1
                    short -= 1
                elif move < 0 and quotas[c] > 1:
                    quotas[c] -= 1
                    short += 1
                guard += 1
        if short != 0:
            sys.exit("proportional_alloc: could not reach target %d (off by %d)"
                     % (target, short))
    return quotas


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--property", choices=sorted(RUNDEF), required=True)
    parser.add_argument("--total", type=int, default=0,
                        help="target corpus size; 0 = all applicable")
    parser.add_argument("--out", required=True)
    args = parser.parse_args()

    if not os.path.isdir(bc.BENCH):
        sys.exit("sv-benchmarks not found at %s" % bc.BENCH)
    if not os.path.isfile(OFFICIAL_DEF):
        sys.exit("official definition not found at %s" % OFFICIAL_DEF)

    pools = applicable_categories(RUNDEF[args.property], args.property)
    sizes = {c: len(v) for c, v in pools.items()}
    quotas = proportional_alloc(sizes, args.total)

    per_category = {}
    print("%-45s %6s %6s" % ("category", "pool", "chosen"))
    total = 0
    for category, rows in pools.items():
        chosen = bc.spread_order(rows)[:quotas[category]]
        per_category[category] = [
            (category, os.path.relpath(program, bc.BENCH), data_model,
             expected or "unknown")
            for yml, program, data_model, expected, subproperty in chosen
        ]
        print("%-45s %6d %6d" % (category, sizes[category], len(chosen)))
        total += len(chosen)

    depth = max((len(v) for v in per_category.values()), default=0)
    rows = []
    for i in range(depth):
        for category in pools:  # round-robin, the definition's order
            bucket = per_category[category]
            if i < len(bucket):
                rows.append(bucket[i])

    with open(args.out, "w") as handle:
        handle.write("# category\tprogram\tdata_model\texpected_unreach\n")
        for row in rows:
            handle.write("\t".join(row) + "\n")
    print("total: %d tasks -> %s" % (len(rows), args.out))


if __name__ == "__main__":
    main()
