#include "Core/Direction.hpp"

std::unordered_map<Direction, std::array<int, 2>> stringToIntDirection = {
    {U, {0, -1}}, {UR, {1, -1}}, {R, {1, 0}}, {DR, {1, 1}}, {D, {0, 1}}, {DL, {-1, 1}}, {L, {-1, 0}}, {UL, {-1, -1}}};

std::unordered_map<std::string, Direction> stringToDirection = {
    {"U", U}, {"UR", UR}, {"R", R}, {"DR", DR}, {"D", D}, {"DL", DL}, {"L", L}, {"UL", UL}};

std::unordered_map<Direction, Direction> reverseDirection = {
    {U, D}, {UR, DL}, {R, L}, {DR, UL}, {D, U}, {DL, UR}, {L, R}, {UL, DR}};

std::unordered_map< Direction, std::string>  directionToString = {
    {U, "U"}, {UR, "UR"}, {R, "R"}, {DR, "DR"}, {D, "D"}, {DL, "DL"}, {L, "L"}, {UL, "UL"}};

std::array<Direction, 8> directions = {U, UR, R, DR, D, DL, L, UL};

std::unordered_map<std::pair<int, int>, std::string, pair_hash> pairToDirections = {
    {{0, -1}, "U"}, {{1, -1}, "UR"}, {{1, 0}, "R"}, {{1, 1}, "DR"}, {{0, 1}, "D"}, {{-1, 1}, "DL"}, {{-1, 0}, "L"}, {{-1, -1}, "UL"}};
    
std::unordered_map<std::string,double> stringToAngle = {{"a",-0.25},{"d",0.25},{"q",-0.125},{"e",0.125},{"x",0}};

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


Direction &operator-=(Direction &dir, double angle) {
    return dir += (-angle); 
}

Direction operator+(Direction dir, double angle) {
    dir += angle;
    return dir;
}

Direction operator-(Direction dir, double angle) {
    dir -= angle;  
    return dir;
}
