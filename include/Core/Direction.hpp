#pragma once

#include <unordered_map>
#include <array>
#include <string>
#include <utility>
#include <functional>

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
