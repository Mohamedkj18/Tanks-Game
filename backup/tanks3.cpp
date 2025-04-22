#include "tanks3.hpp"
#include <iostream>
#include <fstream>

std::unordered_map<Direction, std::array<int, 2>> stringToIntDirection = {
    {U, {0, -1}}, {UR, {1, -1}}, {R, {1, 0}}, {DR, {1, 1}}, {D, {0, 1}}, {DL, {-1, 1}}, {L, {-1, 0}}, {UL, {-1, -1}}};

std::unordered_map<std::string, Direction> stringToDirection = {
    {"U", U}, {"UR", UR}, {"R", R}, {"DR", DR}, {"D", D}, {"DL", DL}, {"L", L}, {"UL", UL}};

std::unordered_map<Direction, Direction> reverseDirection ={
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


// ------------------------ MovingGameObject ------------------------

bool MovingGameObject::moveForward()
{
    updatePosition(direction);
    if (checkForAWall()){
        updatePosition(reverseDirection[direction]);
        return false;
    }
    return true;
}

void MovingGameObject::updatePosition(Direction dir)
{
    std::array<int, 2> d = stringToIntDirection[dir];
    x = (x + d[0] + game->getWidth()*2) % (game->getWidth()*2);
    y = (y + d[1] + game->getHeight()*2) % (game->getHeight()*2);
}

// ------------------------ Tank ------------------------

Tank::Tank(int x, int y, Direction d, Game *g,int id)
    : MovingGameObject(x, y,d,g), id(id),artilleryShells(16), destroyed(false), cantShoot(0){}

int Tank::getId() { return id; }

std::string Tank::getLastMove(){return lastMove;}

void Tank::setLastMove(std::string lastMoveStr){
    lastMove = lastMoveStr;
}

void Tank::ignoreMove()
{
    lastMove = "x";
}

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


void Tank::moveBackwards()
{
    direction = reverseDirection[direction];
    moveForward();
    direction = reverseDirection[direction];
}


bool Tank::checkForAWall() {
    updatePosition(direction);
    std::cout << "Tank " << id << " checking for a wall at " << (int)x/2 << ", " << (int)y/2 << std::endl;
    if(game->getWalls().count(game->bijection(x,y))==1){
        std::cout << "Tank " << id << " hit a wall at " << (int)x/2 << ", " << (int)y/2 << "!\n";
        updatePosition(reverseDirection[direction]);
        lastMove = "x";
        return true;
    }
    updatePosition(reverseDirection[direction]);
    return false;
}

void Tank::rotateTank(double angle)
{
    if(angle == 0)return;
    direction += angle;
}

void Tank::fire(std::set<int> &tanksToRemove,std::set<int> &shellsToRemove,std::unordered_map<int, Tank *> tanks)
{
    artilleryShells -= 1;
    Artillery *shell = new Artillery(x, y,direction, game);
    shell->moveForward();
    shell->moveForward();
    game->addArtillery(shell,tanksToRemove, shellsToRemove,tanks);
    std::cout << "Artillery fired!" << std::endl;
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


void Tank::incrementReverseCharge()
{
    if (reverseCharge < 3)
        reverseCharge++;
    else 
        reverseReady = true;
    
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
    lastMove = "s";
    reverseReady = true;
}


// ------------------------ Artillery ------------------------

Artillery::Artillery(int x, int y, Direction dir, Game *game)
    : MovingGameObject(x,y,dir,game){}

bool Artillery::checkForAWall(){
    Wall* currWall;
    int wallPos;
    std::unordered_map<int,Wall>& walls = game->getWalls();
    wallPos = game->bijection(x,y);
    if(walls.count(wallPos)){
        std::cout << "Artillery at " << x << ", " << y << "hit a wall!" << std::endl;
        currWall = &walls[wallPos];
        currWall->health -= 1;
        updatePosition(direction);
        return true;
    }
    return false;
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

std::set<int> &Game::getMines() { return mines; }

std::unordered_map<int, Wall> &Game::getWalls() { return walls; }

void Game::incrementGameStep() { gameStep++; }

void Game::addTank(Tank *tank)
{
    tanks[bijection(tank->getX(),tank->getY())] = tank;
}

int Game::bijection(int x, int y){
    return ((int)((x+y)*(x+y+1))*0.5)+y;
}

std::pair<int, int> Game::inverseBijection(int z) {
    int w = static_cast<int>(std::floor((std::sqrt(8 * z + 1) - 1) / 2));
    int t = (w * w + w) / 2;
    int y = z - t;
    int x = w - y;
    return {x, y};
}

void Game::addArtillery(Artillery *artillery,std::set<int> &tanksToRemove,std::set<int> &shellsToRemove,std::unordered_map<int, Tank *> secondaryTanks)
{
    int newPos = bijection(artillery->getX(),artillery->getY());
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
        currWall->health -= 1;
        return;
    }
    artilleries[bijection(artillery->getX(),artillery->getY())] = artillery;
}

void Game::addMine(int x, int y)
{
    mines.insert(bijection(x,y));
}

void Game::addWall(int x, int y)
{
    Wall wall = {x, y, 2};
    walls[bijection(x,y)] = wall;
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
                addWall(xAxis*2, yAxis*2);
            else if (c == '@')
                addMine(xAxis*2, yAxis*2);
            else if (c == '1')
            {
                Tank *player1 = new Tank(xAxis*2, yAxis*2, stringToDirection["L"], this, 1);
                addTank(player1);
            }
            else if (c == '2')
            {
                Tank *player2 = new Tank(xAxis*2, yAxis*2, stringToDirection["R"], this, 2);
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
    Artillery* artillery;
    int newPos;
    std::string move;
    std::unordered_map<std::string,double> stringToAngle = {{"a",-0.25},{"d",0.25},{"q",-0.125},{"e",0.125},{"x",0}};
    int count = 0;
    int i,x,y,j;
    int currTankPos;
    bool didItMove;
    


    while (true)
    {
        std::cout << "Game step: " << gameStep << std::endl;
        printBoard();
        std::set<int> wallsToRemove;
        for(j=0;j<3;j++){
        for(i=0;i<2;i++){
        //printBoard();
        std::set<int> tanksToRemove;
        std::set<int> shellsToRemove;
        std::unordered_map<int, Artillery *> secondaryArtilleries;
        std::unordered_map<int, Tank*> secondaryTanks;
        
        if(j!=1){
        for(const auto &pair:artilleries){
            artillery = artilleries[pair.first];
            didItMove = artillery->moveForward();
            if(i)std::cout << "Artillery "<< artillery->getX() << " "<< artillery->getY() << std::endl;
            newPos = bijection(artillery->getX(),artillery->getY());
            if(didItMove){
                if(secondaryArtilleries.count(newPos)) shellsToRemove.insert(newPos);
                else secondaryArtilleries[newPos] = artillery;
                }
            else{
                std::cout << "hereeeeee  " << walls[newPos].health <<std::endl;
                if(walls[newPos].health <= 0)wallsToRemove.insert(newPos);
            }
        }
        artilleries = secondaryArtilleries;
    }
        if(j==1){
        for (const auto pair : tanks)
        {
            Tank *tank = pair.second;
            
            if(!i){
                std::cout << "Tank " << tank->getId() << " move: ";
                std::cin >> move;
            } 
            else {
                move = tank->getLastMove(); 
                if(move != "w" && move != "s")move = "x";
            }
            tank->setLastMove(move);
            // Shooting cooldown handler
            if (tank->getCantShoot() > 0 && !i)
            {
                tank->incrementCantShoot();
                if (tank->getCantShoot() == 4)
                    tank->resetCantShoot();
            }
            if((tank->isReverseQueued() || move == "s") && move != "w"){
                if(tank->isReverseQueued()){
                    if(!i)tank->incrementReverseCharge();
                    if(tank->isReverseReady())tank->executeReverse();
                }
            
            else if (move == "s")

            {
                tank->queueReverse();
                if(!i)tank->incrementReverseCharge();
                std::cout << "Tank " << tank->getId() << " queued reverse!\n";
                if(tank->isReverseReady()){
                    tank->executeReverse();
                }
            }
            x= tank->getX();
            y= tank->getY();
            if (mines.count(bijection(x,y)))
                {
                    std::cout << "Tank " << tank->getId() << " hit a mine at " << x << ", " << y << "!\n";
                    removeMine(bijection(x,y));
                    tanksToRemove.insert(tank->getId());
                }
            
        }
           
            // Forward move cancels reverse streak
            else if (move == "w" && !tank->isReverseQueued() )
            {
            
                tank->resetReverseState();
                tank->moveForward();
                x = tank->getX();
                y = tank->getY();
                if (mines.count(bijection(x,y)))
                {
                    std::cout << "Tank " << tank->getId() << " hit a mine at " << x << ", " << y << "!\n";
                    removeMine(bijection(x,y));
                    tanksToRemove.insert(tank->getId());
                    continue;
                }
            }
            // Shooting cancels reverse streak

            else if (move == "t")
            {
                tank->resetReverseState();
                if (tank->canShoot())
                {
                    tank->fire(tanksToRemove, shellsToRemove, secondaryTanks);
                    totalShellsRemaining--;
                    std::cout << "Tank " << tank->getId() << " fired!\n";
                    tank->incrementCantShoot();
                }
                else
                {
                    std::cout << "Tank " << tank->getId() << " can't shoot yet!\n";
                }
            }

            else{
                if(move == "w")std::cout<<"Tank " << tank->getId() << " cancelled Reverse!\n";
                else std::cout << "Tank " << tank->getId() << " rotating to " << move << std::endl;
                tank->resetReverseState();
                tank->rotateTank(stringToAngle[move]);
            }
            currTankPos = bijection(tank->getX(),tank->getY());
            if(secondaryTanks.count(currTankPos)) tanksToRemove.insert(currTankPos);
            secondaryTanks[currTankPos] = tank;
            
        }
    
        tanks = secondaryTanks;
    }
        
        for(const auto &pair : artilleries)
        {
            if (tanks.count(pair.first) == 1)tanksToRemove.insert(pair.first);
        }

        for(int object : tanksToRemove){
            removeTank(object);
        }
        for(int object : shellsToRemove){
            removeArtillery(object);
        }

        for(int wall: wallsToRemove){
            removeWall(wall);
        }
    }
    }



        // Victory conditions
        if (tanks.size() == 1)
        {
            std::cout << "Game Over! Player " << tanks.begin()->first << " wins!\n";
            return;
        }

        else if (tanks.empty())
        {
            std::cout << "Game Over! It's a tie!\n";
            return;
        }

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
    std::pair<int,int> xy;
    // Add walls
    for (const auto &pair : walls)
    {
        int x = pair.second.x/2;
        int y = pair.second.y/2;
        board[y][x] = '#';
    }

    // Add mines
    for (const auto &mine : mines)
    {
        xy = inverseBijection(mine);
        int x = xy.first/2;
        int y = xy.second/2;
        board[y][x] = '@';
    }

    // Add artillery
    for (const auto &pair : artilleries)
    {
        Artillery *a = pair.second;
        int x = a->getX()/2;
        int y = a->getY()/2;
        board[y][x] = '*';
    }

    // Add tanks
    for (const auto &pair : tanks)
    {
        Tank *tank = pair.second;
        int x = tank->getX()/2;
        int y = tank->getY()/2;
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