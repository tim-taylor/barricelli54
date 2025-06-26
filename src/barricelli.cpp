// Barricelli's Universes
//
// compile with g++ -std=c++20 -o barricelli barricelli.cpp

#include <iostream>
#include <vector>
#include <format> // from C++20
//#include <cstdarg>
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

void init(int rule);
void initWorld(std::initializer_list<int> initlist);
void printWorld();
void updateWorld();
void updateBasic();
void updateConditional();
void updateExclusion();

/********************************************************** */

int main(int argc, char** argv) {

    int rule = 2; // TODO get this from command line

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


void init(int rule)
{
    if (rule < 1 || rule > 22) {
        std::cerr << std::format("Error! Rule set to {} but should be in range 1 to 22.", rule) << std::endl;
        exit(1);
    }

    switch (rule) {
        case 1: {
            worldSize = 10;
            numGens = 5;
            norm = Norm::BASIC;
            //initWorld(); //TODO
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
            std::cerr << std::format("Unexpected rule encountered ({})!", rule) << std::endl;
            exit(1);
        }

    }

}


void initWorld(std::initializer_list<int> initlist)
{
    world.clear();
    world.resize(worldSize);
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
}


void updateBasic()
{

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