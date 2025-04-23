#include "Core/Game.hpp"
#include "Core/Tank.hpp"
#include "Core/Artillery.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

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

int Game::getWallHealth(int wallPos){
        return walls[wallPos].health;
}

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
    return ((int)((x + y) * (x + y + 1)) * 0.5) + y;
}

std::pair<int, int> Game::inverseBijection(int z)
{
    int w = static_cast<int>(std::floor((std::sqrt(8 * z + 1) - 1) / 2));
    int t = (w * w + w) / 2;
    int y = z - t;
    int x = w - y;
    return {x, y};
}

void Game::addArtillery(Artillery *artillery)
{
    int newPos = bijection(artillery->getX(), artillery->getY());
    if (secondaryTanks.count(newPos))
    {
        std::cout << "Artillery at " << artillery->getX() << ", " << artillery->getY() << " hit a tank!" << std::endl;
        tanksToRemove.insert(newPos);
        shellsToRemove.insert(newPos);
    }
    else if (artilleries.count(newPos))
    {
        std::cout << "Artillery at " << artillery->getX() << ", " << artillery->getY() << " hit another artillery!" << std::endl;
        shellsToRemove.insert(newPos);
        return;
    }
    else if (walls.count(newPos))
    {
        std::cout << "Artillery at " << artillery->getX() << ", " << artillery->getY() << " hit a wall!" << std::endl;
        shellsToRemove.insert(newPos);

        Wall *currWall = &walls[newPos];

        if (currWall->health <= 0)
        {
            wallsToRemove.insert(newPos);
        }

        else
        {
            std::cout << "Wall already destroyed!" << std::endl;
        }
        return;
    }
    artilleries[bijection(artillery->getX(), artillery->getY())] = artillery;
}

void Game::addMine(int x, int y)
{
    mines.insert(bijection(x, y));
}

void Game::addWall(int x, int y)
{
    Wall wall = {x, y, 2};
    walls[bijection(x, y)] = wall;
}

void Game::removeMine(int x)
{
    mines.erase(x);
}

void Game::removeWall(int x)
{
    walls.erase(x);
}

void Game::removeTank(int tankPos)
{
    delete tanks[tankPos]; // cleanup
    tanks.erase(tankPos);
}

void Game::removeArtillery(int artilleryPos)
{
    delete artilleries[artilleryPos]; // cleanup
    artilleries.erase(artilleryPos);
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
                addWall(xAxis * 2, yAxis * 2);
            else if (c == '@')
                addMine(xAxis * 2, yAxis * 2);
            else if (c == '1')
            {
                Tank *player1 = new Tank(xAxis * 2, yAxis * 2, stringToDirection["L"], this, 1);
                addTank(player1);
            }
            else if (c == '2')
            {
                Tank *player2 = new Tank(xAxis * 2, yAxis * 2, stringToDirection["R"], this, 2);
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

void Game::checkForAMine(int x, int y){
    int currTankPos = bijection(x,y);
    if (mines.count(currTankPos))
        {
        removeMine(bijection(x,y));
        tanksToRemove.insert(currTankPos);
    }


}

void Game::tankHitByAShell(int tankPos)
{
    shellsToRemove.insert(tankPos);
    tanksToRemove.insert(tankPos);
}

void Game::artilleryHitAWall(int wallPos)
{
    if(getWallHealth(wallPos) <= 0)wallsToRemove.insert(wallPos);
}


void Game::advanceArtilleries(){
        Artillery* artillery;
        int newPos;
        bool didItMove;
        for(const auto &pair:artilleries){
            artillery = artilleries[pair.first];
            didItMove = artillery->moveForward();
            newPos = bijection(artillery->getX(),artillery->getY());
            if(didItMove)checkForShellCollision(artillery);
            else artilleryHitAWall(newPos);
        }
        artilleries = secondaryArtilleries;
        secondaryArtilleries.clear();
    }

    void Game::reverseHandler(Tank *tank, std::string move)
    {
        if(move == "w"){
            tank->resetReverseState();
            tank->setLastMove("x");
        }
        else if(tank->isReverseQueued()){
            tank->incrementReverseCharge();
            if(tank->isReverseReady())tank->executeReverse();
        }
    
        else if (move == "s")
        {
            tank->queueReverse();
            tank->incrementReverseCharge();
            std::cout << "Tank " << tank->getId() << " queued reverse!\n";
            if(tank->isReverseReady()){
                std::cout << "Tank " << tank->getX()   << " executed reverse!\n";
                tank->executeReverse();
            }
        }
        checkForAMine(tank->getX(),tank->getY());
    }


    void Game::advanceTank(Tank *tank)
    {
        tank->resetReverseState();
        tank->moveForward();
        checkForAMine(tank->getX(),tank->getY());
    }

    void Game::tankShootingShells(Tank *tank)
    {
        tank->resetReverseState();
        if (tank->canShoot()){
            tank->fire();
            totalShellsRemaining--;
            std::cout << "Tank " << tank->getId() << " fired!\n";
            tank->incrementCantShoot();
            }
        else{
            std::cout << "Tank " << tank->getId() << " can't shoot yet!\n";
        }
        tank->setLastMove("x");
    }

void Game::rotateArtillery(Tank *tank)
{
    tank->resetReverseState();
    tank->rotateTank(stringToAngle[tank->getLastMove()]);
    tank->setLastMove("x");
}

void Game::checkForTankCollision(Tank *tank)
{
    int currTankPos = bijection(tank->getX(), tank->getY());
    if(secondaryTanks.count(currTankPos)) tanksToRemove.insert(currTankPos);
    if(artilleries.count(currTankPos)){
        tankHitByAShell(currTankPos);
    }
    secondaryTanks[currTankPos] = tank;
}

void Game::checkForShellCollision(Artillery *artillery)
{
    int shellPos = bijection(artillery->getX(), artillery->getY());
    if (tanks.count(shellPos))tankHitByAShell(shellPos);
    if (secondaryArtilleries.count(shellPos))shellsToRemove.insert(shellPos);
    secondaryArtilleries[shellPos] = artillery;
}




void Game::executeTanksMoves(){
    std::string move;
    for (const auto pair : tanks)
    {
        
        Tank *tank = pair.second;
        
        move = tank->getLastMove();
        if (tank->getCantShoot())
        {
            tank->incrementCantShoot();
            if (tank->getCantShoot() == 8)
                tank->resetCantShoot();
        }
        if((tank->isReverseQueued() || move == "s")){
            reverseHandler(tank,move);
        }
        else if (move == "w")advanceTank(tank);
        else if (move == "t")tankShootingShells(tank);
        

        else{
            rotateArtillery(tank);
        }
        checkForTankCollision(tank);
    }

    tanks = secondaryTanks;
    secondaryTanks.clear();
}

void Game::removeTanks()
{
    for(int object : tanksToRemove){
        removeTank(object);
    }
}

void Game::removeArtilleries()
{
    for(int object : shellsToRemove){
        removeArtillery(object);
    }
}

void Game::removeWalls()
{
    for(int object : wallsToRemove){
        removeWall(object);
    }
}


void Game::removeObjectsFromTheBoard(){
    removeTanks();
    removeArtilleries();
    removeWalls();
    tanksToRemove.clear();
    shellsToRemove.clear();
    wallsToRemove.clear();
}


void Game::getPlayersInput(){
    std::string move;
    for(auto &pair : tanks){
        std::cout << "Tank " << pair.second->getId() << " move: ";
            std::cin >> move;
            pair.second->setLastMove(move);
    }
}

bool Game::checkForAWinner(){
    if (tanks.size() == 1)
        {
            std::cout << "Game Over! Player " << tanks.begin()->second->getId() << " wins!\n";
            return true;
        }
    return false;
}

bool Game::isItATie(){
    if (tanks.empty())
        {
            std::cout << "Game Over! It's a tie!\n";
            return true;
        }
    return false;

}

void Game::gameManager()
{
    
    
    int count = 0;
    
    while (true)
    {
        std::cout << "Game step: " << gameStep << std::endl;
        printBoard();
        getPlayersInput();
        
        advanceArtilleries();
        removeArtilleries();
        advanceArtilleries();
        removeObjectsFromTheBoard();

        executeTanksMoves();
        removeTanks();
        executeTanksMoves();
        removeObjectsFromTheBoard();
        
        advanceArtilleries();
        removeArtilleries();
        advanceArtilleries();
        removeObjectsFromTheBoard();



        // Victory conditions
        if (checkForAWinner())return;
        else if (isItATie())return;
        else if (totalShellsRemaining == 0)
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
    std::pair<int, int> xy;
    // Add walls
    for (const auto &pair : walls)
    {
        int x = pair.second.x / 2;
        int y = pair.second.y / 2;
        board[y][x] = '#';
    }

    // Add mines
    for (const auto &mine : mines)
    {
        xy = inverseBijection(mine);
        int x = xy.first / 2;
        int y = xy.second / 2;
        board[y][x] = '@';
    }

    // Add artillery
    for (const auto &pair : artilleries)
    {
        Artillery *a = pair.second;
        int x = a->getX() / 2;
        int y = a->getY() / 2;
        board[y][x] = '*';
    }

    // Add tanks
    for (const auto &pair : tanks)
    {
        Tank *tank = pair.second;
        int x = tank->getX() / 2;
        int y = tank->getY() / 2;
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
