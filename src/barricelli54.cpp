// barricelli54
//
// An implementation of the artificial worlds described by Nils Aall Barricelli
// in his first journal publication about his numerical symbioorganisms,
// published in 1954:
//
//   Nils Aall Barricelli. "Esempi Numerici Di Processi Di Evoluzione",
//     Methodos (6) 45-68, 1954.
//
// An English translation of the paper appears in the Artificial Life journal:
//
//   Nils Aall Barricelli. "Numerical examples of evolutionary processes",
//     Artificial Life (x) xx-xx, 202x. DOI: xyz
//
// This translation is accompanied by a commentary on the paper:
//
//   Tim Taylor, Giulia Bianchi et al. "Lorem Ipsum",
//     Artificial Life (x) xx-xx, 202x. DOI: xyz
//
// TODO fill in citation details in previous lines when known
//
// The program takes a number in the range 1-22 as a command line argument,
// and reproduces the corresponding figure from Barricelli's 1954 paper.
//
// Usage:
//   > barricelli54 [-c] n
// where:
//   n  is a number between 1 and 22 to specify which figure from
//      Barricelli's 1954 paper is to be reproduced
//   -c Produce output in CSV format. If this flag is not specified, the
//      output is space separated and padded so that columns line up
//      vertically
//
// Example compilation command with the g++ compiler:
//   > g++ -std=c++20 -o barricelli54 barricelli54.cpp
//
// Written by: Tim Taylor <https://www.tim-taylor.com>
// First release: 10 July 2025
// Last update: 10 July 2025
//
// GitHub repository: https://github.com/tim-taylor/barricelli54
//
// This program is distributed under the GNU GPLv3 license. For full
// information, see the LICENSE file included in the base directory of
// this distribution.


#include <iostream>
#include <vector>
#include <format> // from C++20
#include <string>
#include <cstring>
#include <cassert>
#include <random>
#include <algorithm>

enum class Norm {
    BASIC,
    SYMBIOTIC,
    EXCLUSION,
    CONDITIONAL
};

struct FindResult {
    int pos;
    int num;
};

const int NUM_RULES = 25;
const int X_MARK = 99999;

int worldSize = 10;
int numGens = 10;
Norm norm = Norm::BASIC;
std::vector<int> world;
std::vector<int> nextWorld;

bool printCSV = false;
bool debug = false;

void printUsageAndExit(const std::string& progname, int rc);
int  parseFigNumberOrExit(int argc, char** argv);
std::string getNormName();
void init(int fig);
void initWorld(std::vector<int> initlist);
void printWorld();
void updateWorld();
void flipWorlds();
void updateBasic();
void updateSymbiotic();
void reproduceSymbiotic(int i, int j, int level);
void updateExclusion();
void reproduceExclusion(int i, int j, int level);
void updateConditional();
void reproduceConditional(int i, int j, int level);
FindResult findNearestNumber(int i, int delta);

/********************************************************** */

int main(int argc, char** argv)
{
    int fig = parseFigNumberOrExit(argc, argv);

    init(fig);

    if (!printCSV) {
        std::cout << std::format("Figure {}: {} reproduction for {} generations with universe size {}",
            fig, getNormName(), numGens, worldSize)
            << std::endl << std::endl;
    }

    printWorld();
    for (int i=1; i<numGens; ++i) {
        updateWorld();
        printWorld();
    }

    if (!printCSV) {
        std::cout << std::endl;
    }

    return 0;
}


int parseFigNumberOrExit(int argc, char** argv)
{
    std::string progname{ argv[0] };
    std::size_t pos = progname.find_last_of("//");
    if (pos != std::string::npos && pos < progname.size() - 1) {
        progname = progname.substr(pos+1);
    }

    if (argc < 2 || argc > 3) {
        printUsageAndExit(progname, 1);
    }

    int figIdx = 1;

    if (argc == 3) {
        if (std::strcmp(argv[1], "-c") == 0) {
            printCSV = true;
            figIdx = 2;
        }
        else {
            printUsageAndExit(progname, 1);
        }
    }

    int fig;
    try {
        fig = std::stoi(argv[figIdx]);
        if (fig < 1 || fig > NUM_RULES) {
            printUsageAndExit(progname, 1);
        }
    }
    catch (...) {
        printUsageAndExit(progname, 1);
    }

    return fig;
}


void printUsageAndExit(const std::string& progname, int rc) {
    std::cerr << std::format("Usage: {} [-c] n", progname) << std::endl;
    std::cerr << std::format("  where n is a figure number between 1 and {} (numbers above 22 are test cases)",
        NUM_RULES) << std::endl;
    std::cerr << "        -c specifies CSV output" << std::endl;
    exit(rc);
}


std::string getNormName()
{
    switch (norm) {
        case Norm::BASIC: return "basic";
        case Norm::SYMBIOTIC: return "symbiotic";
        case Norm::EXCLUSION: return "symbiotic+exclusion";
        case Norm::CONDITIONAL: return "symbiotic+conditional";
        default: {
            std::cerr << std::format("Error! Encountered unknown norm {}", (int)norm) << std::endl;
            exit(1);
        }

    }
}


void init(int fig)
{
    switch (fig) {
        case 1: {
            worldSize = 62;
            numGens = 10;
            norm = Norm::BASIC;
            initWorld({4,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,-3,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,2,0,0,0,0,-8,0});
            break;
        }
        case 2: {
            worldSize = 17;
            numGens = 5;
            norm = Norm::SYMBIOTIC;
            initWorld({4});
            break;
        }
        case 3: {
            worldSize = 13;
            numGens = 7;
            norm = Norm::SYMBIOTIC;
            initWorld({0,0,0,0,0,0,0,0,0,0,0,0,-2});
            break;
        }
        case 4: {
            worldSize = 20;
            numGens = 10;
            norm = Norm::SYMBIOTIC;
            initWorld({0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,-3});
            break;
        }
        case 5: {
            worldSize = 11;
            numGens = 5;
            norm = Norm::SYMBIOTIC;
            initWorld({4,0,0,0,0,3,0,0,0,-2});
            break;
        }
        case 6: {
            worldSize = 20;
            numGens = 12;
            norm = Norm::EXCLUSION;
            initWorld({0,0,5,0,0,0,5,0,1,-3,1,-3});
            break;
        }
        case 7: {
            worldSize = 41;
            numGens = 10;
            norm = Norm::EXCLUSION;
            initWorld({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,-3,1,-3,0,-3,1});
            break;
        }
        case 8: {
            worldSize = 59;
            numGens = 17;
            norm = Norm::EXCLUSION;
            initWorld({9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,0,0,0,0,5,-11,1,-3,5,-11,1,-3,5,-11,1,-3,5,-11,1,-3,5,-11,1,-3,5,-11,1,-3});
            break;
        }
        case 9: {
            worldSize = 116;
            numGens = 19;
            norm = Norm::EXCLUSION;
            initWorld({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,-11,1,-3,5,-11,1,-3,5,-11,1,-3,5,-11,1,-3,5,-11,1,-3,5,-11,1,-3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7,9,-11,1,-7});
            break;
        }
        case 10: {
            worldSize = 56;
            numGens = 14;
            norm = Norm::EXCLUSION;
            initWorld({9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,0,0,0,0,5,-11,1,-7,5,-11,1,-7,5,-11,1,-7,5,-11,1,-7,5,-11,1,-7,5,-11,1,-7});
            break;
        }
        case 11: {
            worldSize = 84;
            numGens = 11;
            norm = Norm::EXCLUSION;
            initWorld({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,-11,1,-7,5,-11,1,-7,5,-11,1,-7,5,-11,1,-7,5,-11,1,-7,5,-11,1,-7,0,0,0,0,0,0,0,0,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3,9,-11,1,-3});
            break;
        }
        case 12: {
            worldSize = 12;
            numGens = 7;
            norm = Norm::CONDITIONAL;
            initWorld({3,0,0,0,0,2,0,-4});
            break;
        }
        case 13: {
            worldSize = 6;
            numGens = 2;
            norm = Norm::CONDITIONAL;
            initWorld({0,0,-2,0,0,-5});
            break;
        }
        case 14: {
            worldSize = 8;
            numGens = 2;
            norm = Norm::CONDITIONAL;
            initWorld({3,0,0,2,0,0,0,-4});
            break;
        }
        case 15: {
            worldSize = 83;
            numGens = 101;
            norm = Norm::CONDITIONAL;
            initWorld({0,1,-1,0,0,-1,0,0,-1,0,0,0,1,0,0,1,0,-1,0,0,0,-1,1,1,-1,1,1,1,1,1,0,0,1,-1,1,0,0,-1,-1,0,1,1,-1,0,1,1,1,1,0,-1,-1,-1,0,0,0,-1,0,0,1,-1,0,-1,1,0,-1,0,0,-1,1,0,0,-1,1,-1,1,-1,-1,1,1,0,-1,1,1});
            break;
        }
        case 16: {
            worldSize = 12;
            numGens = 6;
            norm = Norm::CONDITIONAL;
            initWorld({0,0,0,0,0,1,-1});
            break;
        }
        case 17: {
            worldSize = 20;
            numGens = 39;
            norm = Norm::CONDITIONAL;
            initWorld({0,0,0,0,0,1,-2,1,1,-2,0,1,-2,1,1,-2});
            break;
        }
        case 18: {
            worldSize = 21;
            numGens = 20;
            norm = Norm::CONDITIONAL;
            initWorld({0,0,0,0,0,0,1,-1,0,0,1,1,-2,0,1,-2});
            break;
        }
        case 19: {
            worldSize = 21;
            numGens = 3;
            norm = Norm::CONDITIONAL;
            initWorld({0,0,0,0,0,0,0,0,4,0,0,0,-4});
            break;
        }
        case 20: {
            worldSize = 18;
            numGens = 4;
            norm = Norm::CONDITIONAL;
            initWorld({0,0,0,0,0,0,0,0,0,1,-3,1,-3,1,-3});
            break;
        }
        case 21: {
            worldSize = 19;
            numGens = 4;
            norm = Norm::CONDITIONAL;
            initWorld({0,0,0,0,0,0,3,0,0,-3});
            break;
        }
        case 22: {
            worldSize = 20;
            numGens = 5;
            norm = Norm::CONDITIONAL;
            initWorld({0,0,0,0,0,0,0,0,2,2,-2,-2});
            break;
        }
        case 23: {
            // Test case: run Figure 15 for the first 9 generations
            worldSize = 83;
            numGens = 9;
            norm = Norm::CONDITIONAL;
            initWorld({0,1,-1,0,0,-1,0,0,-1,0,0,0,1,0,0,1,0,-1,0,0,0,-1,1,1,-1,1,1,1,1,1,0,0,1,-1,1,0,0,-1,-1,0,1,1,-1,0,1,1,1,1,0,-1,-1,-1,0,0,0,-1,0,0,1,-1,0,-1,1,0,-1,0,0,-1,1,0,0,-1,1,-1,1,-1,-1,1,1,0,-1,1,1});
            break;
        }
        case 24: {
            // Test case: initialise world with row 8 of Figure 15 and run for just one further generation
            worldSize = 83;
            numGens = 2;
            norm = Norm::CONDITIONAL;
            initWorld({1,-1,1,-1,1,-1,1,0,0,1,0,0,1,4,-1,0,0,0,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,1,-1,1,-1,1,0,0,0,-1,1,-1,1,-1,1,-1,1,-1,1,-1,0,1,-1,1,-1,1,3,0,-4,0,-4,0,0,0,1,1,-1,1,-1,1,-1,1,X_MARK,X_MARK,X_MARK,1,-2,1,1,-2,0,1,0});
            break;
        }
        case 25: {
            // Test case: like Barricelli's Figure 15, where he started with a randomly assigned
            // initial state using tosses of two coins => both heads were marked as a 1, both tails
            // as -1, and mixed head/tail as 0. Here we use the same probability distribution but
            // generate a new random state each time the test is called.

            worldSize = 83;
            numGens = 101;
            norm = Norm::CONDITIONAL;

            std::random_device rnd_device;
            std::mt19937 rng { rnd_device() };
            std::uniform_int_distribution<int> dist {-1, 2}; // generate numbers in range [-1,2]

            std::vector<int> state(worldSize);
            std::generate(state.begin(), state.end(), [&](){
                int n = dist(rng);
                return (n==2) ? 0 : n; // returns "-1" 25% of time, "0" 50% and "1" 25%
            });

            initWorld(state);
            break;
        }
        default: {
            std::cerr << std::format("Error: Unexpected figure number encountered ({})!", fig) << std::endl;
            exit(1);
        }
    }
}


void initWorld(std::vector<int> initlist)
{
    if (initlist.size() > worldSize) {
        std::cerr << std::format("Error: Initializer list size ({}) is bigger than world size ({})!", initlist.size(), worldSize) << std::endl;
        exit(1);
    }

    // set world vectors to correct size and initialise elements to 0
    world.resize(worldSize);
    nextWorld.resize(worldSize);

    // set initial world stated according to initlist
    int pos = 0;
    for (int num : initlist) {
        world[pos++] = num;
    }
}


void printWorld()
{
    if (printCSV) {
        for (int i = 0; i < worldSize; ++i) {
            if (world[i] == X_MARK) {
                std::cout << "x";
            }
            else {
                std::cout << world[i];
            }
            if (i < worldSize-1) {
                std::cout << ",";
            }
        }
    }
    else {
        for (int num : world) {
            std::cout << ((num==0) ? "   " : (num==X_MARK) ? "  x" : std::format("{:3}", num));
        }
    }
    std::cout << std::endl;
}


void updateWorld()
{
    switch (norm) {
        case Norm::BASIC: {
            updateBasic();
            break;
        }
        case Norm::SYMBIOTIC: {
            updateSymbiotic();
            break;
        }
        case Norm::EXCLUSION: {
            updateExclusion();
            break;
        }
        case Norm::CONDITIONAL: {
            updateConditional();
            break;
        }
        default: {
            std::cerr << std::format("Encountered unknown norm {}!", (int)norm) << std::endl;
            exit(1);
        }
    }

    flipWorlds();
}


void flipWorlds()
{
    world = std::move(nextWorld);
    nextWorld.assign(worldSize, 0);
}


// Basic update procedure, as described in Section 2 of (Barricelli, 1954)
void updateBasic()
{
    for (int i=0; i<worldSize; ++i) {
        // copy state to same position on next line
        int x = (nextWorld[i] != 0) ? world[i] : 0;         // collision rule for basic reproduction
        nextWorld[i] += (world[i]-x);

        // reproduce state elsewhere on next line
        if (world[i] != 0) {
            int c = i + world[i];
            if (c >= 0 && c < worldSize) {
                int x = (nextWorld[c] != 0) ? world[c] : 0; // collision rule for basic reproduction
                nextWorld[c] += (world[i] - x);
            }
        }
    }
}


// Symbiotic update procedure, as described in Section 4 of (Barricelli, 1954)
void updateSymbiotic()
{
    for (int i=0; i<worldSize; ++i) {
        // if this cell contains a number (not blank(0)), attempt to reproduce it
        if (world[i] != 0) {
            reproduceSymbiotic(i, i+world[i], 1);
        }
    }
}


// Recursive helper function for updateSymbiotic() to implement
// the symbiotic reproduction process.
//
// This function reproduces the number at location i in current world into
// location j in the updated world. It then checks whether location j
// is occupied in the current world - if it is, and its content
// is not the same as at location i, then it calls this function
// recursively to reproduce the number at location i into the
// the location given by i offset by the content of location j.
//
void reproduceSymbiotic(int i, int j, int level)
{
    // first do a belt and braces check to guard against infinite recursion
    if (level > worldSize) {
        return;
    }

    if ((j >= 0) && (j < worldSize)) {
        // reproduce number in cell i into cell j of next generation
        nextWorld[j] = world[i];
        // if the new contents of cell j comes below a different (non-zero) number,
        // then reproduce it in cell (i + [contents of j])
        if ((world[j] != 0) && (world[j] != world[i])) {
            reproduceSymbiotic(i, i+world[j], ++level);
        }
    }
}


// Exclusion update procedure ("exclusion norm"), as described in Section 4 of (Barricelli, 1954)
void updateExclusion()
{
    for (int i=0; i<worldSize; ++i) {
        // if this cell contains a number (not blank(0) or X), attempt to reproduce it
        if ((world[i] != 0) && (world[i] != X_MARK)) {
            reproduceExclusion(i, i+world[i], 1);
        }
    }
}


// Recursive helper function for updateExclusion() to implement
// the exclusion norm.
//
// This function attempts to reproduce the number at location i in current world into
// location j in the updated world. If location j in the updated world is already
// occupied, and the contents is different to the number we are trying to reproduce,
// then an exlusion mark (X_MARK) is placed in location j instead.
// Regardless of whether the number was copied or an X_MARK was written, the
// function then checks whether location j is occupied in the current world - if it is,
// and its content is not the same as at location i, then we call this function
// recursively to reproduce the number at location i into the
// the location given by i offset by the content of location j.
//
void reproduceExclusion(int i, int j, int level)
{
    // first do a belt and braces check to guard against infinite recursion
    if (level > worldSize) {
        return;
    }

    if ((j >= 0) && (j < worldSize)) {
        if (nextWorld[j] == 0) {
            // the destination cell is blank, so go ahead
            nextWorld[j] = world[i];
        }
        else if (nextWorld[j] == world[i]) {
            // the destination cell contains the same number that we want to move
            // to it, so do nothing in this case (the current number remains)
        }
        else {
            // the destination cell is neither blank nor contains the same
            // number that we want to move to it, so mark it with
            // an exclusion mark
            nextWorld[j] = X_MARK;
        }
        if ((world[j] != 0) && (world[j] != X_MARK) && (world[j] != world[i]) && (i+world[j] != j)) {
            // if the new contents of cell j comes below a different (non-zero) number,
            // then reproduce it in cell (i + [contents of j]). The final condition
            // in the line above (i+world[j] != j) ensures we don't waste our time trying to move
            // into the same cell j as we have just handeled in the current call to this function.
            reproduceExclusion(i, i+world[j], ++level);
        }
    }
}


// Conditional update procedure, as described in Section 5 of (Barricelli, 1954)
void updateConditional()
{
    for (int i=0; i<worldSize; ++i) {
        if ((world[i] != 0) && (world[i] != X_MARK)) {
            // if this cell contains a number (not blank(0) or X), attempt to reproduce it
            reproduceConditional(i, i+world[i], 1);
        }
    }
}


// Recursive helper function for updateConditional() to implement
// the conditional norm.
//
// This function attempts to reproduce the number at location i in current world into
// location j in the updated world. If location j in the updated world is already
// occupied, and the contents is different to the number we are trying to reproduce,
// then an exlusion mark (X_MARK) is placed in location j instead.
// If the exclusion mark falls under an empty cell, or another X_MARK, it is replaced
// by a number equal to the distance between the nearest number to the left and the
// nearest number to the right of the aforementioned empty cell. If the two said
// numbers have the same sign, then the new number (distance) is given a positive
// sign, otherwise a negative sign.
// Regardless of whether the number was copied or an X_MARK was written, the
// function then checks whether location j is occupied in the current world - if it is,
// and its content is not the same as at location i, then we call this function
// recursively to reproduce the number at location i into the
// the location given by i offset by the content of location j.
//
void reproduceConditional(int i, int j, int level)
{
    // first do a belt and braces check to guard against infinite recursion
    if (level > worldSize) {
        return;
    }

    if (debug) {
        std::cout << std::format("reproduceConditional: i={:2}, j={:2}, level={:2}",i,j,level) << std::endl;
    }

    if ((j >= 0) && (j < worldSize)) {
        if (nextWorld[j] == 0) {
            // the destination cell is blank, so go ahead
            nextWorld[j] = world[i];
        }
        else if (nextWorld[j] == world[i]) {
            // the destination cell contains the same number that we want to move
            // to it, so do nothing in this case (the current number remains)
        }
        else {
            // the destination cell contains a number different to the one we
            // are attempting to move into it - so potentially mark it with X_MARK
            // or produce a mutation

            if (world[j] != 0 && world[j] != X_MARK) {
                // the cell above our destination cell contains a number, so the
                // place an X_MARK in the destination cell
                nextWorld[j] = X_MARK;
            }
            else {
                // the cell above our destination cell is either blank or
                // contains an X_MARK, so consider placing a mutated number in
                // the destination cell
                auto [lpos, lnum] = findNearestNumber(j, -1);
                auto [rpos, rnum] = findNearestNumber(j, 1);
                if (lpos == X_MARK || rpos == X_MARK) {
                    // no number found to the left and/or right of the empty cell,
                    // so he destintaion cell gets an X_MARK
                    nextWorld[j] = X_MARK;
                }
                else {
                    // we found the closest numbers to the left and right of the
                    // empty cell, so the destination cell gets assigned a number
                    // corresponding to the distance between these two found cells.
                    // The sign of the assigned number is positive if the found numbers
                    // are of equal sign, or negative otherwise
                    nextWorld[j] = (rpos-lpos) * ((lnum * rnum) > 0 ? 1 : -1);
                }
            }
        }
        if ((world[j] != 0) && (world[j] != X_MARK) && (world[j] != world[i]) && ((i+world[j]) != j)) {
            if (debug) {
                std::cout << "recurse" << std::endl;
            }
            // if the new contents of cell j comes below a different (non-zero) number,
            // then reproduce it in cell (i + [contents of j]). The final condition
            // in the line above (i+world[j] != j) ensures we don't waste our time trying to move
            // into the same cell j as we have just handeled in the current call to this function.
            reproduceConditional(i, i+world[j], ++level);
        }
    }
}


// Helper function to find the position of the nearest cell to cell i that is
// occupied by a number. The paramater delta specifies the direction of the
// search (1=right, -1=left).
// Returns a FindResults object containing the index and contents of the
// found cell, or {X_MARK, X_MARK} is the edge of the world is reached without
// finding an occupied cell.
FindResult findNearestNumber(int i, int delta) {
    assert(delta == 1 || delta == -1);

    int pos = i + delta;
    while ((pos >= 0) && (pos < worldSize)) {
        if ((world[pos] != 0) && (world[pos] != X_MARK)) {
            return {pos, world[pos]};
        }
        pos += delta;
    }

    return {X_MARK, X_MARK};
}
