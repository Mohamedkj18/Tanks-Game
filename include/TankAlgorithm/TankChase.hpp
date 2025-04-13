// include/TankAlgorithm/TankChase.h
#pragma once

#include "TankAlgorithm.h"
#include <string>
#include <optional>
#include <unordered_map>

// Optional: if you want to define offsets
enum Direction;

class TankChase : public TankAlgorithm
{
public:
    std::string decideAction(Tank *tank, Game *game) override;
    ~TankChase() noexcept override = default;

private:
    std::optional<std::pair<int, int>> bfsToEnemy(std::pair<int, int> start, std::pair<int, int> target, Game *game);
    bool canShootEnemy(Tank *self, Tank *enemy, Game *game);
    bool isSafe(int x, int y, Game *game);
    std::string rotateTowards(Direction current, Direction desired);
    Direction getDirectionFromOffset(int dx, int dy);
    Tank *getEnemy(Tank *self, Game *game);
};
