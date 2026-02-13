#!/usr/bin/env python3
"""Analyse run-categories.csv, summarising columns 2 (distinct count) and 3 (category)."""

import csv
from collections import defaultdict

with open("run-categories.csv") as f:
    rows = list(csv.reader(f))

# Build mappings: value -> list of run numbers
count_runs = defaultdict(list)  # column 2: distinct value count
cat_runs = defaultdict(list)    # column 3: category

for run, count, cat in rows:
    count_runs[int(count)].append(int(run))
    cat_runs[int(cat)].append(int(run))

for label, mapping in [("Distinct value count", count_runs),
                        ("Category", cat_runs)]:
    print(f"{label}: {len(mapping)} distinct values")
    for val in sorted(mapping):
        runs = mapping[val]
        run_list = ", ".join(str(r) for r in runs)
        print(f"  {val}: {len(runs)} ({run_list})")
    print()
