#include "robot.h"

struct Cell {
	Cell() : robot(nullptr) {};
	Cell(Robot* robot) : robot(robot) {};

	Robot* robot;
};

class Grid
{
public:
	Grid(size_t height, size_t width);
	~Grid();

	Cell* getCellAt(size_t x, size_t y);

	size_t getHeight();
	size_t getWidth();

private:
	Cell** grid;
	const size_t height;
	const size_t width;
};