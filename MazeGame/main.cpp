#include <algorithm>
#include <thread>
#include "graphics.h"
#include "character.h"
#include "maze.h"
#include "font.h"
#include "enemy.h"

int gameLoop(Maze& maze, Character& character) {
	while (!shouldClose()) {
		Game::fps_start_t = std::chrono::high_resolution_clock::now();

		render();
		if (Game::questionState == HIDDEN) {
			std::ranges::for_each(maze.peppermints, [&](Peppermint* peppermint) {
				if (peppermint->isVisible() && character.getX() == peppermint->getX() && character.getY() == peppermint->getY()) {
					character.incrementScore();
					character.incrementCollectedPeppermints();
					peppermint->setVisible(false);
				}
				});
			std::erase_if(maze.enemies, [&](Enemy* enemy) {
				if (character.getX() == enemy->getX() && character.getY() == enemy->getY()) {
					if (character.getCollectedPeppermints() == 0)
					{
						character.decrementScore();
						character.setX(0);
						character.setY(0);
					}
					else
					{
						character.decrementCollectedPeppermints();
						return true;
					}
				}
				enemy->tick();
				return false;
				}
			);
		}
		std::ranges::for_each(maze.locks, [&](Lock* lock) {
			if (lock->isVisible() && character.getX() == lock->getX() && character.getY() == lock->getY()) {
				if (Game::questionState == HIDDEN && !lock->showQuestion)
					lock->showQuestion = true;
				else if (Game::questionState == ANSWERED && lock->showQuestion) {
					lock->setVisible(false);
					lock->showQuestion = false;
					std::ranges::transform(Game::textInput, Game::textInput.begin(),
					                       [](unsigned char c) { return std::tolower(c); });
					if (Game::textInput == lock->question->second) {
						Game::questionState = CORRECT;
						Game::level++;
						character.incrementScore();
						Game::gameTimer = std::chrono::steady_clock::now();
					} else {
						Game::questionState = INCORRECT;
					}
					Game::maze->resetMaze();
					Game::textInput.clear();
					lock->newQuestion();
					character.setX(0);
					character.setY(0);
				}
			}
		});

		if (std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - Game::gameTimer).count() >= 5) {
			Game::gameOver = true;
		}

		Game::fps_end_t = std::chrono::high_resolution_clock::now();
		Sleep(static_cast<DWORD>(std::max<long long>(0, 1000 / 60 - std::chrono::duration_cast<std::chrono::milliseconds>(Game::fps_end_t - Game::fps_start_t).count()))); // 60fps = 1000/60 = 16.666ms

	}
	return 0;
}

int gameMenu() {
	if (setupGraphics() != 0)
		return -1;
	while (!shouldClose() && Game::textInput!="P" && Game::textInput!="E") {
		Game::fps_start_t = std::chrono::high_resolution_clock::now();
		renderMenu();
		Game::fps_end_t = std::chrono::high_resolution_clock::now();
		Sleep(static_cast<DWORD>(std::max<long long>(0, 1000 / 60 - std::chrono::duration_cast<std::chrono::milliseconds>(Game::fps_end_t - Game::fps_start_t).count()))); // 60fps = 1000/60 = 16.666ms
	}
	return 0;
}

int main() {
	srand(static_cast<unsigned int>(time(nullptr)));
	constexpr int mazeSize = 10;

	Character character;
	Maze maze = Maze(mazeSize, mazeSize, &character);
	Game::maze = &maze;

	Game::gameTimer = std::chrono::steady_clock::now();
	gameMenu();
	gameLoop(maze, character);

	close();
	return 0;
}
