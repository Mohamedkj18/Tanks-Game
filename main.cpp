
#include "Core/Game.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: tankgame <input_file>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    Game game(inputFile);

    game.runGame();
    return 0;
}
