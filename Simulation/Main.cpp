#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <stack>
#include <queue>
#include "API.h"

/*  Goal positions (X, Y): (7, 7), (7, 8), (8, 7), (8, 8)
*   Starting position
*/
int currentPos[2] = { 0, 0 };
const int MAZE_WIDTH = 16;
const int MAZE_HEIGHT = 16;

std::stack<std::pair<int, int>> dfsStack;
bool isBacktracking = false;

/*  Global orientation
*   0 = North  /\
*   1 = East   >
*   2 = South  \/
*   3 = West   <
*/


const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;

struct Cell{
    bool walls[4];
    bool visited;
   Cell() {
    walls[NORTH] = false;
    walls[EAST] = false;
    walls[SOUTH] = false;
    walls[WEST] = false;
    visited = false;
   }
};

Cell maze[MAZE_HEIGHT][MAZE_WIDTH];

int currOri = 0;

bool hasWallInDirection() {
    int x = currentPos[0];
    int y = currentPos[1];
    if (currOri == 0) {         // FACING NORTH
        if (API::wallFront()) {
            maze[x][y].walls[NORTH] = true;
        }
        if (API::wallLeft()) {
            maze[x][y].walls[WEST] = true;
        }
        if (API::wallRight()) {
            maze[x][y].walls[EAST] = true;
        }
    }
    if (currOri == 1) {        // FACING EAST
        if (API::wallFront()) {
            maze[x][y].walls[EAST] = true;
        }
        if (API::wallLeft()) {
            maze[x][y].walls[NORTH] = true;
        }
        if (API::wallRight()) {
            maze[x][y].walls[SOUTH] = true;
        }
    }
    if (currOri == 2) {        // FACING SOUTH
        if (API::wallFront()) {
            maze[x][y].walls[SOUTH] = true;
        }
        if (API::wallLeft()) {
            maze[x][y].walls[EAST] = true;
        }
        if (API::wallRight()) {
            maze[x][y].walls[WEST] = true;
        }
    }
    if (currOri == 3) {     // FACING WEST
        if (API::wallFront()) {
            maze[x][y].walls[WEST] = true;
        }
        if (API::wallLeft()) {
            maze[x][y].walls[SOUTH] = true;
        }
        if (API::wallRight()) {
            maze[x][y].walls[NORTH] = true;
        }
    }
    std::cerr << "Current cell while facing: " << currOri << ", have walls: " << std::endl << "North: " << maze[x][y].walls[NORTH]
        << std::endl << "East: " << maze[x][y].walls[EAST]
        << std::endl << "South: " << maze[x][y].walls[SOUTH]
        << std::endl << "West: " << maze[x][y].walls[WEST]
        << std::endl << "Current cell is marked as visisted: " << maze[x][y].visited << std::endl;
}

bool isValidCell(int x, int y) {
    return x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_HEIGHT;
}

void checkNeighbors() {
    int x = currentPos[0];
    int y = currentPos[1];

    if (currOri == NORTH) {
        if (!API::wallLeft() && !maze[x - 1][y].visited && isValidCell(x - 1, y)) {
            std::cerr << "Current left (west) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x - 1, y });
        }
        if (!API::wallFront() && !maze[x][y + 1].visited && isValidCell(x, y + 1)) {
            std::cerr << "Current front (north) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x, y + 1 });
        }
        if (!API::wallRight() && !maze[x + 1][y].visited && isValidCell(x + 1, y)) {
            std::cerr << "Current right (east) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x + 1, y});
        }
    }
    else if (currOri == EAST) {
        if (!API::wallLeft() && !maze[x][y + 1].visited && isValidCell(x, y + 1)) {
            std::cerr << "Current left (north) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x, y + 1 });
        }
        if (!API::wallFront() && !maze[x + 1][y].visited && isValidCell(x + 1, y)) {
            std::cerr << "Current front (east) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x + 1, y});
        }
        if (!API::wallRight() && !maze[x][y - 1].visited && isValidCell(x, y - 1)) {
            std::cerr << "Current right (south) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x, y - 1 });
        }
    }
    else if (currOri == SOUTH) {
        if (!API::wallLeft() && !maze[x + 1][y].visited && isValidCell(x + 1, y)) {
            std::cerr << "Current left (east) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x + 1, y });
        }
        if (!API::wallFront() && !maze[x][y - 1].visited && isValidCell(x, y - 1)) {
            std::cerr << "Current front (south) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x, y - 1 });
        }
        if (!API::wallRight() && !maze[x - 1][y].visited && isValidCell(x - 1, y)) {
            std::cerr << "Current right (west) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x - 1, y });
        }
    }
    else if (currOri == WEST) {
        if (!API::wallLeft() && !maze[x][y - 1].visited && isValidCell(x, y - 1)) {
            std::cerr << "Current left (south) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x, y - 1 });
        }
        if (!API::wallFront() && !maze[x - 1][y].visited && isValidCell(x - 1, y)) {
            std::cerr << "Current front (west) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x - 1, y });
        }
        if (!API::wallRight() && !maze[x][y + 1].visited && isValidCell(x, y + 1)) {
            std::cerr << "Current right (north) neighbor should be checked out!" << std::endl;
            dfsStack.push({ x, y + 1 });
        }
    }
}

void moveForward(int steps = 1) {
    for (int i = 0; i < steps; i++) {
        API::moveForward();
        API::setColor(currentPos[0], currentPos[1], 'Y');
        API::setText(currentPos[0], currentPos[1], "VIS");
        maze[currentPos[0]][currentPos[1]].visited = true;
    }

}



void queueNeighbors(int x, int y) {
    if (isValidCell(x + 1, y) && !maze[x + 1][y].visited && !maze[x][y].walls[EAST]) {    // East neighbor
        dfsStack.push({ x + 1, y });
        API::setColor(x + 1, y, 'R');
        API::setText(x + 1, y, "QUE");
        std::cerr << "East neighbor queued!" << std::endl;
    }
    if (isValidCell(x - 1, y) && !maze[x - 1][y].visited && !maze[x][y].walls[WEST]) {    // West neighbor
        dfsStack.push({ x - 1, y });
        API::setColor(x - 1, y, 'R');
        API::setText(x - 1, y, "QUE");
        std::cerr << "West neighbor queued!" << std::endl;
    }
    if (isValidCell(x, y + 1) && !maze[x][y + 1].visited && !maze[x][y].walls[NORTH]) {    // North neighbor
        dfsStack.push({ x, y + 1 });
        API::setColor(x, y + 1, 'R');
        API::setText(x, y + 1, "QUE");
        std::cerr << "North neighbor queued!" << std::endl;
    }
    if (isValidCell(x, y - 1) && !maze[x][y - 1].visited && !maze[x][y].walls[SOUTH]) {   // South neighbor
        dfsStack.push({ x, y - 1 });
        API::setColor(x, y - 1, 'R');
        API::setText(x, y - 1, "QUE");
        std::cerr << "South neighbor queued!" << std::endl;
    }
}

void moveToNeighbor(int direction) {
    int dx = 0, dy = 0;
    if (direction == NORTH) {
        dy = 1;
    }
    else if (direction == EAST) {
        dx = 1;
    }
    else if (direction == SOUTH) {
        dy = -1;
    }
    else if (direction == WEST) {
        dx = -1;
    }

    int newX = currentPos[0] + dx;
    int newY = currentPos[1] + dy;

    if (isValidCell(newX, newY) && !maze[currentPos[0]][currentPos[1]].walls[direction] && !maze[newX][newY].visited) {
        if (!isBacktracking || (newX != dfsStack.top().first || newY != dfsStack.top().second)) {
            currentPos[0] = newX;
            currentPos[1] = newY;
            moveForward();
            hasWallInDirection();
            queueNeighbors(newX, newY);
            dfsStack.push({ newX, newY }); // pushing current cell, pushe neighbors?
            isBacktracking = false;
        }
    }
}

bool visitedNeighbor(int direction) {
    int dx = 0, dy = 0;
    if (direction == NORTH) {
        dy = 1;
    }
    else if (direction == EAST) {
        dx = 1;
    }
    else if (direction == SOUTH) {
        dy = -1;
    }
    else if (direction == WEST) {
        dx = -1;
    }
    int neighborX = currentPos[0] + dx;
    int neighborY = currentPos[1] + dy;

    if (maze[neighborX][neighborY].visited) {
        return true;
    }
    else {
        return false;
    }
 }

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

void logPos(int posArr[]) {
    std::cerr << "Current position (X,Y): (" << posArr[0] << ", " << posArr[1] << ")" << std::endl;
}

void logInt(int printInt) {
    std::cerr << "Current Orientation: " << currOri << std::endl;
}

void updateOri(int turnDir) {
    int sampleOri = currOri;
    currOri = (((currOri + turnDir)%4) + 4) % 4;
    std::cerr << "Current Orientation after executing: (" << sampleOri << " + " << turnDir << ") % 4 = " << currOri << std::endl;
}

void updatePos(int moveDirection = 1) {
    if (currOri == 0) {         // Facing North
        currentPos[1] = currentPos[1] + moveDirection;
    }
    else if (currOri == 1) {    // Facing East
        currentPos[0] = currentPos[0] + moveDirection;
    }
    else if (currOri == 2) {    // Facing South
        currentPos[1] = currentPos[1] - moveDirection;
    }
    else if(currOri == 3){      // Facing West
        currentPos[0] = currentPos[0] - moveDirection;
    }
}



void turnLeft(int steps = 1) {
    for (int i = 0; i < steps; i++) {
        API::turnLeft();
        updateOri(-1);
    }
}

void turnRight(int steps = 1) {
    for (int i = 0; i < steps; i++) {
        API::turnRight();
        updateOri(1);
    }
}

int randomInt(int min, int max) {
    unsigned int seed = static_cast<unsigned int>(time(0));
    srand(seed);
    return min + rand() % (max - min + 1);
}



bool hasUnvisitedNeighbors(int x, int y) {
    // Check if there are unvisited neighbors from the current cell
    return (!maze[x][y].walls[NORTH] && !maze[x][y + 1].visited) ||
        (!maze[x][y].walls[EAST] && !maze[x + 1][y].visited) ||
        (!maze[x][y].walls[SOUTH] && !maze[x][y - 1].visited) ||
        (!maze[x][y].walls[WEST] && !maze[x - 1][y].visited);
}

int getOppositeDirection(int direction) {
    return (direction + 2) % 4;
}

int getNeighborOri(int x, int y, int dx, int dy) {
    if (x > dx) {
        return WEST;
    }
    else if (x < dx) {
        return EAST;
    }
    else if (y > dy) {
        return SOUTH;
    }
    else if (y < dy) {
        return NORTH;
    }
}


int main(int argc, char* argv[]) {
    log("Running...");
    std::cerr << "0 = North" << std::endl << "1 = East" << std::endl << "2 = South" << std::endl << "3 = West" << std::endl;

    API::setColor(0, 0, 'G');
    API::setText(0, 0, "Start");

    API::setColor(7, 7, 'Y');
    API::setText(7, 7, "Goal");
    API::setColor(7, 8, 'Y');
    API::setText(7, 8, "Goal");
    API::setColor(8, 7, 'Y');
    API::setText(8, 7, "Goal");
    API::setColor(8, 8, 'Y');
    API::setText(8, 8, "Goal");

    // Initialize the visisted array to all false at start
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            maze[i][j].visited = false;
        }
    }

    // Initially pushing the only two options for next move as we spawn in the left most corner, north or east is the only two possabilities.
    dfsStack.push({ 0, 0 });
    /*
    *   Stack after initial push
    *   [0, 0]
    */

    maze[currentPos[0]][currentPos[1]].visited = true;
    std::cerr << "(X, Y):  (" << dfsStack.top().first << ", " << dfsStack.top().second << ")" << std::endl;
    bool mustTurn = false;
    bool canMove = false; // Flag to check if you can move in any direction
    bool canMoveInDir = false;
    std::cerr << "Can moveInDir is currently false: " << canMoveInDir << std::endl;

    while (!dfsStack.empty()) {
        std::pair<int, int> currentCell = dfsStack.top();
        int x = currentCell.first;
        int y = currentCell.second;

        // Check for the goal condition before making a move
        if ((x == 7 && y == 7) || (x == 7 && y == 8) || (x == 8 && y == 7) || (x == 8 && y == 8)) {
            log("Goal reached!");
            break;
        }

        maze[x][y].visited = true;

        hasWallInDirection();
        checkNeighbors();
        std::cerr << "Current position before checking anything (X, Y): (" << x << ", " << y << ")" << std::endl;

        bool canMoveInDir = false; // Reset canMoveInDir here

        if (!maze[x][y].walls[currOri]) {
            canMoveInDir = true;
            std::cerr << "CanMoveInDir set to true!" << std::endl;
        }

        if (canMoveInDir) {
            std::cerr << "CanMoveInDir is true, so moving in direction" << std::endl;
            moveToNeighbor(currOri);
            canMoveInDir = false;
            isBacktracking = false; // Reset the backtracking flag
        }
        else {
            int rightDirection = (currOri + 1) % 4;
            int leftDirection = (currOri + 3) % 4;
            int orrientationDiff = 0;
            std::cerr << "Orientation: " << currOri << ", leftDirection: " << leftDirection << ", rightDirection: " << rightDirection << std::endl;
            std::cerr << "CanMoveInDir is false, so checking sides" << std::endl;
            bool canMoveLeft = !maze[x][y].walls[leftDirection];
            bool canMoveRight = !maze[x][y].walls[rightDirection];
            int dx = dfsStack.top().first;
            int dy = dfsStack.top().second;
            int neighborOri = getNeighborOri(x, y, dx, dy);
            orrientationDiff = (neighborOri - currOri + 4) % 4;
            if (orrientationDiff == 1) {
                turnRight();
            }
            else if (orrientationDiff == 2) {
                turnRight(2);
            }
            else if (orrientationDiff == 3) {
                turnLeft();
            }
            moveToNeighbor(neighborOri);
            dfsStack.pop();

            if (canMoveLeft || canMoveRight) {
                if (canMoveLeft && canMoveRight && !visitedNeighbor(leftDirection) || !visitedNeighbor(rightDirection)) {
                    // If both left and right are possible, choose one randomly
                    int randomDirection = randomInt(0, 1); // 0 for left, 1 for right
                    if (randomDirection == 0) {
                        turnLeft();
                        moveToNeighbor(leftDirection);
                        dfsStack.pop();
                    }
                    else {
                        turnRight();
                        moveToNeighbor(rightDirection);
                        dfsStack.pop();
                    }
                }
                else if (canMoveLeft) {
                    turnLeft();
                    moveToNeighbor(leftDirection);
                    dfsStack.pop();
                }
                else if (canMoveRight) {
                    turnRight();
                    moveToNeighbor(rightDirection);
                    dfsStack.pop();
                }
                else if (canMoveLeft && canMoveRight && visitedNeighbor(leftDirection) && visitedNeighbor(rightDirection)) {
                    int oppositeDirection = getOppositeDirection(currOri);
                    dfsStack.pop();
                    turnRight(oppositeDirection);
                    currOri = oppositeDirection;
                    moveToNeighbor(currOri);
                }
                canMoveInDir = false;
                isBacktracking = false; // Reset the backtracking flag
            }
            else {
                // Dead-end, backtrack
                dfsStack.pop();
                isBacktracking = true;
                std::cerr << "Dead-end, backtracking" << std::endl;
                // Continue popping positions until you find one with unvisited neighbors
                while (!dfsStack.empty()) {
                    std::pair<int, int> previousCell = dfsStack.top();
                    int px = previousCell.first;
                    int py = previousCell.second;
                    if (hasUnvisitedNeighbors(px, py)) {
                        // Found a position with unvisited neighbors, break the loop
                        std::cerr << "*** Last cell i was in have neighbors that are not visited! ***" << std::endl;
                        break;
                    }
                    else {
                        // No unvisited neighbors, continue backtracking
                        dfsStack.pop();
                        std::cerr << "Backtracking... (X, Y): (" << px << ", " << py << ")" << std::endl;
                    }
                }
            }
        }

        // Check for the goal condition after making a move or rotation
        if ((x == 7 && y == 7) || (x == 7 && y == 8) || (x == 8 && y == 7) || (x == 8 && y == 8)) {
            log("Goal reached!");
            break;
        }

        std::cerr << "Can move in dir? " << canMoveInDir << std::endl;
    }
}
