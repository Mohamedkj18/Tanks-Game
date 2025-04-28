
#pragma once

#include "TankAlgorithm.hpp"



class TankChase : public TankAlgorithm
{


public:

    TankChase(Game *game, int numOfMovesPerPath, int range = 5);
    
    std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target);
    std::string getNextMove(int playerNum, int playerToChase);
    
};


