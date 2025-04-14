#include "Core/Tank.hpp"
#include "Core/Artillery.hpp"
#include "Core/Game.hpp"
#include <iostream>
// ------------------------ Tank ------------------------

Tank::Tank(int x, int y, Direction d, Game *g, int id)
    : MovingGameObject(x, y, d, g), id(id), artilleryShells(16), destroyed(false), cantShoot(0) {}

int Tank::getId() { return id; }

std::string Tank::getLastMove() { return lastMove; }

void Tank::setLastMove(std::string lastMoveStr)
{
    lastMove = lastMoveStr;
}

void Tank::ignoreMove()
{
    lastMove = "x";
}

void Tank::setDirection(std::string directionStr)
{
    if (stringToDirection.find(directionStr) != stringToDirection.end())
    {
        this->direction = stringToDirection[directionStr];
    }
    else
    {
        std::cerr << "Invalid direction string: " << directionStr << std::endl;
    }
}

void Tank::moveBackwards()
{
    direction = reverseDirection[direction];
    moveForward();
    direction = reverseDirection[direction];
}

bool Tank::checkForAWall()
{
    updatePosition(direction);
    std::cout << "Tank " << id << " checking for a wall at " << (int)x / 2 << ", " << (int)y / 2 << std::endl;
    if (game->getWalls().count(game->bijection(x, y)) == 1)
    {
        std::cout << "Tank " << id << " hit a wall at " << (int)x / 2 << ", " << (int)y / 2 << "!\n";
        updatePosition(reverseDirection[direction]);
        lastMove = "x";
        return true;
    }
    updatePosition(reverseDirection[direction]);
    return false;
}

void Tank::rotateTank(double angle)
{
    if (angle == 0)
        return;
    direction += angle;
}

void Tank::fire(std::set<int> &tanksToRemove, std::set<int> &shellsToRemove, std::set<int> &wallsToRemove, std::unordered_map<int, Tank *> tanks)
{
    artilleryShells -= 1;
    Artillery *shell = new Artillery(x, y, direction, game);
    shell->moveForward();
    shell->moveForward();
    game->addArtillery(shell, tanksToRemove, shellsToRemove, wallsToRemove, tanks);
    std::cout << "Artillery fired!" << std::endl;
}

void Tank::hit()
{
    destroyed = true;
    game->removeTank(id);
}

void Tank::incrementCantShoot() { cantShoot += 1; }

void Tank::resetCantShoot() { cantShoot = 0; }

bool Tank::canShoot() { return cantShoot == 0; }

int Tank::getCantShoot() { return cantShoot; }

int Tank::getReverseCharge() const { return reverseCharge; }
bool Tank::isReverseQueued() const { return reverseQueued; }
bool Tank::isReverseReady() const { return reverseReady; }

void Tank::queueReverse()
{
    reverseQueued = true;
    reverseCharge = 1;
}

void Tank::incrementReverseCharge()
{
    if (reverseCharge < 3)
        reverseCharge++;
    else
        reverseReady = true;
}

void Tank::resetReverseState()
{
    reverseQueued = false;
    reverseCharge = 0;
    reverseReady = false;
}

void Tank::executeReverse()
{
    moveBackwards();
    reverseQueued = false;
    reverseCharge = 0;
    lastMove = "s";
    reverseReady = true;
}
