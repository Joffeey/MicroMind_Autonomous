// Cell.cpp

#include <Cell.h>

void Cell::updateWalls(bool walls[4]){
    northWall = walls[0];
    eastWall = walls[1];
    southWall = walls[2];
    westWall = walls[4];
}
