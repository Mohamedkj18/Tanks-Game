#include "Core/Direction.hpp"

std::unordered_map<Direction, std::array<int, 2>> stringToIntDirection = {
    {U, {0, -1}}, {UR, {1, -1}}, {R, {1, 0}}, {DR, {1, 1}}, {D, {0, 1}}, {DL, {-1, 1}}, {L, {-1, 0}}, {UL, {-1, -1}}};

std::unordered_map<std::string, Direction> stringToDirection = {
    {"U", U}, {"UR", UR}, {"R", R}, {"DR", DR}, {"D", D}, {"DL", DL}, {"L", L}, {"UL", UL}};

std::unordered_map<Direction, Direction> reverseDirection = {
    {U, D}, {UR, DL}, {R, L}, {DR, UL}, {D, U}, {DL, UR}, {L, R}, {UL, DR}};

Direction &operator+=(Direction &dir, double angle)
{
    std::array<Direction, 8> directions = {U, UR, R, DR, D, DL, L, UL};
    int shift = static_cast<int>(angle * 8);
    int dIdx = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (dir == directions[i])
        {
            dIdx = i;
            break;
        }
    }
    dIdx = (dIdx + shift + 8) % 8;
    dir = directions[dIdx];
    return dir;
}