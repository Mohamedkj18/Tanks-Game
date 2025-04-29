#include "TankAlgorithm/TankEvasion.hpp"


TankEvasion::TankEvasion(Game *game, int numOfMovesPerPath, int range):TankAlgorithm(game, numOfMovesPerPath, range){ 
    this->fleeingMode = false;
}



std::vector<std::pair<int, int>> TankEvasion::getPath(std::pair<int, int> start, std::pair<int, int> target) {
    std::stack<std::pair<int, int>> stack;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;

    
    stack.push(start);
    visited.insert(start);
    visited.insert(target);
    std::pair<int,int>fleeingTo = findFirstLegalLocationToFlee(start.first, start.second);
    
    
    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();

    
        if (current == fleeingTo) {
            std::vector<std::pair<int, int>> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current];
            }
            std::reverse(path.begin(), path.end());
            
            return path;
        }

        for (const auto& dir : directions) {
            auto next = move(current, dir);


            if (visited.find(next) == visited.end() && !isThereAMineOrAWall(next.first, next.second)) {
                stack.push(next);
                visited.insert(next);
                parent[next] = current;
            }
        }
    }

    return {};
}





std::string TankEvasion::getNextMove(int playerNum,int playerToChase) {
    std::string move;
    Tank* tank = game->getPlayer(playerNum);
    int x = tank->getX()/2;
    int y = tank->getY()/2;
    Direction dir = tank->getDirection(); 
    std::array<int,2> target = stringToIntDirection[dir];
    int shouldTheTankMove = isTheSquareSafe(x,y, 4);
    if(isTheEnemyInRange(playerNum, playerToChase)){
        moveNumToBeExecuted = numOfMovesPerPath;
        return "t";
    }
    else if (fleeingMode){
        if(moveNumToBeExecuted == numOfMovesPerPath)setTheMovesToBeExecuted(playerNum, playerToChase);
        move = movesToBeExecuted[moveNumToBeExecuted];
        moveNumToBeExecuted++;
        if(moveNumToBeExecuted == numOfMovesPerPath)fleeingMode = false;
        if(move == ""){
            fleeingMode = false;
            return "t";
        }
        return move;
    }
    else if(shouldTheTankMove != -1){
        moveNumToBeExecuted = numOfMovesPerPath;
        Artillery* artillery = game->getArtillery()[shouldTheTankMove];
        if(directionToString[artillery->getDirection()] == directionToString[reverseDirection[dir]])return "t";
        else if(isThereAMineOrAWall((x+target[0]+game->getWidth())%game->getWidth(),(y+target[1]+game->getHeight()))%game->getHeight())return "e";
        else {
            fleeingMode = true;
            return "w";
        }
    }
    else if(isThereATankClose(x,y)){
        fleeingMode = true;
        
        return "w";
    }
    else return "x";
}

bool TankEvasion::isThereATankClose(int x, int y) {
    int xPos,yPos,currPos,width,height;
    width = game->getWidth();
    height = game->getHeight();
    for (int i = -6; i <= 6 && i != 0; ++i) {
        for(Direction dir: directions){
            xPos = (x + std::abs(i)*(stringToIntDirection[dir][0] + width))%width;
            yPos = (y + std::abs(i)*(stringToIntDirection[dir][1] + height))%height;
            
            currPos = game->bijection(2*xPos,2*yPos);
            if(game->getTanks().count(currPos)){
                return true;
            }
       }
    }
    
    return false;
}

std::pair<int,int> TankEvasion::findFirstLegalLocationToFlee(int n, int m){
    int x,y;
    std::pair<int,int> fleeingTo;
    x = n,y = m;
    for (int i = -5; i <= 5 && i != 0; ++i) {
        for(Direction dir: directions){
            x = (x + std::abs(i)*(stringToIntDirection[dir][0] + game->getWidth()))%game->getWidth();
            y = (y + std::abs(i)*(stringToIntDirection[dir][1] + game->getHeight()))%game->getHeight();
            if(!isThereAMineOrAWall(x,y) && !isThereATankClose(x,y)){
                fleeingTo = {x,y};
                return fleeingTo;
            }
       }
    }
    
    return fleeingTo;
}