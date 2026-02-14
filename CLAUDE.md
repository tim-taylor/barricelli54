# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A C++ implementation of Nils Aall Barricelli's 1954 paper *Esempi Numerici di Processi di Evoluzione*, which describes computational evolution experiments run on the IAS machine at Princeton. The program reproduces all 22 figures from the paper. Each figure uses a specific "norm" (reproduction rule), world size, number of generations, and initial conditions hardcoded in the `init()` function.

## Build & Run

```bash
# Compile (requires g++ with C++20 support)
./compile                    # produces ./barricelli54

# Run (n = figure number 1-22; numbers 23-25 are test cases)
./barricelli54 n             # human-readable aligned output
./barricelli54 -c n          # CSV output

# Convert CSV to PNG image (requires Python 3 + matplotlib + numpy)
python3 csv2img.py [-b] file.csv [label_specs...]
# -b adds grid lines; label_specs: above|below:start_col:end_col:label_text

# Generate Figures 8-11 with paper-matching labels
./gen-figs-with-labels
```

## Architecture

**Single-file C++ program** (`src/barricelli54.cpp`, ~705 lines). No build system beyond the `compile` shell script.

Key concepts:
- **World**: a 1D array (`std::vector<int>`) of size `worldSize`, indexed cyclically (wraps around)
- **Norms** (reproduction rules), in order of complexity:
  - `BASIC` — numbers shift by their own value each generation
  - `SYMBIOTIC` — organisms reproduce by finding a partner number nearby
  - `EXCLUSION` — symbiotic + organisms cannot overwrite occupied cells
  - `CONDITIONAL` — symbiotic + conditional reproduction based on partner type
- **`init(fig)`** — big switch statement mapping figure numbers 1-22 to their parameters (world size, generations, norm, initial world contents)
- **`updateWorld()`** dispatches to `updateBasic/Symbiotic/Exclusion/Conditional` based on current norm
- **`findNearestNumber(i, delta)`** — core helper that searches cyclically for the nearest non-zero number at distance `delta` from position `i`

## Analysis Scripts (output/fig15-reruns/analysis/)

Python scripts for analysing 100 re-runs of Figure 15 with random initial conditions:
- `categorise-run.py X` — classifies a single run by checking if the final row has reached a repeating state (period 1-25)
- `analyse-categories.py` — summarises `run-categories.csv` with counts of distinct values and cycle categories
