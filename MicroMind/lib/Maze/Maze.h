// Maze.h
#include <Cell.h>
#include <iostream>
#include <vector>

#ifndef MAZE_H
#define MAZE_H


class Maze{
public:
    Maze(int width, int height);
    void updateCell(int x, int y, const Cell& cell);
    Cell getCell(int x, int y) const;

private:
    std::vector<std::vector<Cell>> grid;
    int width;
    int height;

};


#endif