#pragma once

#include "TankAlgorithm/TankAlgorithm.hpp"

class TankChase : public TankAlgorithm
{
public:
    std::string computeMove(Tank *self, Tank *opponent, Game *game) override;
};
