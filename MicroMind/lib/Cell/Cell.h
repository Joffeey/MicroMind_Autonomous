// Cell.h

#ifndef CELL_H
#define CELL_H

class Cell{
public:
    bool northWall = false;
    bool eastWall = false;
    bool westWall = false;
    bool southWall = false;
    bool visited = false;
    int distance = 0;

private:
    void updateWalls(bool walls[4]);
    void updateDistance();
};


#endif