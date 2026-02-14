#!/usr/bin/env python3
"""
Classify fig15 reruns by the dynamics shown in the last 10 lines of each CSV.

Categories:
  STATIC  - all of the last 10 lines are identical
  CYCLE   - short exact cycle (period 2 or 3)
  GLIDER  - simple localised pattern gliding through a repeating background
  COMPLEX - anything else

Usage: classify-dynamics.py file1.csv [file2.csv ...]

Output: one line per file: filename,category

Detection method for GLIDER:
  Compare each line with the line D rows earlier (for D = 1, 2, 3).
  In a periodic background, most cells repeat with some small period D,
  so lines D apart will be nearly identical.  A glider causes a small
  number of cells to differ.  If the best (minimum) such diff count
  across D = 1..3 is small (<= GLIDER_THRESHOLD) for all pairs in the
  last 10 lines, the run is classified as GLIDER.

Written by Claude Code and Tim Taylor, 13 Feb 2026
"""

import sys
import os
from collections import Counter

GLIDER_THRESHOLD = 15   # max differing cells (at best period) to count as GLIDER
GLIDER_MAX_LOOKBACK = 6 # max period to check for glider detection


def parse_line(line_str):
    """Parse a CSV line into a list of string tokens."""
    return [v.strip() for v in line_str.strip().split(",")]


def max_diffs_at_period(lines, period):
    """Max number of differing cells between lines[i] and lines[i-period]."""
    mx = 0
    for i in range(period, len(lines)):
        d = sum(1 for a, b in zip(lines[i], lines[i - period]) if a != b)
        mx = max(mx, d)
    return mx


def classify(filepath):
    with open(filepath) as f:
        lines = f.readlines()

    last = [parse_line(l) for l in lines[-10:]]

    # STATIC: all 10 lines identical
    if all(last[i] == last[0] for i in range(1, 10)):
        return "STATIC"

    # CYCLE D=2
    if all(last[i] == last[i - 2] for i in range(2, 10)):
        return "CYCLE"

    # CYCLE D=3
    if all(last[i] == last[i - 3] for i in range(3, 10)):
        return "CYCLE"

    # GLIDER: find the period (1, 2, or 3) that gives the fewest diffs,
    # then check whether the max diff count is small enough
    best_diff = min(max_diffs_at_period(last, d) for d in range(1, GLIDER_MAX_LOOKBACK + 1))
    if best_diff <= GLIDER_THRESHOLD:
        return "GLIDER"

    return "COMPLEX"


def main():
    if len(sys.argv) < 2:
        print("Usage: classify-dynamics.py file1.csv [file2.csv ...]", file=sys.stderr)
        sys.exit(1)

    files = sys.argv[1:]

    counts = Counter()
    for f in files:
        cat = classify(f)
        counts[cat] += 1
        print(f"{os.path.basename(f)},{cat}")

    # Print summary to stderr
    print(file=sys.stderr)
    print(f"Total: {len(files)} runs", file=sys.stderr)
    for cat in ["STATIC", "CYCLE", "GLIDER", "COMPLEX"]:
        if counts[cat]:
            print(f"  {cat}: {counts[cat]}", file=sys.stderr)


if __name__ == "__main__":
    main()
