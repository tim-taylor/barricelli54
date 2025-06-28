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
    EXCLUSION,
    CONDITIONAL
};

int worldSize = 20;
int numGens = 10;
Norm norm = Norm::BASIC;

std::vector<int> world;
std::vector<int> nextWorld;

void printUsageAndExit(const std::string& progname, int rc);
int  parseRuleNumberOrExit(int argc, char** argv);
std::string getNormName();
void init(int rule);
void initWorld(std::initializer_list<int> initlist);
void printWorld();
void updateWorld();
void flipWorlds();
void updateBasic();
void updateConditional();
void updateExclusion();
void checkForReproduction(int i, int j, int level);

/********************************************************** */

int main(int argc, char** argv)
{
    int rule = parseRuleNumberOrExit(argc, argv);

    init(rule);

    std::cout << std::format("> Running rule {} ({} norm) for {} generations with universe size {}",
        rule, getNormName(), numGens, worldSize)
        << std::endl << std::endl;

    printWorld();
    for (int i=1; i<numGens; ++i) {
        updateWorld();
        printWorld();
    }

    std::cout << std::endl;

    return 0;
}


int parseRuleNumberOrExit(int argc, char** argv)
{
    std::string progname{ argv[0] };
    std::size_t pos = progname.find_last_of("//");
    if (pos != std::string::npos && pos < progname.size() - 1) {
        progname = progname.substr(pos+1);
    }

    if (argc != 2) {
        printUsageAndExit(progname, 1);
    }

    int rule;
    try {
        rule = std::stoi(argv[1]);
        if (rule < 1 || rule > 22) {
            printUsageAndExit(progname, 1);
        }
    }
    catch (...) {
        printUsageAndExit(progname, 1);
    }

    return rule;
}


void printUsageAndExit(const std::string& progname, int rc) {
    std::cerr << std::format("Usage: {} n", progname) << std::endl;
    std::cerr << "  where n is rule number between 1 and 22" << std::endl;
    exit(rc);
}


std::string getNormName()
{
    switch (norm) {
        case Norm::BASIC: return "basic";
        case Norm::CONDITIONAL: return "conditional";
        case Norm::EXCLUSION: return "exclusion";
        default: {
            std::cerr << std::format("Error! Encountered unknown norm {}", (int)norm) << std::endl;
            exit(1);
        }

    }
}


void init(int rule)
{
    switch (rule) {
        case 1: {
            worldSize = 62;
            numGens = 10;
            norm = Norm::BASIC;
            initWorld({4,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,-3,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,2,0,0,0,0,-8,0}); //TODO
            break;
        }
        case 2: {
            worldSize = 17;
            numGens = 5;
            norm = Norm::EXCLUSION;
            initWorld({4});
            break;
        }
        case 3: {
            worldSize = 13;
            numGens = 7;
            norm = Norm::EXCLUSION;
            initWorld({0,0,0,0,0,0,0,0,0,0,0,0,-2});
            break;
        }
        case 4: {
            worldSize = 20;
            numGens = 10;
            norm = Norm::EXCLUSION;
            initWorld({0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,-3});
            break;
        }
        case 5: {
            worldSize = 11;
            numGens = 5;
            norm = Norm::EXCLUSION;
            initWorld({4,0,0,0,0,3,0,0,0,-2});
            break;
        }
        default: {
            std::cerr << std::format("Error: Unexpected rule encountered ({})!", rule) << std::endl;
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

    world.clear();
    world.resize(worldSize);
    nextWorld.clear();
    nextWorld.resize(worldSize);

    int pos = 0;
    for (int num : initlist) {
        world[pos++] = num;
    }
}


void printWorld()
{
    for (int num : world) {
        std::cout << ((num==0) ? "   " : std::format("{:3}", num));
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
        case Norm::CONDITIONAL: {
            updateConditional();
            break;
        }
        case Norm::EXCLUSION: {
            updateExclusion();
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


void updateConditional()
{
    // TODO
    assert(false);
}

void updateExclusion()
{
    for (int i=0; i<worldSize; ++i) {
        // reproduce state elsewhere on next line
        if (world[i] != 0) {
            int c = i + world[i];
            checkForReproduction(i,c,1);
        }
    }
}


void checkForReproduction(int i, int j, int level)
{
    if (level > 100) {
        return;
    }

    if (j >= 0 && j < worldSize) {
        nextWorld[j] = world[i];
        if (world[j] != 0 && world[j] != world[i]) {
            int d = world[j];
            int k = i + d;
            checkForReproduction(i,k,++level);
        }
    }
}