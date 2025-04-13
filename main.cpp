#include <iostream>
#include "Core/Game.hpp"

int main()
{
    std::string fileName;
    std::cout << "Enter the map file name: ";
    std::cin >> fileName;
    Game game(fileName);
    game.gameManager();
    std::cout << "Game ended!" << std::endl;
    return 0;
}
