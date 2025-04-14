#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include "Core/Direction.hpp"

class Tank;
class Artillery;
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
    int bijection(int x, int y);
    std::pair<int, int> inverseBijection(int z);

    std::unordered_map<int, Tank *> getTanks();
    std::unordered_map<int, Artillery *> getArtillery();
    std::set<int> &getMines();
    std::unordered_map<int, Wall> &getWalls();

    void incrementGameStep();
    void addTank(Tank *tank);
    void addArtillery(Artillery *artillery,
                      std::set<int> &tanksToRemove,
                      std::set<int> &shellsToRemove,
                      std::set<int> &wallsToRemove,
                      std::unordered_map<int, Tank *> &tanks);
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