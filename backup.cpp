#include "tanks.hpp"

std::unordered_map<Direction, std::array<int, 2>> stringToIntDirection = {
    {U, {0, 1}}, {UR, {1, 1}}, {R, {1, 0}}, {DR, {1, -1}}, {D, {0, -1}}, {DL, {-1, -1}}, {L, {-1, 0}}, {UL, {-1, 1}}};

std::unordered_map<std::string, Direction> stringToDirection = {
    {"U", U}, {"UR", UR}, {"R", R}, {"DR", DR}, {"D", D}, {"DL", DL}, {"L", L}, {"UL", UL}};

Direction &operator+=(Direction &dir, double angle)
{
    int newDir = static_cast<int>(dir) + static_cast<int>(angle * 8);
    if (newDir < 0)
        newDir += 8;
    dir = static_cast<Direction>(newDir % 8);
    return dir;
}

class Tank
{

private:
    int id;
    int xTankPosition;
    int yTankPosition;
    int artilleryShells;
    Direction direction;
    bool destroyed;
    int reverse;
    Game *game;
    int cantShoot;

public:
    Tank(int n, int x, int y, Direction d, Game *g)
    {
        id = n;
        xTankPosition = x;
        yTankPosition = y;
        artilleryShells = 16;
        direction = d;
        destroyed = false;
        game = g;
    }

    int getReverse()
    {
        return reverse;
    }
    void incrementReverse()
    {
        reverse += 1;
    }

    int getId()
    {
        return id;
    }

    void setReverse()
    {
        reverse = 0;
    }

    void
    setDirection(std::string direction)
    {
        if (stringToDirection.find(direction) != stringToDirection.end())
        {
            this->direction = stringToDirection[direction]; // Access the value directly
        }
        else
        {
            std::cerr << "Invalid direction string: " << direction << std::endl;
        }
    }

    void moveForward()
    {
        std::array<int, 2> directions = stringToIntDirection[direction];
        xTankPosition += directions[0] % game->getWidth();
        yTankPosition += directions[1] % game->getHeight();
    }

    void moveBackwards()
    {
        std::array<int, 2> directions = stringToIntDirection[direction];
        xTankPosition -= directions[0] % game->getWidth();
        yTankPosition -= directions[1] % game->getHeight();
    }

    void rotateTank(double angle)
    {
        direction += angle;
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
        Artillery *shellFired = new Artillery(xTankPosition, yTankPosition, id, direction, game, this);
        game->addArtillery(shellFired);
        std::cout << "Artillery fired!" << std::endl;
        shellFired->fire();
    }

    void hit()
    {
        destroyed = true;
        game->removeTank(id);
    }

    void resetReverse()
    {
        reverse = 0;
    }

    void incrementCantShoot()
    {
        cantShoot += 1;
    }
    void resetCantShoot()
    {
        cantShoot = 0;
    }
    bool canShoot()
    {
        return cantShoot == 0;
    }
    int getCantShoot()
    {
        return cantShoot;
    }
};

class Artillery
{
private:
    int x, y;
    int id;
    Direction dir;
    Game *game;
    Tank *tank;

public:
    Artillery(int x, int y, int id, Direction dir, Game *game, Tank *tank)
    {
        this->x = x;
        this->y = y;
        this->id = id;
        this->dir = dir;
        this->game = game;
        this->tank = tank;
    }
    int getId()
    {
        return id;
    }
    int getX()
    {
        return x;
    }
    int getY()
    {
        return y;
    }
    Direction getDirection()
    {
        return dir;
    }
    void setX(int x)
    {
        this->x = x;
    }
    void setY(int y)
    {
        this->y = y;
    }
    void setDirection(Direction dir)
    {
        this->dir = dir;
    }

    void setId(int id)
    {
        this->id = id;
    }

    void fire()
    {
        int w, h;

        w = game->getWidth();
        h = game->getHeight();
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

        if (game->getWalls().find(std::make_pair(x, y)) != game->getWalls().end())
        {
            std::cout << "Wall at " << x << ", " << y << " hit!" << std::endl;
            game->hitWall(x, y);
        }
        else
        {
            for (const auto &pair : game->getTanks())
            {
                Tank *t = pair.second;
                if (t->getTankPositionX() == x && t->getTankPositionY() == y)
                {
                    std::cout << "Tank " << t->getId() << " was destroyed by " << this->tank->getId() << std::endl;
                    game->hitTank(pair.first);
                }
            }
        }
    }
};

class Game
{
private:
    int width;
    int height;
    int gameStep;
    int totalShellsRemaining;

    std::unordered_map<int, Tank *> tanks;
    std::unordered_map<int, Artillery *> artilleries;

    std::set<std::pair<int, int>> mines;
    std::unordered_map<std::pair<int, int>, Wall> walls;

public:
    Game(std::string fileName)
    {
        std::cout << "Game started!" << std::endl;
        gameStep = 0;
        readFile(fileName);
        totalShellsRemaining = tanks.size() * 16;
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

    std::unordered_map<int, Tank *> getTanks()
    {
        return tanks;
    }

    std::unordered_map<int, Artillery *> getArtillery()
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

    void addTank(Tank *tank)
    {
        tanks[tank->getId()] = std::move(tank);
    }

    void addArtillery(Artillery *artillery)
    {
        artilleries[artillery->getId()] = std::move(artillery);
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
            std::cout << "Wall at " << x << ", " << y << " destroyed!" << std::endl;
        }
        else
        {
            walls[std::make_pair(x, y)] = wall;
            std::cout << "Wall hit!" << std::endl;
        }
    }

    void hitTank(int tankId)
    {
        Tank *tank = tanks[tankId];
        tank->hit();
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
        int xAxis = 0;
        int yAxis = 0;
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
                    Tank *player1 = new Tank(1, xAxis, yAxis, stringToDirection["L"], this);
                    addTank(player1);
                }
                else if (c == '2')
                {
                    Tank *player2 = &Tank(2, xAxis, yAxis, stringToDirection["R"], this);
                    addTank(player2);
                }
                xAxis += 1;
            }
            yAxis += 1;
        }

        file.close();
        return 0;
    }

    void gameManager()
    {
        std::string move;
        int count = 0;
        while (true)
        {
            std::cout << "Game step: " << gameStep << std::endl;
            for (const auto &pair : tanks)
            {
                int id = pair.first;
                Tank *tank = pair.second;
                std::cout << "Tank " << tank->getId() << " move:" << std::endl;
                std::cin >> move;
                if (tank->getCantShoot() > 0)
                {
                    tank->incrementCantShoot();
                    if (tank->getCantShoot() == 4)
                    {
                        tank->resetCantShoot();
                    }
                }
                if (tank->getReverse() == 2)
                {
                    tank->moveBackwards();
                    tank->incrementReverse();
                    continue;
                }

                if (move == "w")
                {
                    if (tank->getReverse() > 0)
                    {
                        tank->resetReverse();
                    }
                    else
                    {
                        tank->moveForward();
                    }
                }
                else if (tank->getReverse() == 1)
                {
                    continue;
                }
                else if (move == "s")
                {
                    if (tank->getReverse() > 2)
                    {
                        tank->moveBackwards();
                    }
                    tank->incrementReverse();
                }

                else if (move == "q")
                {
                    tank->rotateTank(-0.125);
                }
                else if (move == "a")
                {
                    tank->rotateTank(-0.25);
                }
                else if (move == "e")
                {
                    tank->rotateTank(0.125);
                }
                else if (move == "d")
                {
                    tank->rotateTank(0.25);
                }
                else if (move == " ")
                {
                    if (tank->canShoot())
                    {
                        tank->fire();
                        totalShellsRemaining--;
                        std::cout << "Tank " << tank->getId() << " fired!" << std::endl;
                        tank->incrementCantShoot();
                    }
                    else
                    {
                        std::cout << "Tank " << tank->getId() << " can't shoot!" << std::endl;
                    }
                }
            }

            if (totalShellsRemaining == 0)
            {
                count++;
                if (count == 40)
                {
                    std::cout << "Game Over! It's a tie!" << std::endl;
                    return;
                }
            }
            for (const auto &pair : tanks)
            {
                int id = pair.first;
                Tank *tank1 = pair.second;
                for (const auto &pair2 : tanks)
                {
                    int id2 = pair2.first;
                    Tank *tank2 = pair2.second;
                    if (tank1->getTankPositionX() == tank2->getTankPositionX() && tank1->getTankPositionY() == tank2->getTankPositionY())
                    {
                        std::cout << "Tank " << tank1->getId() << " and Tank " << tank2->getId() << " collided!" << std::endl;
                        tank1->hit();
                        tank2->hit();
                    }
                }
            }
            if (tanks.size() == 1)
            {
                std::cout << "Game Over! Player " << tanks.begin()->first << " wins!" << std::endl;
                return;
            }
            if (tanks.size() == 0)
            {
                std::cout << "Game Over!  It's a tie!" << std::endl;
                return;
            }

            gameStep++;
        }
    }
};

int main()
{
    std::string fileName;
    std::cout << "Enter the name of the file: ";
    std::cin >> fileName;
    Game game = Game(fileName);
    std::cout << "Game initialized!" << std::endl;
    game.gameManager();
    std::cout << "Game ended!" << std::endl;
    return 0;
}