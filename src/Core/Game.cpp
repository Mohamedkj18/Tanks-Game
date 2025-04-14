#include "Core/Game.hpp"
#include "Core/Tank.hpp"
#include "Core/Artillery.hpp"
#include "Core/Direction.hpp"
#include "TankAlgorithm/TankAlgorithm.hpp"
#include "TankAlgorithm/TankChase.hpp"
#include "TankAlgorithm/TankEvasion.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

std::ofstream outputFile("data/output.txt");

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
std::set<int> &Game::getMines() { return mines; }
std::unordered_map<int, Wall> &Game::getWalls() { return walls; }

void Game::incrementGameStep() { gameStep++; }

void Game::addTank(Tank *tank)
{
    tanks[bijection(tank->getX(), tank->getY())] = tank;
}

int Game::bijection(int x, int y)
{
    return ((x + y) * (x + y + 1)) / 2 + y;
}

std::pair<int, int> Game::inverseBijection(int z)
{
    int w = static_cast<int>(std::floor((std::sqrt(8 * z + 1) - 1) / 2));
    int t = (w * w + w) / 2;
    int y = z - t;
    int x = w - y;
    return {x, y};
}

void Game::removeMine(int pos)
{
    mines.erase(pos);
}

void Game::removeTank(int pos)
{
    tanks.erase(pos);
}

void Game::removeArtillery(int pos)
{
    artilleries.erase(pos);
}

void Game::removeWall(int pos)
{
    walls.erase(pos);
}

void Game::addWall(int x, int y)
{
    walls[bijection(x, y)] = Wall{x, y, 2};
}

void Game::addMine(int x, int y)
{
    mines.insert(bijection(x, y));
}

void Game::addArtillery(Artillery *shell,
                        std::set<int> &tanksToRemove,
                        std::set<int> &shellsToRemove,
                        std::set<int> &wallsToRemove,
                        std::unordered_map<int, Tank *> &secondaryTanks)
{
    int newPos = bijection(shell->getX(), shell->getY());

    if (walls.count(newPos) == 1)
    {
        walls[newPos].health -= 1;
        if (walls[newPos].health <= 0)
            wallsToRemove.insert(newPos);
        return;
    }

    if (tanks.count(newPos) == 1)
        tanksToRemove.insert(newPos);

    if (secondaryTanks.count(newPos) == 1)
        tanksToRemove.insert(newPos);

    if (artilleries.count(newPos) == 1)
        shellsToRemove.insert(newPos);

    artilleries[newPos] = shell;
}

std::vector<std::string> Game::splitByComma(const std::string &line)
{
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ','))
        result.push_back(item);
    return result;
}

int Game::readFile(std::string fileName)
{
    std::ifstream file(fileName);
    std::ofstream inputErrors("data/input_errors.txt");
    bool inputHasErrors = false;

    int xAxis = 0, yAxis = 0;
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    }

    getline(file, line);
    std::vector<std::string> params = splitByComma(line);
    height = std::stoi(params[0]);
    width = std::stoi(params[1]);

    std::cout << "Height: " << height << ", Width: " << width << std::endl;

    bool hasTank1 = false;
    bool hasTank2 = false;
    int rowCount = 0;

    while (std::getline(file, line))
    {
        if (rowCount >= height)
        {
            inputErrors << "Extra row beyond declared height at line " << rowCount + 2 << " was ignored.\n";
            inputHasErrors = true;
            continue;
        }

        xAxis = 0;
        for (char c : line)
        {
            if (xAxis >= width)
            {
                inputErrors << "Line " << rowCount + 2 << " exceeds declared width; ignoring chars beyond column " << width << ".\n";
                inputHasErrors = true;
                break;
            }

            int posX = xAxis * 2;
            int posY = yAxis * 2;

            if (c == '#')
            {
                addWall(posX, posY);
            }
            else if (c == '@')
            {
                addMine(posX, posY);
            }
            else if (c == '1')
            {
                if (!hasTank1)
                {
                    Tank *player1 = new Tank(posX, posY, stringToDirection["L"], this, 1);
                    addTank(player1);
                    hasTank1 = true;
                }
                else
                {
                    inputErrors << "Multiple tanks for Player 1: ignoring extra tank at (" << rowCount << "," << xAxis << ")\n";
                    inputHasErrors = true;
                }
            }
            else if (c == '2')
            {
                if (!hasTank2)
                {
                    Tank *player2 = new Tank(posX, posY, stringToDirection["R"], this, 2);
                    addTank(player2);
                    hasTank2 = true;
                }
                else
                {
                    inputErrors << "Multiple tanks for Player 2: ignoring extra tank at (" << rowCount << "," << xAxis << ")\n";
                    inputHasErrors = true;
                }
            }
            else if (c != ' ' && c != '.')
            {
                inputErrors << "Unknown symbol '" << c << "' at (" << rowCount << "," << xAxis << ") treated as empty.\n";
                inputHasErrors = true;
            }

            xAxis++;
        }

        while (xAxis < width)
        {
            inputErrors << "Line " << rowCount + 2 << " too short: filling space at column " << xAxis << ".\n";
            inputHasErrors = true;
            xAxis++;
        }

        yAxis++;
        rowCount++;
    }

    while (yAxis < height)
    {
        inputErrors << "Missing line for row " << yAxis << ": filling with empty spaces.\n";
        inputHasErrors = true;
        yAxis++;
    }

    file.close();

    if (!inputHasErrors)
        std::remove("data/input_errors.txt");
    else
        inputErrors.close();

    std::cout << "File read successfully!" << std::endl;
    return 0;
}

void Game::gameManager()
{
    TankAlgorithm *algo1 = new TankChase();
    TankAlgorithm *algo2 = new TankEvasion();

    std::unordered_map<std::string, double> stringToAngle = {
        {"a", -0.25}, {"d", 0.25}, {"q", -0.125}, {"e", 0.125}, {"x", 0}};
    int count = 0;

    while (true)
    {
        std::cout << "Game step: " << gameStep << std::endl;
        outputFile << "Step " << gameStep << ":\n";
        printBoard();

        std::set<int> wallsToRemove;

        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 2; i++)
            {
                std::set<int> tanksToRemove;
                std::set<int> shellsToRemove;
                std::unordered_map<int, Artillery *> secondaryArtilleries;
                std::unordered_map<int, Tank *> secondaryTanks;

                if (j != 1)
                {
                    for (const auto &pair : artilleries)
                    {
                        Artillery *artillery = pair.second;
                        bool didItMove = artillery->moveForward();
                        int newPos = bijection(artillery->getX(), artillery->getY());

                        if (didItMove)
                        {
                            if (secondaryArtilleries.count(newPos))
                                shellsToRemove.insert(newPos);
                            else
                                secondaryArtilleries[newPos] = artillery;
                        }
                        else
                        {
                            if (walls[newPos].health <= 0)
                                wallsToRemove.insert(newPos);
                        }
                    }

                    artilleries = secondaryArtilleries;
                }

                if (j == 1)
                {
                    Tank *t1 = nullptr;
                    Tank *t2 = nullptr;
                    for (auto &p : tanks)
                    {
                        if (p.second->getId() == 1)
                            t1 = p.second;
                        else if (p.second->getId() == 2)
                            t2 = p.second;
                    }

                    for (const auto &pair : tanks)
                    {
                        Tank *tank = pair.second;
                        Tank *opponent = tank->getId() == 1 ? t2 : t1;

                        std::string move = (tank->getId() == 1)
                                               ? algo1->computeMove(tank, opponent, this)
                                               : algo2->computeMove(tank, opponent, this);

                        tank->setLastMove(move);
                        outputFile << "  Tank " << tank->getId() << " -> " << move;

                        bool valid = true;

                        if (move == "w")
                        {
                            if (tank->checkForAWall())
                            {
                                outputFile << " (Blocked by wall — bad step)";
                                tank->ignoreMove();
                                valid = false;
                            }
                            else
                            {
                                tank->resetReverseState();
                                tank->moveForward();
                            }
                        }
                        else if (move == "s")
                        {
                            if (tank->isReverseQueued())
                            {
                                tank->incrementReverseCharge();
                                if (tank->isReverseReady())
                                    tank->executeReverse();
                            }
                            else
                            {
                                tank->queueReverse();
                                tank->incrementReverseCharge();
                            }
                        }
                        else if (move == "t")
                        {
                            if (tank->canShoot())
                            {
                                tank->fire(tanksToRemove, shellsToRemove, wallsToRemove, secondaryTanks);
                                totalShellsRemaining--;
                                tank->incrementCantShoot();
                            }
                            else
                            {
                                outputFile << " (Can't shoot — cooldown or out of ammo)";
                                tank->ignoreMove();
                                valid = false;
                            }
                        }
                        else if (stringToAngle.count(move))
                        {
                            tank->resetReverseState();
                            tank->rotateTank(stringToAngle[move]);
                        }
                        else if (move != "x")
                        {
                            outputFile << " (Unknown move)";
                            tank->ignoreMove();
                            valid = false;
                        }

                        if (valid)
                        {
                            int x = tank->getX(), y = tank->getY();
                            if (mines.count(bijection(x, y)))
                            {
                                outputFile << " (Hit a mine!)";
                                removeMine(bijection(x, y));
                                tanksToRemove.insert(bijection(x, y));
                            }
                        }

                        outputFile << "\n";
                        int currTankPos = bijection(tank->getX(), tank->getY());
                        if (secondaryTanks.count(currTankPos))
                            tanksToRemove.insert(currTankPos);
                        secondaryTanks[currTankPos] = tank;
                    }

                    tanks = secondaryTanks;
                }

                for (const auto &pair : artilleries)
                {
                    if (tanks.count(pair.first) == 1)
                        tanksToRemove.insert(pair.first);
                }

                for (int object : tanksToRemove)
                    removeTank(object);
                for (int object : shellsToRemove)
                    removeArtillery(object);
                for (int wall : wallsToRemove)
                    removeWall(wall);
            }
        }

        if (tanks.size() == 1)
        {
            outputFile << "\nGame Over! Tank " << tanks.begin()->second->getId() << " wins!\n";
            return;
        }
        else if (tanks.empty())
        {
            outputFile << "\nGame Over! It's a tie!\n";
            return;
        }
        else if (totalShellsRemaining == 0)
        {
            count++;
            if (count == 40)
            {
                outputFile << "\nGame Over! It's a tie due to time out!\n";
                return;
            }
        }

        gameStep++;
    }
}

void Game::printBoard()
{
    std::vector<std::vector<char>> board(height, std::vector<char>(width, '.'));

    for (const auto &pair : walls)
        board[pair.second.y / 2][pair.second.x / 2] = '#';

    for (const auto &mine : mines)
    {
        std::pair<int, int> pos = inverseBijection(mine);
        board[pos.second / 2][pos.first / 2] = '@';
    }

    for (const auto &pair : artilleries)
    {
        Artillery *a = pair.second;
        board[a->getY() / 2][a->getX() / 2] = '*';
    }

    for (const auto &pair : tanks)
    {
        Tank *t = pair.second;
        char id = '0' + t->getId(); // '1' or '2'
        board[t->getY() / 2][t->getX() / 2] = id;
    }

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
