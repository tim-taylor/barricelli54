#!/usr/bin/env python3
import sys

HELP = """\
Usage: categorise-run.py X

Categorise a fig15 run by comparing the final line of
fig15-random-init-X.csv with earlier lines.

Arguments:
  X   Run number (1-100)

Output: X,N,C
  X = run number
  N = count of distinct values in the final line
  C = category (1-25 if the final line matches the line N rows
      before it; 0 if no match found within 25 rows)"""

if len(sys.argv) != 2 or sys.argv[1] in ("-h", "--help"):
    print(HELP)
    sys.exit(0 if sys.argv[1:] in (["-h"], ["--help"]) else 1)

try:
    x = int(sys.argv[1])
except ValueError:
    print(f"Error: expected a number, got '{sys.argv[1]}'", file=sys.stderr)
    print(HELP)
    sys.exit(1)

filename = f"fig15-random-init-{x}.csv"

try:
    with open(filename) as f:
        lines = f.readlines()
except FileNotFoundError:
    print(f"Error: file not found: {filename}", file=sys.stderr)
    print(HELP)
    sys.exit(1)

last_line = lines[-1]
num_different = len(set(last_line.strip().split(",")))

category = 0
for n in range(1, 26):
    idx = -(n + 1)
    if len(lines) >= n + 1 and lines[idx] == last_line:
        category = n
        break

print(f"{x},{num_different},{category}")
