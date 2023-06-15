#include "Grid.h"

Grid::Grid(size_t height, size_t width) : height(height), width(width), grid(nullptr)
{
    grid = new Cell* [height];

    for (size_t i = 0; i < height; i++)
        grid[i] = new Cell[width];
}

Grid::~Grid()
{
    for (size_t i = 0; i < height; i++)
        delete[] grid[i];

    delete[] grid;
}

Cell* Grid::getCellAt(size_t x, size_t y)
{
    return x < height && y < height ? &grid[y][x] : nullptr;
}

size_t Grid::getHeight()
{
    return height;
}

size_t Grid::getWidth()
{
    return width;
}
