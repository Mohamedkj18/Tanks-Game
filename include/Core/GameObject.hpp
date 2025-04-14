#pragma once
#include "Core/Direction.hpp"

class Game; // Forward declaration

// ========================= CLASS: GameObject =========================
class GameObject
{
protected:
    int x;
    int y;
    Game *game;

public:
    GameObject(int x, int y, Game *game) : x(x), y(y), game(game) {}
    int getX() { return x; }
    int getY() { return y; }
    virtual ~GameObject() = default;
};

// ========================= CLASS: MovingGameObject =========================

class MovingGameObject : public GameObject
{
protected:
    Direction direction;

public:
    MovingGameObject(int x, int y, Direction dir, Game *game)
        : GameObject(x, y, game), direction(dir) {}

    virtual ~MovingGameObject() = default;

    // Common interface for all game objects

    Direction getDirection() { return direction; }
    virtual bool checkForAWall() = 0;
    bool moveForward();
    void updatePosition(Direction dir);
};
