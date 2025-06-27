// Barricelli's Universes
//
// compile with g++ -std=c++20 -o barricelli barricelli.cpp

#include <iostream>
#include <vector>
#include <format> // from C++20
#include <cassert>
#include <string>

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

void printUsage(const std::string& progname);
int parseRuleNumber(int argc, char** argv);
void init(int rule);
void initWorld(std::initializer_list<int> initlist);
void printWorld();
void updateWorld();
void flipWorlds();
void updateBasic();
void updateConditional();
void updateExclusion();

/********************************************************** */

int main(int argc, char** argv)
{
    int rule = parseRuleNumber(argc, argv);

    init(rule);

    std::cout << std::format("> Running rule {} ({} norm) for {} generations with universe size {}",
        rule, "basic", numGens, worldSize)
        << std::endl << std::endl;

    printWorld();
    for (int i=1; i<numGens; ++i) {
        updateWorld();
        printWorld();
    }

    std::cout << std::endl;

    return 0;
}


int parseRuleNumber(int argc, char** argv)
{
    std::string progname{ argv[0] };
    std::size_t pos = progname.find_last_of("//");
    if (pos != std::string::npos && pos < progname.size() - 1) {
        progname = progname.substr(pos+1);
    }

    if (argc != 2) {
        printUsage(progname);
        exit(1);
    }

    int rule;
    try {
        rule = std::stoi(argv[1]);
        if (rule < 1 || rule > 22) {
            printUsage(progname);
            exit(1);
        }
    }
    catch (...) {
        printUsage(progname);
        exit(1);
    }

    return rule;
}


void printUsage(const std::string& progname) {
    std::cerr << std::format("Usage: {} n", progname) << std::endl;
    std::cerr << "  where n is rule number between 1 and 22" << std::endl;
}


void init(int rule)
{
    switch (rule) {
        case 1: {
            worldSize = 22;
            numGens = 10;
            norm = Norm::BASIC;
            initWorld({4,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,-3}); //TODO
            break;
        }
        case 2: {
            worldSize = 17;
            numGens = 5;
            norm = Norm::BASIC;
            initWorld({4});
            break;
        }
        case 3: {
            worldSize = 13;
            numGens = 7;
            norm = Norm::BASIC;
            initWorld({0,0,0,0,0,0,0,0,0,0,0,0,-2});
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
        std::cout << std::format("{:3}", num);
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
        nextWorld[i] = world[i];
        int c=i+world[i];
        if (c>=0 && c<worldSize) {
            nextWorld[c]=world[i];
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
    // TODO
    assert(false);
}