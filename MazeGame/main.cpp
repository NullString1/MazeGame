#include <thread>
#include "graphics.h"
#include "character.h"
#include "maze.h"

int gameLoop(Maze& maze, Character& character) {
	if (setupGraphics(maze) != 0)
		return -1;

	while (!shouldClose()) {
		render();
		for_each(maze.goals.begin(), maze.goals.end(), [&](Goal* goal) {
			if (goal->isVisible() && character.getX() == goal->getX() && character.getY() == goal->getY()) {
				character.incrementScore();
				goal->setVisible(false);
			}
		});
		for (Enemy* enemy : maze.enemies) {
			if (character.getX() == enemy->getX() && character.getY() == enemy->getY()) {
				character.decrementScore();
				character.setX(0);
				character.setY(0);
			}
			enemy->tick();
		}
	}
	return 0;
}

int main() {
	srand((unsigned int) time(0));
	const int mazeSize = 10;

	Character character;
	Maze maze(mazeSize, mazeSize, &character);

	character.setMaze(&maze);

	gameLoop(maze, character);

	close();
	return 0;
}
