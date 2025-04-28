#include "TankAlgorithm/TankAlgorithm.hpp"

TankAlgorithm::TankAlgorithm(Game *game, int numOfMovesPerPath, int range){
    this->range = range;
    this->moveToAdd = 0;
    this->moveNumToBeExecuted = numOfMovesPerPath;
    this->game = game;
    this->numOfMovesPerPath = numOfMovesPerPath;
};

std::pair<int, int> TankAlgorithm::move(std::pair<int, int> current, Direction direction) {


    auto offset = stringToIntDirection[direction];
    return {(current.first + offset[0]+game->getHeight())%game->getHeight(), (current.second + offset[1]+game->getWidth())%game->getWidth()};
}



void TankAlgorithm::setTheMovesToBeExecuted(int playerNum, int playerToChase) {
    int i = 0;
    moveNumToBeExecuted = 0;
    std::string directionOfMove;
    Tank* tank = game->getPlayer(playerToChase);
    std::pair<int,int> target = {tank->getX()/2, tank->getY()/2};
    tank = game->getPlayer(playerNum);
    std::pair<int,int> start = {tank->getX()/2, tank->getY()/2};
    std::string directionOfTank = directionToString[tank->getDirection()];
    std::vector<std::pair<int,int>> positionsToGetTo = getPath(start, target);
    while(moveToAdd<numOfMovesPerPath){
        if(i>=(int)positionsToGetTo.size())break;
        directionOfMove = getDirectionFromPosition(start, positionsToGetTo[i]);
        if(directionOfMove != directionOfTank)getMovesToRotateTank(directionOfMove, directionOfTank);
        if(moveToAdd < numOfMovesPerPath)movesToBeExecuted[moveToAdd++] = "w";
        start = positionsToGetTo[i];
        directionOfTank = directionOfMove;
        i++;
    }
    moveToAdd = 0;
}

void TankAlgorithm::addMoveToBeExecuted(double angle) {
    if (angle == 0.125)movesToBeExecuted[moveToAdd++] = "e";
    else if (angle == 0.25)movesToBeExecuted[moveToAdd++] = "d";
    else if (angle == 0.375){
        movesToBeExecuted[moveToAdd++] = "d";
        if(moveToAdd >= numOfMovesPerPath)return;
        movesToBeExecuted[moveToAdd++] = "e";
    }
    else if (angle == 0.5){
        movesToBeExecuted[moveToAdd++] = "d";
        if(moveToAdd >= numOfMovesPerPath)return;
        movesToBeExecuted[moveToAdd++] = "d";
    }
    else if (angle == 0.625){
        movesToBeExecuted[moveToAdd++] = "a";
        if(moveToAdd >= numOfMovesPerPath)return;
        movesToBeExecuted[moveToAdd++] = "q";
    }
    else if (angle == 0.75)movesToBeExecuted[moveToAdd++] = "a";
    else if (angle == 0.875)movesToBeExecuted[moveToAdd++] = "q";

}


std::string TankAlgorithm::getDirectionFromPosition(std::pair<int,int> current, std::pair<int,int> target) {
    int xDiff = target.first - current.first;
    xDiff = xDiff > 1 || xDiff == -1 ? -1 : (xDiff + game->getWidth()) % game->getWidth();
    int yDiff = target.second - current.second;
    yDiff = yDiff > 1 || yDiff == -1 ? -1 : (yDiff + game->getHeight()) % game->getHeight();
    return pairToDirections[{xDiff, yDiff}];
}

int TankAlgorithm::getMovesToRotateTank(std::string directionToRotateTo, std::string currentDirection) {
    int i,movesToRotate = 0;
    double rotate = 0.125,angle;
    Direction dir = stringToDirection[currentDirection];
    Direction dirToCheck;
    for(i=0;i<8;i++){
        angle = rotate*i;
        dirToCheck = dir;
        dirToCheck += (angle);
        if(directionToString[dirToCheck] == directionToRotateTo){
            if(rotate == 0)return 0;

            else if(angle == 0.125 || angle == 0.25 || angle == 0.875 || angle == 0.75)movesToRotate++;
            else if(angle == 0.375 || angle == 0.5 || angle == 0.625)movesToRotate += 2;
            addMoveToBeExecuted(angle);
            break;
        }
    
    }
    return movesToRotate;
}



bool TankAlgorithm::isThereAMineOrAWall(int x,int y) {
    int currPos = game->bijection(2*x,2*y);
    return game->getMines().count(currPos) || game->getWalls().count(currPos);
}


bool TankAlgorithm::isTheEnemyInRange(int playerNum,int playerToChase) {
    int i;
    Tank* tank = game->getPlayer(playerToChase);
    std::pair<int,int> target = {tank->getX(), tank->getY()};
    tank = game->getPlayer(playerNum);
    std::pair<int,int> start = {tank->getX(), tank->getY()};
    std::array<int,2> offset = stringToIntDirection[tank->getDirection()];  
    for (i=1;i<range;i++){
        std::pair<int,int> newPos = {start.first + offset[0]*i, start.second + offset[1]*i};
        if(newPos == target)return true;
    }
    return false;
}


int TankAlgorithm::isTheSquareSafe(int x, int y, int rangeToCheck) {
    int xPos,yPos,currPos;
    Artillery* artillery;
        for (int i = -rangeToCheck; i <= rangeToCheck; ++i) {
            for(Direction dir: directions){
                xPos = (x + i*(stringToIntDirection[dir][0] + game->getWidth()))%game->getWidth();
                yPos = (y + i*(stringToIntDirection[dir][1]+game->getHeight()))%game->getHeight();
                currPos = game->bijection(2*xPos,2*yPos);
                if(game->getArtillery().count(currPos)){
                    artillery = game->getArtillery()[currPos];
                    if(directionToString[artillery->getDirection()] == directionToString[reverseDirection[dir]]){
                        return currPos;
                    }
                }
            }
        }
    return -1;
}