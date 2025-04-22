#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <array>
#include <set>
#include <utility>
#include <cmath>

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
extern std::unordered_map<Direction, Direction> reverseDirection;

Direction &operator+=(Direction &dir, double angle);

// ========================= FORWARD DECLARATIONS =========================

class Game;
class Tank;
class Artillery;

// ========================= CLASS: GameObject =========================
class GameObject {
    protected:
        int x;
        int y;
        Game* game;

    public:
        GameObject(int x, int y, Game* game):
        x(x), y(y), game(game){}
        int getX(){return x;}
        int getY(){return y;}
    virtual ~GameObject() = default;
};


// ========================= CLASS: MovingGameObject =========================

class MovingGameObject : public GameObject 
{
    protected:
        Direction direction;
    
    public:
        MovingGameObject(int x, int y, Direction dir, Game* game)
            : GameObject(x,y,game), direction(dir) {}
    
        virtual ~MovingGameObject() = default;
    
        // Common interface for all game objects
        
        
        Direction getDirection(){return direction;}
        virtual bool checkForAWall() = 0;
        bool moveForward();
        void updatePosition(Direction dir);
};

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
    Tank(int x, int y, Direction dir, Game *game,int id);

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
    void fire(std::set<int> &tanksToRemove,std::set<int> &shellsToRemove,std::unordered_map<int, Tank *> tanks);

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
    void incrementReverseCharge();
    void resetReverseState();
    void executeReverse();
    void ignoreMove();
};

// ========================= CLASS: Artillery =========================

class Artillery : public MovingGameObject
{
public:
    Artillery(int x, int y, Direction dir, Game *game);
    bool checkForAWall();
    
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
    std::set<int> mines;
    std::unordered_map<int, Wall> walls;

public:
    Game(std::string fileName);

    int getWidth();
    int getHeight();
    int getGameStep();
    int bijection(int x,int y);
    std::pair<int, int> inverseBijection(int z);

    std::unordered_map<int, Tank *> getTanks();
    std::unordered_map<int, Artillery *> getArtillery();
    std::set<int> &getMines();
    std::unordered_map<int, Wall> &getWalls();

    void incrementGameStep();
    void addTank(Tank *tank);
    void addArtillery(Artillery *artillery,std::set<int> &tanksToRemove,std::set<int> &shellsToRemove, std::unordered_map<int, Tank *> tanks);
    void addMine(int x, int y);
    void addWall(int x, int y);

    void removeMine(int x);
    void removeWall(int x);
    void removeTank(int tankId);
    void removeArtillery(int artilleryId);

    void hitWall(int x, int y);
    void hitTank(int tankId);

    int readFile(std::string fileName);
    std::vector<std::string> splitByComma(const std::string &input);
    void gameManager();
    void printBoard();
};