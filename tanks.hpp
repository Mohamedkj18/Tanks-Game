#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <array>
#include <set>
#include <utility>

// ========================= ENUMS & STRUCTS =========================

enum Direction
{
    U,
    UR,
    R,
    DR,
    D,
    DL,
    L,
    UL
};

struct Wall
{
    int x, y;
    short health;
};

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

// ========================= GLOBAL VARS =========================

extern std::unordered_map<Direction, std::array<int, 2>> stringToIntDirection;
extern std::unordered_map<std::string, Direction> stringToDirection;

Direction &operator+=(Direction &dir, double angle);

// ========================= FORWARD DECLARATIONS =========================

class Game;
class Tank;
class Artillery;

// ========================= CLASS: Tank =========================

class Tank
{
private:
    int id;
    int xTankPosition;
    int yTankPosition;
    int artilleryShells;
    Direction direction;
    bool destroyed;
    Game *game;
    int cantShoot;

    int reverseCharge = 0;
    bool reverseQueued = false;
    bool reverseReady = false;

public:
    Tank(int id, int x, int y, Direction dir, Game *game);

    // Position and state
    int getId();
    int getTankPositionX();
    int getTankPositionY();
    Direction getDirection();

    // Movement
    void moveForward();
    void moveBackwards();
    void rotateTank(double angle);
    void setDirection(std::string directionStr);

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

// ========================= CLASS: Artillery =========================

class Artillery
{
private:
    int x, y;
    int id;
    Direction dir;
    Game *game;
    Tank *tank;

public:
    Artillery(int x, int y, int id, Direction dir, Game *game, Tank *tank);

    int getId();
    int getX();
    int getY();
    Direction getDirection();

    void setX(int x);
    void setY(int y);
    void setDirection(Direction dir);
    void setId(int id);
    void fire();
};

// ========================= CLASS: Game =========================

class Game
{
private:
    int width;
    int height;
    int gameStep;
    int totalShellsRemaining;

    std::unordered_map<int, Tank *> tanks;
    std::unordered_map<int, Artillery *> artilleries;

    std::set<std::pair<int, int>> mines;
    std::unordered_map<std::pair<int, int>, Wall, pair_hash> walls;

public:
    Game(std::string fileName);

    int getWidth();
    int getHeight();
    int getGameStep();

    std::unordered_map<int, Tank *> getTanks();
    std::unordered_map<int, Artillery *> getArtillery();
    std::set<std::pair<int, int>> &getMines();
    std::unordered_map<std::pair<int, int>, Wall, pair_hash> &getWalls();

    void incrementGameStep();
    void addTank(Tank *tank);
    void addArtillery(Artillery *artillery);
    void addMine(int x, int y);
    void addWall(int x, int y);

    void removeMine(int x, int y);
    void removeWall(int x, int y);
    void removeTank(int tankId);
    void removeArtillery(int artilleryId);

    void hitWall(int x, int y);
    void hitTank(int tankId);

    int readFile(std::string fileName);
    std::vector<std::string> splitByComma(const std::string &input);
    void gameManager();
    void printBoard();
};
