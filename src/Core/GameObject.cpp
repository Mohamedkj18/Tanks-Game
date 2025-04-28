#include "Core/GameObject.hpp"
#include "Core/Game.hpp"
#include "Core/Direction.hpp"

// ------------------------ MovingGameObject ------------------------

bool MovingGameObject::moveForward()
{
    updatePosition(direction);
    if (checkForAWall())
    {
        updatePosition(reverseDirection[direction]);
        return false;
    }
    return true;
}

void MovingGameObject::updatePosition(Direction dir)
{
    int switchDirections;
    std::array<int, 2> d = stringToIntDirection[dir];
    x = (x + d[0] + game->getWidth() * 2) % (game->getWidth() * 2);
    y = (y + d[1] + game->getHeight() * 2) % (game->getHeight() * 2);
}
