#!/usr/bin/env python3
"""Gate diferencial do cutover: compara as linhas de base C e Rust (critérios 1-8)."""
import argparse, csv, statistics, sys
from collections import Counter, defaultdict

KEY = ("task", "prp", "gen")
FIELDS = ("verdict", "tag", "line", "function", "witness")


def load(path):
    groups = defaultdict(list)
    with open(path, newline="") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            groups[tuple(row[k] for k in KEY)].append(row)
    return groups


def mode(rows, field):
    return Counter(r[field] for r in rows).most_common(1)[0][0]


def median_num(rows, field):
    vals = [float(r[field]) for r in rows if r[field] not in ("-", "")]
    return statistics.median(vals) if vals else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("c"); ap.add_argument("rust")
    ap.add_argument("--accepted", required=True)
    ap.add_argument("--max-wall", type=float, default=1.10)
    ap.add_argument("--max-rss", type=float, default=1.36)
    a = ap.parse_args()
    c, r = load(a.c), load(a.rust)
    accepted = set()
    with open(a.accepted, newline="") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            accepted.add((row["task"], row["prp"], row["gen"], row["field"], row["c_value"], row["rust_value"]))

    failures, wall_ratios, rss_ratios = [], [], []
    timeouts = Counter()
    for key in sorted(set(c) | set(r)):
        if key not in r or key not in c:
            failures.append(f"| {' '.join(key)} | missing | {key in c} | {key in r} |")
            continue
        for field in FIELDS:
            cv, rv = mode(c[key], field), mode(r[key], field)
            if cv != rv and (*key, field, cv, rv) not in accepted:
                failures.append(f"| {' '.join(key)} | {field} | {cv} | {rv} |")
        for side, rows in (("c", c[key]), ("rust", r[key])):
            if mode(rows, "verdict") == "TIMEOUT":
                timeouts[(key[2], side)] += 1
        cw, rw = median_num(c[key], "wall_s"), median_num(r[key], "wall_s")
        if cw and rw:
            wall_ratios.append(rw / cw)
        cm, rm = median_num(c[key], "maxrss_kb"), median_num(r[key], "maxrss_kb")
        if cm and rm:
            rss_ratios.append(rm / cm)

    for gen in {k[2] for k in c}:
        if timeouts[(gen, "rust")] > timeouts[(gen, "c")]:
            failures.append(f"| gen={gen} | TIMEOUT count | {timeouts[(gen, 'c')]} | {timeouts[(gen, 'rust')]} |")
    wall = statistics.median(wall_ratios) if wall_ratios else 1.0
    rss = statistics.median(rss_ratios) if rss_ratios else 1.0
    if wall > a.max_wall:
        failures.append(f"| all | median wall ratio | <= {a.max_wall} | {wall:.3f} |")
    if rss > a.max_rss:
        failures.append(f"| all | median rss ratio | <= {a.max_rss} | {rss:.3f} |")

    print(f"median wall ratio: {wall:.3f}  median rss ratio: {rss:.3f}\n")
    if failures:
        print("| key | field | c | rust |\n|---|---|---|---|")
        print("\n".join(failures))
        return 1
    print("gate: PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
