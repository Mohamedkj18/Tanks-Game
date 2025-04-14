#pragma once

#include "Core/Game.hpp"
#include "Core/Tank.hpp"

class TankAlgorithm
{
public:
    virtual std::string computeMove(Tank *self, Tank *opponent, Game *game) = 0;
    virtual ~TankAlgorithm() = default;
};
