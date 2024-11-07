#include "graphics.h"
#include "maze.h"
#include "character.h"
#include <thread>

int main() {
	srand(time(0));
	const int mazeSize = 16;
	Character character;
	Maze maze(mazeSize, mazeSize, &character);

	if (createWindow() != 0)
		return -1;
	
	render(&maze);
	return 0;
}