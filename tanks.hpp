

#include <iostream>
#include <set>
#include <utility>
#include <array>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>

class Game;
class Artillery;
class TankShellAbstract;

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

extern std::unordered_map<Direction, std::array<int, 2>> directionToDelta;
extern std::unordered_map<std::string, Direction> stringToDirection;

class Tank
{
private:
    int id;
    int xTankPosition;
    int yTankPosition;
    int artilleryShells;
    Direction direction;
    bool destroyed;
    int reverse = 0;
    Game *game;
    int cantShoot = 0;

public:
    Tank() = default;
    Tank(int n, int x, int y, Direction d, Game *g);

    int getId() const;
    void fire();
    void hit();
    void moveForward();
    void moveBackwards();
    void rotateTank(double angle);
    int getTankPositionX() const;
    int getTankPositionY() const;
    void incrementReverse();
    void resetReverse();
    int getReverse() const;
    void incrementCantShoot();
    void resetCantShoot();
    bool canShoot() const;
    int getCantShoot() const;
};

class Artillery
{
private:
    int x, y, id;
    Direction dir;
    Game *game;
    Tank *tank;

public:
    Artillery() = default;
    Artillery(int x, int y, int id, Direction dir, Game *game, Tank *tank);
    void fire();
    int getId() const;
    int getX() const;
    int getY() const;
    Direction getDirection() const;
    void setX(int x);
    void setY(int y);
    void setDirection(Direction dir);
    void setId(int id);
};

class Game
{
private:
    int width;
    int height;
    int gameStep;
    int totalShellsRemaining;

    std::unordered_map<int, Tank> tanks;
    std::unordered_map<int, Artillery> artilleries;
    std::set<std::pair<int, int>> mines;
    std::unordered_map<std::pair<int, int>, Wall, pair_hash> walls;

public:
    Game(std::string fileName);
    int getWidth() const;
    int getHeight() const;
    int getGameStep() const;
    std::unordered_map<int, Tank> &getTanks();
    std::unordered_map<int, Artillery> &getArtillery();
    std::set<std::pair<int, int>> &getMines();
    std::unordered_map<std::pair<int, int>, Wall, pair_hash> &getWalls();
    void incrementGameStep();
    void addTank(const Tank &tank);
    void addArtillery(const Artillery &artillery);
    void addMine(int x, int y);
    void addWall(int x, int y);
    void removeMine(int x, int y);
    void removeWall(int x, int y);
    void removeTank(int tankId);
    void removeArtillery(int artilleryId);
    void hitWall(int x, int y);
    void hitTank(int tankId);
    int readFile(const std::string &fileName);
    void gameManager();
};