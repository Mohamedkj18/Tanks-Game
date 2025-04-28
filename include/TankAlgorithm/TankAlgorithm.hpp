
#pragma once


#include "TankAlgorithm.hpp"
#include <string>
#include <optional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <unordered_set>
#include "Core/Game.hpp"
#include "Core/Tank.hpp"
#include "Core/Artillery.hpp"

// Forward declarations to avoid circular includes
class Tank;
class Game;
class Artillery;
/**
 * Abstract base class for all tank control algorithms.
 */
class TankAlgorithm
{
protected:
    int moveNumToBeExecuted;
    std::array<std::string, 8> movesToBeExecuted;
    int moveToAdd;
    Game* game;
    int numOfMovesPerPath;
    int range;



public:

TankAlgorithm(Game *game, int numOfMovesPerPath, int range = 5);

std::pair<int, int> move(std::pair<int, int> current, Direction direction);
std::string getDirectionFromPosition(std::pair<int, int> current, std::pair<int, int> target);
void setTheMovesToBeExecuted(int playerNum, int playerToChase);
int getMovesToRotateTank(std::string directionToRotateTo, std::string currentDirection);
void addMoveToBeExecuted(double angle);
virtual std::string getNextMove(int playerNum, int playerToChase) = 0;
bool isTheEnemyInRange(int playerNum, int playerToChase);
bool isThereAMineOrAWall(int x, int y);
int isTheSquareSafe(int x, int y,int rangeToCheck);
virtual std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) = 0;
};
