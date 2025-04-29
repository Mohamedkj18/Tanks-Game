

#pragma once

#include "TankAlgorithm.hpp"



class TankEvasion : public TankAlgorithm
{
    private:
    bool fleeingMode;


public:
    TankEvasion(Game *game, int numOfMovesPerPath, int range = 5);
    std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target);
    std::string getNextMove(int playerNum, int playerToChase);
    bool isThereATankClose(int x, int y);
    std::pair<int,int> findFirstLegalLocationToFlee(int n, int m);
    
};




