#include "main.h"
#include <algorithm>
#include <filesystem>
#include <thread>
#include "graphics.h"
#include "character.h"
#include "maze.h"
#include "font.h"
#include "enemy.h"


/**
 * Main game loop
 * @param maze Reference to maze
 * @param character Reference to character
 */
void gameLoop(Maze& maze, Character& character) {
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
		bool shouldReset = false;
		unsigned int w, h;
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
						character.incrementScore();
						w = Game::maze->getWidth() + 1;
						h = Game::maze->getHeight() + 1;
					} else {
						Game::questionState = INCORRECT;
						w = Game::maze->getWidth();
						h = Game::maze->getHeight();
						lock->newQuestion();
					}
					Game::textInput.clear();
					shouldReset = true;
				}
			}
		});
		if (shouldReset) {
			newLevel(w, h);
			shouldReset = false;
		}

		if (std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - Game::gameTimer).count() >= 5) {
			Game::gameOver = true;
		}

		Game::fps_end_t = std::chrono::high_resolution_clock::now();
		Sleep(static_cast<DWORD>(std::max<long long>(0, 1000 / 60 - std::chrono::duration_cast<std::chrono::milliseconds>(Game::fps_end_t - Game::fps_start_t).count()))); // 60fps = 1000/60 = 16.666ms

	}
}

/**
 * Load save file
 */
void loadSave() {
	std::ifstream file(std::filesystem::current_path().append("save.conf"));
	if (file.is_open()) {
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			std::string line;
			std::getline(file, line);
			Game::level = std::stoi(line);
			std::getline(file, line);
			Game::maze->player->setScore(std::stoi(line));
			std::getline(file, line);
			Game::maze->player->setCollectedPeppermints(std::stoi(line));
			std::getline(file, line);
			auto now = std::chrono::steady_clock::now();
			auto oldTime = std::chrono::seconds(std::stoi(line));
			auto t = now - oldTime;
			Game::gameTimer = t;
			file.close();
		}
		catch (std::ifstream::failure& e) {
			std::cout << "ERROR::LOADING SAVE FILE " << e.what() << '\n';
		}
	}
}

/**
 * Game menu loop
 */
int gameMenu() {
	unsigned int size = 10;
	if (setupGraphics() != 0)
		return -1;
	while (!shouldClose() && Game::textInput!="P" && Game::textInput!="E" && Game::textInput!="L") {
		Game::fps_start_t = std::chrono::high_resolution_clock::now();
		renderMenu(size);
		if (Game::textInput == "+" && size < 15) {
			size++;
			Game::textInput.clear();
		}

		else if (Game::textInput == "-" && size > 1) {
			size--;
			Game::textInput.clear();
		}
		Game::fps_end_t = std::chrono::high_resolution_clock::now();
		Sleep(static_cast<DWORD>(std::max<long long>(0, 1000 / 60 - std::chrono::duration_cast<std::chrono::milliseconds>(Game::fps_end_t - Game::fps_start_t).count()))); // 60fps = 1000/60 = 16.666ms
	}
	if (size != 10) {
		Game::maze->resizeMaze(size, size);
	}
	if (Game::textInput == "E")
		close();
	else if (Game::textInput == "L")
		loadSave();
	else if (Game::textInput == "P") 
		return 0;
	return 0;
}

/**
 * Create new level. If width and height are the same as the old maze, reset the maze, otherwise create a new maze object
 * @param w width of maze
 * @param h height of maze
 */
static void newLevel(unsigned int w, unsigned int h) {
	if (w != Game::maze->getWidth() || h != Game::maze->getHeight()) { // if the new maze is not the same size as the old one
		Game::maze->resizeMaze(w, h);
		Game::gameTimer = std::chrono::steady_clock::now();
		Game::level++;
	}
	else { // otherwise just reset the maze
		Game::maze->resetMaze();
	}
	Game::maze->player->setX(0);
	Game::maze->player->setY(0);
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
