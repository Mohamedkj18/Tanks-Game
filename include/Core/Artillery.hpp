#pragma once

#include "Core/GameObject.hpp"

// ========================= CLASS: Artillery =========================

class Artillery : public MovingGameObject
{
public:
    Artillery(int x, int y, Direction dir, Game *game);
    bool checkForAWall();
};