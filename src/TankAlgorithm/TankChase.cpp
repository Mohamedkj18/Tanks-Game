#include "TankAlgorithm/TankChase.hpp"



TankChase::TankChase(Game *game, int numOfMovesPerPath, int range):
    TankAlgorithm(game, numOfMovesPerPath, range){}





std::vector<std::pair<int,int>> TankChase::getPath(std::pair<int,int> start, std::pair<int,int> target) {
    
    
    std::queue<std::pair<int, int>> queue;
    std::unordered_map<std::pair<int, int>, bool, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;
    std::string n;
    queue.push(start);
    visited[start] = true;

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();
        if (current == target) {
            std::vector<std::pair<int,int>> path;
            while (current != start) {
                auto p = parent[current];
                if(current!=start)path.push_back(current);
                current = p;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& dir : directions) {
            auto next = move(current, dir);
            if (!visited[next] && !isThereAMineOrAWall(next.first, next.second)) {
                visited[next] = true;
                parent[next] = current;
                queue.push(next);
            }
        }
    }

    return {};
}







std::string TankChase::getNextMove(int playerNum,int playerToChase) {
        Tank* tank = game->getPlayer(playerNum);
        std::string n;
        int x = tank->getX()/2;
        int y = tank->getY()/2;
        Direction dir = tank->getDirection(); 
        std::array<int,2> target = stringToIntDirection[dir];
        int shouldTheTankMove = isTheSquareSafe(x,y, 4);
        if(shouldTheTankMove != -1){
            moveNumToBeExecuted = numOfMovesPerPath;
            Artillery* artillery = game->getArtillery()[shouldTheTankMove];
            if(directionToString[artillery->getDirection()] == directionToString[reverseDirection[dir]])return "t";
            else if(isThereAMineOrAWall((x+target[0]+game->getWidth())%game->getWidth(),(y+target[1]+game->getHeight()))%game->getHeight())return "e";
            else return "w";
        }
        if(isTheEnemyInRange(playerNum, playerToChase)){
            moveNumToBeExecuted = numOfMovesPerPath;
            return "t";
        }
        else{
            if(moveNumToBeExecuted == numOfMovesPerPath || movesToBeExecuted[moveNumToBeExecuted] == "n")setTheMovesToBeExecuted(playerNum, playerToChase); 
            if(movesToBeExecuted[moveNumToBeExecuted] == ""){
                return "t";
            }
            return movesToBeExecuted[moveNumToBeExecuted++];
        }
}

