// Barricelli's Universes
//
// compile with g++ -std=c++20 -o barricelli barricelli.cpp

#include <iostream>
#include <vector>
#include <format> // from C++20
#include <string>
#include <cassert>

enum class Norm {
    BASIC,
    SYMBIOTIC,
    EXCLUSION,
    CONDITIONAL
};

int worldSize = 20;
int numGens = 10;
Norm norm = Norm::BASIC;

const int X_MARK = 99999;
std::vector<int> world;
std::vector<int> nextWorld;

void printUsageAndExit(const std::string& progname, int rc);
int  parseFigNumberOrExit(int argc, char** argv);
std::string getNormName();
void init(int fig);
void initWorld(std::initializer_list<int> initlist);
void printWorld();
void updateWorld();
void flipWorlds();
void updateBasic();
void updateSymbiotic();
void reproduceSymbiotic(int i, int j, int level);
void updateExclusion();
void reproduceExclusion(int i, int j, int level);
void updateConditional();

/********************************************************** */

int main(int argc, char** argv)
{
    int fig = parseFigNumberOrExit(argc, argv);

    init(fig);

    std::cout << std::format("Figure {}: {} reproduction for {} generations with universe size {}",
        fig, getNormName(), numGens, worldSize)
        << std::endl << std::endl;

    printWorld();
    for (int i=1; i<numGens; ++i) {
        updateWorld();
        printWorld();
    }

    std::cout << std::endl;

    return 0;
}


int parseFigNumberOrExit(int argc, char** argv)
{
    std::string progname{ argv[0] };
    std::size_t pos = progname.find_last_of("//");
    if (pos != std::string::npos && pos < progname.size() - 1) {
        progname = progname.substr(pos+1);
    }

    if (argc != 2) {
        printUsageAndExit(progname, 1);
    }

    int fig;
    try {
        fig = std::stoi(argv[1]);
        if (fig < 1 || fig > 22) {
            printUsageAndExit(progname, 1);
        }
    }
    catch (...) {
        printUsageAndExit(progname, 1);
    }

    return fig;
}


void printUsageAndExit(const std::string& progname, int rc) {
    std::cerr << std::format("Usage: {} n", progname) << std::endl;
    std::cerr << "  where n is a figure number between 1 and 22" << std::endl;
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
        default: {
            std::cerr << std::format("Error: Unexpected figure number encountered ({})!", fig) << std::endl;
            exit(1);
        }

    }

}


void initWorld(std::initializer_list<int> initlist)
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
    for (int num : world) {
        std::cout << ((num==0) ? "   " : (num==X_MARK) ? "  X" : std::format("{:3}", num));
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


void updateBasic()
{
    for (int i=0; i<worldSize; ++i) {
        // copy state to same position on next line
        int x = (nextWorld[i]!=0) ? world[i] : 0;           // collision rule for basic reproduction
        nextWorld[i]+=(world[i]-x);

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


void updateSymbiotic()
{
    for (int i=0; i<worldSize; ++i) {
        // reproduce state elsewhere on next line
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
        nextWorld[j] = world[i];
        if ((world[j] != 0) && (world[j] != world[i])) {
            reproduceSymbiotic(i, i+world[j], ++level);
        }
    }
}


void updateExclusion()
{
    for (int i=0; i<worldSize; ++i) {
        // reproduce state elsewhere on next line
        if ((world[i] != 0) && (world[i] != X_MARK)) {
            reproduceExclusion(i, i+world[i], 1);
        }
    }
}

void reproduceExclusion(int i, int j, int level)
{
    // first do a belt and braces check to guard against infinite recursion
    if (level > worldSize) {
        return;
    }

    if ((j >= 0) && (j < worldSize)) {
        if (nextWorld[j] == 0) {
            nextWorld[j] = world[i];
        }
        else if (nextWorld[j] == world[i]) {
            // do nothing
        }
        else {
            nextWorld[j] = X_MARK;
        }
        if ((world[j] != 0) && (world[j] != X_MARK) && (world[j] != world[i])) {
            reproduceExclusion(i, i+world[j], ++level);
        }

    }
}


void updateConditional()
{
    // TODO
    assert(false);
}