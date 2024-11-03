#include "graphics.h"
#include "maze.h"
int main() {
	const int mazeSize = 16;
	Maze maze(mazeSize, mazeSize);

	if (createWindow() != 0) 
		return -1;
	drawMaze(mazeSize, &maze);

	return 0;
}