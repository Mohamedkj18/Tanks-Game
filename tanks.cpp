#include <iostream>
#include <set>
#include <utility>
#include <list>
#include <unordered_map>
#include "File.h"

class Game
{
private:
    int width;
    int height;
    int gameStep;

    std::unordered_map<int, Tank> tanks;
    std::unordered_map<int, Artillery> artilleries;

    std::set<std::pair<int, int>> mines;
    std::unordered_map<std::pair<int, int>, Wall> walls;

public:
    Game(File &file)
    {
        std::cout << "Game started!" << std::endl;
        gameStep = 0;
        readFile(file);

        gameStep = 0;
        artilleries = std::unordered_map<int, Artillery>();
    }

    int getWidth()
    {
        return width;
    }
    int getHeight()
    {
        return height;
    }
    int getGameStep()
    {
        return gameStep;
    }
    std::unordered_map<int, Tank> getTanks()
    {
        return tanks;
    }
    std::unordered_map<int, Artillery> getArtillery()
    {
        return artilleries;
    }
    std::set<std::pair<int, int>> getMines()
    {
        return mines;
    }
    std::unordered_map<std::pair<int, int>, Wall> getWalls()
    {
        return walls;
    }
    void incrementGameStep()
    {
        gameStep++;
    }
    void addTank(Tank tank)
    {
        tanks.insert(std::make_pair(tank.getId(), tank));
    }
    void addArtillery(Artillery artillery)
    {
        artilleries.insert(std::make_pair(artillery.getId(), artillery));
    }

    void addMine(int x, int y)
    {
        mines.insert(std::make_pair(x, y));
    }
    void addWall(int x, int y)
    {
        Wall wall;
        wall.x = x;
        wall.y = y;
        wall.health = 2;
        walls.insert(std::make_pair(std::make_pair(x, y), wall));
    }
    void removeMine(int x, int y)
    {
        mines.erase(std::make_pair(x, y));
    }
    void removeWall(int x, int y)
    {
        walls.erase(std::make_pair(x, y));
    }
    void removeTank(int tankId)
    {
        tanks.erase(tankId);
    }
    void removeArtillery(int artilleryId)
    {
        artilleries.erase(artilleryId);
    }
    void hitWall(int x, int y)
    {
        Wall wall = walls[std::make_pair(x, y)];
        wall.health--;
        if (wall.health == 0)
        {
            walls.erase(std::make_pair(x, y));
            std::cout << "Wall at " << x << ', ' << y << " destroyed!" << std::endl;
        }
        else
        {
            walls[std::make_pair(x, y)] = wall;
            std::cout << "Wall hit!" << std::endl;
        }
    }

    void hitTank(int tankId, Tank tank)
    {
        Tank tank = tanks[tankId];
        tank.hit();
    }
    std::vector<std::string> splitByComma(const std::string &input)
    {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = input.find(',');

        while (end != std::string::npos)
        {
            tokens.push_back(input.substr(start, end - start));
            start = end + 1;
            end = input.find(',', start);
        }
        tokens.push_back(input.substr(start));

        return tokens;
    }

    int readFile(std::string fileName)
    {
        int currxAxis = 0;
        int curryAxis = 0;
        std::ifstream file(fileName);
        std::string line;

        if (!file.is_open())
        {
            std::cerr << "Failed to open file!" << std::endl;
            return 1;
        }
        getline(file, line);
        std::vector<std::string> Paramaters = splitByComma(line);
        height = std::stoi(Paramaters[0]);
        width = std::stoi(Paramaters[1]);
        while (std::getline(file, line))
        {
            for (char c : line)
            {
                if (c == '#')
                    addWall(xAxis, height);
                else if (c == '@')
                    addMine(xAxis, height);
                else if (c == '1')
                {
                    Tank player1 = Tank(1, xAxis, y, stringToDirection.find("L"), game);
                    addTank(player1);
                }
                else if (c == '2')
                {
                    Tank player2 = Tank(2, xAxis, y, stringToDirection.find("R"), game);
                    addTank(player2);
                }
                currxAxis += 1;
            }
            curryAxis += 1;
        }

        file.close();
        return 0;
    }
};

class Tank
{

    static const std::unordered_map<std::string, int[2]> stringToIntDirection{{"U", {0, 1}}, {"UR", {1, 1}}, {"R", {1, 0}}, {"DR", {1, -1}}, {"D", {0, -1}}, {"DL", {-1, -1}}, {"L", {-1, 0}}, {"UL", {-1, 1}}};
    static const std::unordered_map < std::string, Direction stringToDirection{{"U", Direction::U}, {"UR", Direction::UR}, {"R", Direction::R}, {"DR", Direction::DR}, {"D", Direction::D}, {"DL", Direction::DL}, {"L", Direction::L}, {"UL", Direction::UL}};

private:
    int id;
    int xTankPosition;
    int yTankPosition;
    int artilleryShells;
    Direction direction;
    bool destroyed;
    Game game;

public:
    Tank(int n, int x, int y, Direction d, Game g)
    {
        id = n;
        xTankPosition = x;
        yTankPosition = y;
        artilleryShells = 16;
        direction = d;
        destroyed = false;
        game = g;
    }

    int getId()
    {
        return id;
    }

    void setDirection(std::string d)
    {
        direction = stringToDirection.find(d);
    }

    void setPosition(std::string direction)
    {
        int[2] directions = stringToIntDirection.find(direction);
        xTankPosition += directions[0] % game::width;
        yTankPosition += directions[1] % game::height;
    }

    void rotateTank(double angle)
    {
        rotateTank.direction += angle;
    }

    int getTankPositionX()
    {
        return xTankPosition;
    }

    int getTankPositionY()
    {
        return yTankPosition;
    }

    void fire()
    {
        artilleryShells -= 1;
        Artillery shellFired = Artillery(x, y, id, direction, game);
        game.addArtillery(shellFired);
        std::cout << "Artillery fired!" << std::endl;
        shellFired.fire();
    }

    void hit()
    {
        destroyed = true;
        game.removeTank(id);
        std::cout << "Tank " << id << " was destroyed!" << std::endl;
    }
}

enum Dierction {
    U,
    UR,
    R,
    DR,
    D,
    DL,
    L,
    UL
};

Dierction &operator+=(Dierction &dir, double angle)
{
    if (angle == 0.25)
    {
        switch (dir)
        {
        case U:
            dir = R;
            break;
        case UR:
            dir = DR;
            break;
        case R:
            dir = D;
            break;
        case DR:
            dir = DL;
            break;
        case D:
            dir = L;
            break;
        case DL:
            dir = UL;
            break;
        case L:
            dir = U;
            break;
        case UL:
            dir = UR;
            break;
        default:
            break;
        }
        if (angle == -0.25)
        {
            switch (dir)
            {
            case U:
                dir = UL;
                break;
            case UR:
                dir = U;
                break;
            case R:
                dir = UR;
                break;
            case DR:
                dir = R;
                break;
            case D:
                dir = DR;
                break;
            case DL:
                dir = D;
                break;
            case L:
                dir = DL;
                break;
            case UL:
                dir = L;
                break;
            default:
                break;
            }
        }
        if (angle == 0.125)
        {
            switch (dir)
            {
            case U:
                dir = UR;
                break;
            case UR:
                dir = R;
                break;
            case R:
                dir = DR;
                break;
            case DR:
                dir = D;
                break;
            case D:
                dir = DL;
                break;
            case DL:
                dir = L;
                break;
            case L:
                dir = UL;
                break;
            case UL:
                dir = U;
                break;
            default:
                break;
            }
        }
        if (angle == -0.125)
        {
            switch (dir)
            {
            case U:
                dir = UL;
                break;
            case UR:
                dir = U;
                break;
            case R:
                dir = UR;
                break;
            case DR:
                dir = R;
                break;
            case D:
                dir = DR;
                break;
            case DL:
                dir = D;
                break;
            case L:
                dir = DL;
                break;
            case UL:
                dir = L;
                break;
            default:
                break;
            }
        }
    }
}

class Artillery
{
private:
    int x, y;
    int id;
    dierction dir;
    Game game;

public:
    Artillery(int x, int y, int id, Dierction dir, Game game) : x(x), y(y), id(id), dir(dir) {}

    void fire()
    {
        int w, h;

        w = game.getWidth();
        h = game.getHeight();
        int i = 0;

        if (dir == U)
        {

            y = (y - 1) % h;
        }
        else if (dir == UR)
        {
            y = (y - 1) % h;
            x = (x + 1) % w;
        }
        else if (dir == R)
        {
            x = (x + 1) % w;
        }
        else if (dir == DR)
        {
            y = (y + 1) % h;
            x = (x + 1) % w;
        }
        else if (dir == D)
        {
            y = (y + 1) % h;
        }
        else if (dir == DL)
        {
            y = (y + 1) % h;
            x = (x - 1) % w;
        }
        else if (dir == L)
        {
            x = (x - 1) % w;
        }
        else if (dir == UL)
        {
            y = (y - 1) % h;
            x = (x - 1) % w;
        }

        if (game.getWalls().find(std::make_pair(x, y)) != game.getWalls().end())
        {
            std::cout << "Wall at " << x << ', ' << y << " hit!" << std::endl;
            game.hitWall(x, y);
        }
        else
        {
            for (const auto &pair : game.getTanks())
            {
                Tank tank = pair.second;
                if (tank.getX() == x && tank.getY() == y)
                {
                    std::cout << "Tank at " << x << ', ' << y << " hit!" << std::endl;
                    game.hitTank(pair.first, tank);
                }
            }
        }
    }
};

struct Wall
{
    int x, y;
    short health;
} typedef Wall;

int main
{
    File file("game.txt");
    Game game(file);
    std::cout << "Game initialized!" << std::endl;
    game.getTanks()[1].setDirection("L");
    return 0;
}
