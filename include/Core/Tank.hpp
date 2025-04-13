#pragma once

#include "Core/GameObject.hpp"
#include <string>

// ========================= CLASS: Tank =========================

class Tank : public MovingGameObject
{
private:
    int id;
    int artilleryShells;
    bool destroyed;
    int cantShoot;
    int reverseCharge = 0;
    bool reverseQueued = false;
    bool reverseReady = false;
    std::string lastMove;

public:
    Tank(int x, int y, Direction dir, Game *game, int id);

    // Position and state
    int getId();
    std::string getLastMove();
    Direction getDirection();
    bool checkForAWall();

    // Movement
    void moveBackwards();
    void rotateTank(double angle);
    void setDirection(std::string directionStr);
    void setLastMove(std::string lastMoveStr);

    // Firing
    void fire();

    // Damage
    void hit();

    // Shooting cooldown
    void incrementCantShoot();
    void resetCantShoot();
    bool canShoot();
    int getCantShoot();

    // 🔁 Reverse state
    bool isReverseQueued() const;
    bool isReverseReady() const;
    int getReverseCharge() const;
    void queueReverse();
    void cancelReverse();
    void incrementReverseCharge();
    void resetReverseState();
    void executeReverse();
    void endReverseStreak();
};
