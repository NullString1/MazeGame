#include <thread>
#include "graphics.h"
#include "character.h"
#include "maze.h"

int gameLoop(Maze& maze, Character& character) {
	if (setupGraphics(maze) != 0)
		return -1;

	while (!shouldClose()) {
		Game::fps_start_t = std::chrono::high_resolution_clock::now();
		render();
		std::ranges::for_each(maze.goals, [&](Goal* goal) {
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
		Game::fps_end_t = std::chrono::high_resolution_clock::now();
		Sleep(static_cast<DWORD>(std::max<long long>(0, 1000 / 60 - std::chrono::duration_cast<std::chrono::milliseconds>(Game::fps_end_t - Game::fps_start_t).count()))); // 60fps = 1000/60 = 16.666ms

	}
	return 0;
}

int main() {
	srand(static_cast<unsigned int>(time(nullptr)));
	constexpr int mazeSize = 10;

	Character character;
	Maze maze(mazeSize, mazeSize, &character);

	character.setMaze(&maze);
	Game::gameTimer = std::chrono::steady_clock::now();
	gameLoop(maze, character);

	close();
	return 0;
}
