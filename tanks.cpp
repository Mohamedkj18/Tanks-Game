#include "tanks.hpp"
#include <iostream>
#include <fstream>

std::unordered_map<Direction, std::array<int, 2>> stringToIntDirection = {
    {U, {0, -1}}, {UR, {1, -1}}, {R, {1, 0}}, {DR, {1, 1}}, {D, {0, 1}}, {DL, {-1, 1}}, {L, {-1, 0}}, {UL, {-1, -1}}};

std::unordered_map<std::string, Direction> stringToDirection = {
    {"U", U}, {"UR", UR}, {"R", R}, {"DR", DR}, {"D", D}, {"DL", DL}, {"L", L}, {"UL", UL}};

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

// ------------------------ Tank ------------------------

Tank::Tank(int n, int x, int y, Direction d, Game *g)
    : id(n), xTankPosition(x), yTankPosition(y), artilleryShells(16),
      direction(d), destroyed(false), game(g), cantShoot(0) {}

int Tank::getId() { return id; }

void Tank::setDirection(std::string directionStr)
{
    if (stringToDirection.find(directionStr) != stringToDirection.end())
    {
        this->direction = stringToDirection[directionStr];
    }
    else
    {
        std::cerr << "Invalid direction string: " << directionStr << std::endl;
    }
}

void Tank::moveForward()
{
    int x, y;
    std::array<int, 2> d = stringToIntDirection[direction];

    x = (xTankPosition + d[0] + game->getWidth()) % game->getWidth();
    y = (yTankPosition + d[1] + game->getHeight()) % game->getHeight();

    if (game->getWalls().find({x, y}) != game->getWalls().end())
    {
        std::cout << "Wall at " << x << ", " << y << " hit!" << std::endl;
        return;
    }
    if (game->getMines().find({x, y}) != game->getMines().end())
    {
        std::cout << "Tank " << id << " hit a mine at " << x << ", " << y << " hit!" << std::endl;
        game->removeMine(x, y);
        game->hitTank(id);
        return;
    }

    yTankPosition = y;
    xTankPosition = x;
}

void Tank::moveBackwards()
{
    int x, y;
    std::array<int, 2> d = stringToIntDirection[direction];

    x = (xTankPosition - d[0] + game->getWidth()) % game->getWidth();
    y = (yTankPosition - d[1] + game->getHeight()) % game->getHeight();
    if (game->getWalls().find({x, y}) != game->getWalls().end())
    {
        std::cout << "Wall at " << x << ", " << y << " hit!" << std::endl;
        return;
    }
    if (game->getMines().find({x, y}) != game->getMines().end())
    {
        std::cout << "Tank " << id << " hit a mine at " << x << ", " << y << " hit!" << std::endl;
        game->removeMine(x, y);
        game->hitTank(id);
        return;
    }

    yTankPosition = y;
    xTankPosition = x;
}

void Tank::rotateTank(double angle)
{
    direction += angle;
}

int Tank::getTankPositionX() { return xTankPosition; }

int Tank::getTankPositionY() { return yTankPosition; }

void Tank::fire()
{
    artilleryShells -= 1;
    Artillery *shell = new Artillery(xTankPosition, yTankPosition, id, direction, game, this);
    game->addArtillery(shell);
    std::cout << "Artillery fired!" << std::endl;
    shell->fire();
}

void Tank::hit()
{
    destroyed = true;
    game->removeTank(id);
}

void Tank::incrementCantShoot() { cantShoot += 1; }

void Tank::resetCantShoot() { cantShoot = 0; }

bool Tank::canShoot() { return cantShoot == 0; }

int Tank::getCantShoot() { return cantShoot; }

int Tank::getReverseCharge() const { return reverseCharge; }
bool Tank::isReverseQueued() const { return reverseQueued; }
bool Tank::isReverseReady() const { return reverseReady; }

void Tank::queueReverse()
{
    reverseQueued = true;
    reverseCharge = 1;
}

void Tank::cancelReverse()
{
    reverseQueued = false;
    reverseCharge = 0;
}

void Tank::incrementReverseCharge()
{
    if (reverseCharge < 3)
        reverseCharge++;
}

void Tank::resetReverseState()
{
    reverseQueued = false;
    reverseCharge = 0;
    reverseReady = false;
}

void Tank::executeReverse()
{
    moveBackwards();
    reverseQueued = false;
    reverseCharge = 0;
    reverseReady = true;
}

void Tank::endReverseStreak()
{
    reverseReady = false;
}

// ------------------------ Artillery ------------------------

Artillery::Artillery(int x, int y, int id, Direction dir, Game *game, Tank *tank)
    : x(x), y(y), id(id), dir(dir), game(game), tank(tank) {}

int Artillery::getId() { return id; }

int Artillery::getX() { return x; }

int Artillery::getY() { return y; }

Direction Artillery::getDirection() { return dir; }

void Artillery::setX(int newX) { x = newX; }

void Artillery::setY(int newY) { y = newY; }

void Artillery::setDirection(Direction d) { dir = d; }

void Artillery::setId(int newId) { id = newId; }

void Artillery::fire()
{
    int w = game->getWidth();
    int h = game->getHeight();

    std::array<int, 2> delta = stringToIntDirection[dir];
    x = (x + delta[0] + w) % w;
    y = (y + delta[1] + h) % h;

    if (game->getWalls().find({x, y}) != game->getWalls().end())
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
                std::cout << "Tank " << t->getId() << " was destroyed by " << tank->getId() << std::endl;
                game->hitTank(pair.first);
            }
        }
    }
}

// ------------------------ Game ------------------------

Game::Game(std::string fileName)
{
    std::cout << "Game started!" << std::endl;
    gameStep = 0;
    readFile(fileName);
    totalShellsRemaining = tanks.size() * 16;
}

int Game::getWidth() { return width; }

int Game::getHeight() { return height; }

int Game::getGameStep() { return gameStep; }

std::unordered_map<int, Tank *> Game::getTanks() { return tanks; }

std::unordered_map<int, Artillery *> Game::getArtillery() { return artilleries; }

std::set<std::pair<int, int>> &Game::getMines() { return mines; }

std::unordered_map<std::pair<int, int>, Wall, pair_hash> &Game::getWalls() { return walls; }

void Game::incrementGameStep() { gameStep++; }

void Game::addTank(Tank *tank)
{
    tanks[tank->getId()] = tank;
}

void Game::addArtillery(Artillery *artillery)
{
    artilleries[artillery->getId()] = artillery;
}

void Game::addMine(int x, int y)
{
    mines.insert({x, y});
}

void Game::addWall(int x, int y)
{
    Wall wall = {x, y, 2};
    walls[{x, y}] = wall;
}

void Game::removeMine(int x, int y)
{
    mines.erase({x, y});
}

void Game::removeWall(int x, int y)
{
    walls.erase({x, y});
}

void Game::removeTank(int tankId)
{
    delete tanks[tankId]; // cleanup
    tanks.erase(tankId);
}

void Game::removeArtillery(int artilleryId)
{
    delete artilleries[artilleryId]; // cleanup
    artilleries.erase(artilleryId);
}

void Game::hitWall(int x, int y)
{
    Wall &wall = walls[{x, y}];
    wall.health--;
    if (wall.health == 0)
    {
        walls.erase({x, y});
        std::cout << "Wall at " << x << ", " << y << " destroyed!" << std::endl;
    }
    else
    {
        std::cout << "Wall hit!" << std::endl;
    }
}

void Game::hitTank(int tankId)
{
    if (tanks.count(tankId))
    {
        tanks[tankId]->hit();
    }
}

std::vector<std::string> Game::splitByComma(const std::string &input)
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

int Game::readFile(std::string fileName)
{
    std::cout << "Reading file: " << fileName << std::endl;
    int xAxis = 0;
    int yAxis = 0;
    std::ifstream file(fileName);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    }

    std::cout << "File opened successfully!" << std::endl;

    getline(file, line);
    std::vector<std::string> params = splitByComma(line);
    height = std::stoi(params[0]);
    width = std::stoi(params[1]);

    std::cout << "Height: " << height << ", Width: " << width << std::endl;

    while (std::getline(file, line))
    {
        std::cout << "Line: " << line << std::endl;
        xAxis = 0;
        for (char c : line)
        {
            if (c == '#')
                addWall(xAxis, yAxis);
            else if (c == '@')
                addMine(xAxis, yAxis);
            else if (c == '1')
            {
                Tank *player1 = new Tank(1, xAxis, yAxis, stringToDirection["L"], this);
                addTank(player1);
            }
            else if (c == '2')
            {
                Tank *player2 = new Tank(2, xAxis, yAxis, stringToDirection["R"], this);
                addTank(player2);
            }
            xAxis++;
        }
        yAxis++;
    }

    file.close();
    std::cout << "File read successfully!" << std::endl;
    return 0;
}

void Game::gameManager()
{
    std::string move;
    int count = 0;

    while (true)
    {
        std::cout << "Game step: " << gameStep << std::endl;
        printBoard();

        std::set<int> tanksToRemove;
        std::vector<int> tankIds;

        // Take a safe snapshot of tank IDs
        for (const auto &pair : tanks)
            tankIds.push_back(pair.first);

        for (int id : tankIds)
        {
            if (!tanks.count(id))
                continue;

            Tank *tank = tanks[id];
            std::cout << "Tank " << tank->getId() << " move: ";
            std::cin >> move;

            // Shooting cooldown handler
            if (tank->getCantShoot() > 0)
            {
                tank->incrementCantShoot();
                if (tank->getCantShoot() == 4)
                    tank->resetCantShoot();
            }

            // Reverse charging in progress
            if (tank->isReverseQueued())
            {
                if (move == "w")
                {
                    tank->cancelReverse();
                    tank->endReverseStreak();
                    std::cout << "Reverse canceled by forward request. Tank stays in place.\n";
                }
                else
                {
                    std::cout << "Ignoring action during reverse charge...\n";
                    if (tank->getReverseCharge() == 2)
                    {
                        tank->executeReverse();
                    }
                    else
                    {
                        tank->incrementReverseCharge();
                        std::cout << "Reverse charging: step " << tank->getReverseCharge() << "/3\n";
                    }
                }
                continue;
            }

            // Reverse request
            if (move == "s")
            {
                if (tank->isReverseQueued())
                {
                    std::cout << "Ignoring additional reverse request: already charging.\n";
                }
                else if (tank->isReverseReady())
                {
                    tank->executeReverse();
                }
                else
                {
                    tank->queueReverse();
                    std::cout << "Reverse queued.\n";
                }
                continue;
            }

            // Forward move cancels reverse streak
            if (move == "w")
            {
                tank->resetReverseState();
                tank->endReverseStreak();
                tank->moveForward();
                goto post_move;
            }

            // Rotation cancels reverse streak
            if (move == "a")
            {
                tank->endReverseStreak();
                tank->rotateTank(-0.25);
            }
            else if (move == "d")
            {
                tank->endReverseStreak();
                tank->rotateTank(0.25);
            }
            else if (move == "q")
            {
                tank->endReverseStreak();
                tank->rotateTank(-0.125);
            }
            else if (move == "e")
            {
                tank->endReverseStreak();
                tank->rotateTank(0.125);
            }
            else if (move == "x")
            {
                tank->endReverseStreak();
                continue;
            }

            // Shooting cancels reverse streak
            else if (move == " ")
            {
                tank->endReverseStreak();
                if (tank->canShoot())
                {
                    tank->fire();
                    totalShellsRemaining--;
                    std::cout << "Tank " << tank->getId() << " fired!\n";
                    tank->incrementCantShoot();
                }
                else
                {
                    std::cout << "Tank " << tank->getId() << " can't shoot yet!\n";
                }
            }

        post_move:
            // Mine detection
            int x = tank->getTankPositionX();
            int y = tank->getTankPositionY();

            if (mines.count({x, y}))
            {
                std::cout << "Tank " << tank->getId() << " hit a mine at " << x << ", " << y << "!\n";
                removeMine(x, y);
                tanksToRemove.insert(tank->getId());
                continue;
            }
        }

        // Collision check (no duplicate pairs)
        for (auto it1 = tanks.begin(); it1 != tanks.end(); ++it1)
        {
            for (auto it2 = std::next(it1); it2 != tanks.end(); ++it2)
            {
                Tank *t1 = it1->second;
                Tank *t2 = it2->second;
                if (t1->getTankPositionX() == t2->getTankPositionX() &&
                    t1->getTankPositionY() == t2->getTankPositionY())
                {
                    std::cout << "Tank " << t1->getId() << " and Tank " << t2->getId() << " collided!\n";
                    tanksToRemove.insert(t1->getId());
                    tanksToRemove.insert(t2->getId());
                }
            }
        }

        // Delete destroyed tanks
        for (int id : tanksToRemove)
        {
            if (tanks.count(id))
                tanks[id]->hit(); // This removes the tank
        }

        // Victory conditions
        if (tanks.size() == 1)
        {
            std::cout << "Game Over! Player " << tanks.begin()->first << " wins!\n";
            return;
        }

        if (tanks.empty())
        {
            std::cout << "Game Over! It's a tie!\n";
            return;
        }

        if (totalShellsRemaining == 0)
        {
            count++;
            if (count == 40)
            {
                std::cout << "Game Over! It's a tie due to time out!\n";
                return;
            }
        }

        gameStep++;
    }
}

void Game::printBoard()
{
    std::vector<std::vector<char>> board(height, std::vector<char>(width, '.'));

    // Add walls
    for (const auto &pair : walls)
    {
        int x = pair.first.first;
        int y = pair.first.second;
        board[y][x] = '#';
    }

    // Add mines
    for (const auto &mine : mines)
    {
        int x = mine.first;
        int y = mine.second;
        board[y][x] = '@';
    }

    // Add artillery
    for (const auto &pair : artilleries)
    {
        Artillery *a = pair.second;
        int x = a->getX();
        int y = a->getY();
        board[y][x] = '*';
    }

    // Add tanks
    for (const auto &pair : tanks)
    {
        Tank *tank = pair.second;
        int x = tank->getTankPositionX();
        int y = tank->getTankPositionY();
        char symbol = '0' + (tank->getId() % 10); // Single-digit tank ID
        board[y][x] = symbol;
    }

    // Print the board
    std::cout << "\n=== Game Step " << gameStep << " ===\n";
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            std::cout << board[y][x];
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

// Main function to run the game
int main()
{
    std::string fileName;
    std::cout << "Enter the map file name: ";
    std::cin >> fileName;
    Game game(fileName);
    game.gameManager();
    std::cout << "Game ended!" << std::endl;
    return 0;
}