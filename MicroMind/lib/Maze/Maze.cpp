// Maze.cpp
#include <Maze.h>

Maze::Maze(int width, int height) : width(width), height(height) {
    grid.resize(height, std::vector<Cell>(width));
}

void Maze::updateCell(int x, int y, const Cell& cell) {
    if(x >= 0 && x < width && y >= 0 && y < height){
        grid[y][x] = cell;
    }
    // Optionally handle out of bounds index.
}

Cell Maze::getCell(int x, int y) const {
    if( x >= 0 && x < width && y >= 0 && y < height){
        return grid[y][x];
    }
    // Handle out of bounds index. - maybe return a defautl cell or throw and exception.8
}