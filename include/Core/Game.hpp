#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include "Core/Direction.hpp"
#include <fstream> 

class Tank;
class Artillery;
class  TankChase; 

extern std::ofstream outputFile;
// ========================= CLASS: Game =========================
class Game
{
private:
    int width;
    int height;
    int gameStep;
    int totalShellsRemaining;
    
    std::array<Tank*, 2> players;
    std::unordered_map<int, Tank *> tanks;
    std::unordered_map<int, Artillery *> artilleries;
    std::set<int> mines;
    std::unordered_map<int, Wall> walls;
    std::set<int> wallsToRemove;
    std::set<int> tanksToRemove;
    std::set<int> shellsToRemove;
    std::unordered_map<int, Artillery *> secondaryArtilleries;
    std::unordered_map<int, Tank*> secondaryTanks;

protected:


public:
    Game();

    int getWidth();
    int getHeight();
    int getGameStep();
    int bijection(int x, int y);
    std::pair<int, int> inverseBijection(int z);
    Tank* getPlayer(int playerId);
    std::unordered_map<int, Tank *> getTanks();
    std::unordered_map<int, Artillery *> getArtillery();
    std::set<int> &getMines();
    std::unordered_map<int, Wall> &getWalls();


    void getPlayersInput(std::ofstream &file);
    int getWallHealth(int wallPos);
    void incrementGameStep();
    void addTank(Tank *tank);
    void addArtillery(Artillery *artillery);
    void addMine(int x, int y);
    void addWall(int x, int y);

    void removeMine(int x);
    void removeWall(int x);
    void removeTank(int tankId);
    void removeArtillery(int artilleryId);
    void removeTanks();
    void removeArtilleries();
    void removeWalls();

    void hitWall(int x, int y);
    void hitTank(int tankId);

    int readFile(std::string fileName);
    std::vector<std::string> splitByComma(const std::string &input);
    void checkForAMine(int x, int y);
    void runGame();
    void printBoard();
    void advanceArtilleries();
    void executeTanksMoves();
    void removeObjectsFromTheBoard();
    void reverseHandler(Tank *tank, std::string move);
    void advanceTank(Tank* tank);
    void tankShootingShells(Tank* tank);
    void rotateArtillery(Tank* tank);
    void checkForTankCollision(Tank* tank);
    void checkForShellCollision(Artillery* artillery);
    void tankHitByAShell(int tankPos);
    void artilleryHitAWall(int artilleryPos);

    bool checkForAWinner();
    bool isItATie();
    void outputTankMove(int playerNum, std::string move);

};