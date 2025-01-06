#include <algorithm>
#include <chrono>
#include "maze.h"
#include "goal.h"
#include "character.h"
#include "enemy.h"
#include "graphics.h"

static std::vector<Cell*> stack;

/**
 * Get a random cell from maze
 * @return pointer to random cell
 */
Cell* Maze::randCell() const { return this->getCell(maze_rnd(0, this->width - 1), maze_rnd(0, this->height - 1)); }

/**
 * Get a random cell from maze between limits
 * @param lowLimitX minimum x value
 * @param highLimitX maximum x value
 * @param lowLimitY minimum y value
 * @param highLimitY maximum y value
 * @return pointer to random cell
 */
Cell* Maze::randCell(unsigned int lowLimitX, unsigned int highLimitX, unsigned int lowLimitY,
                     unsigned int highLimitY) const {
	return this->getCell(maze_rnd(lowLimitX, highLimitX), maze_rnd(lowLimitY, highLimitY));
}

/**
 * Reset maze
 * @param resetWalls Should walls be reset. Not needed when maze was just resized
 */
void Maze::resetMaze(bool resetWalls) {
	Game::maze_rng_mt_saved = Game::maze_rng_mt19937;
	Game::maze_rng_saved = Game::maze_rng;
	Game::rng_mt_saved = Game::rng_mt19937;
	Game::rng_saved = Game::rng;
	std::ranges::for_each(peppermints, [](const Peppermint* p) { delete p; });
	std::ranges::for_each(locks, [](const Lock* l) { delete l; });
	std::ranges::for_each(enemies, [](const Enemy* e) { delete e; });
	this->doneGenerating = false;
	genCount = 0;
	this->current = this->getCell(0, 0);
	this->startPoint = nullptr;
	this->endPoint = nullptr;
	this->peppermints.clear();
	this->locks.clear();
	this->enemies.clear();
	this->enemiesGC.clear();
	if (resetWalls) {
		for (const auto cell : this->maze) {
			cell->setVisited(false);
			cell->walls[0] = true;
			cell->walls[1] = true;
			cell->walls[2] = true;
			cell->walls[3] = true;
		}
	}
	gens = this->getHeight() * this->getWidth();
	endPointGC = static_cast<unsigned int>(gens * (rnd(9, 10) / 10.0f));

	for (unsigned int i = 0; i < Game::level; i++) {
		peppermintsGC.emplace_back(static_cast<unsigned int>(gens * (rnd(2, 4) / 10.0f)));
		enemiesGC.emplace_back(static_cast<unsigned int>(gens * (rnd(6, 9) / 10.0f)));
	}
}

/**
 * Reset maze. Walls are reset
 */
void Maze::resetMaze() { this->resetMaze(true); }

/**
 * Resize maze
 * @param w New width
 * @param h New height
 */
void Maze::resizeMaze(const unsigned int w, const unsigned int h) {
	this->width = w;
	this->height = h;
	for (const auto cell : this->maze) { delete cell; }
	this->maze.clear();
	this->maze = std::vector<Cell*>(this->width * this->height);
	for (unsigned int i = 0; i < this->width; i++) {
		for (unsigned int j = 0; j < this->height; j++) {
			Cell* cell = new Cell(i, j);
			cell->setX(i);
			cell->setY(j);
			this->maze[i * this->width + j] = new Cell(i, j);
		}
	}
	for (unsigned int i = 0; i < this->width; i++) {
		for (unsigned int j = 0; j < this->height; j++) {
			Cell* cell = this->getCell(i, j);
			if (j != 0) // if not top row
				cell->neighbours.emplace_back(this->getCell(i, j - 1)); // top
			if (i != 0) // if not left column
				cell->neighbours.emplace_back(this->getCell(i - 1, j)); // left
			if (j != this->height - 1) // if not bottom row
				cell->neighbours.emplace_back(this->getCell(i, j + 1)); // bottom
			if (i != this->width - 1) // if not right column
				cell->neighbours.emplace_back(this->getCell(i + 1, j)); // right
		}
	}
	this->resetMaze(false);
}

/**
 * Generate maze using recursive backtracking
 */
void Maze::generateMaze() {
	genCount++;
	if (!this->current->isVisited()) {
		this->current->setVisited(true);
		stack.emplace_back(this->current);
	}

	std::vector<Cell*> unvisited;
	std::ranges::for_each(this->current->neighbours.begin(), this->current->neighbours.end(), [&](Cell* cell) {
		if (!cell->isVisited()) { unvisited.emplace_back(cell); }
	});

	if (!unvisited.empty()) {
		const unsigned int index = maze_rnd(0, unvisited.size()-1);
		Cell* next = unvisited[index];
		const int x = static_cast<int>(this->current->getX()) - next->getX();
		if (x == 1) {
			this->current->walls[3] = false;
			next->walls[1] = false;
		}
		else if (x == -1) {
			this->current->walls[1] = false;
			next->walls[3] = false;
		}

		const int y = this->current->getY() - next->getY();
		if (y == 1) {
			this->current->walls[0] = false;
			next->walls[2] = false;
		}
		else if (y == -1) {
			this->current->walls[2] = false;
			next->walls[0] = false;
		}

		this->current = next;
	}
	else if (!stack.empty()) {
		this->current = stack.back();
		stack.pop_back();
	}
	else {
		this->doneGenerating = true;
		this->startPoint = this->getCell(0, 0);
		for (const auto cell : this->maze) {
			cell->neighbours.clear();
			for (const GameObject::Direction d : {
				     GameObject::Direction::UP, GameObject::Direction::DOWN, GameObject::Direction::LEFT,
				     GameObject::Direction::RIGHT
			     }) {
				if (cell->getEdge(d)) { continue; }
				switch (d) {
				case GameObject::Direction::UP:
					if (cell->getY() != 0)
						cell->neighbours.emplace_back(this->getCell(cell->getX(), cell->getY() - 1));
					break;
				case GameObject::Direction::DOWN:
					if (cell->getY() != this->getHeight())
						cell->neighbours.emplace_back(this->getCell(cell->getX(), cell->getY() + 1));
					break;
				case GameObject::Direction::LEFT:
					if (cell->getX() != 0)
						cell->neighbours.emplace_back(this->getCell(cell->getX() - 1, cell->getY()));
					break;
				case GameObject::Direction::RIGHT:
					if (cell->getX() != this->getWidth())
						cell->neighbours.emplace_back(this->getCell(cell->getX() + 1, cell->getY()));
					break;
				}
			}
		}
	}

	std::erase_if(enemiesGC, [this](unsigned int x) {
		if (genCount == x) {
			this->enemies.emplace_back(new Enemy(this->current));
			return true;
		}
		return false;
	});

	std::erase_if(peppermintsGC, [this](unsigned int x) {
		if (genCount == x) {
			this->peppermints.emplace_back(new Peppermint(this->current));
			return true;
		}
		return false;
	});

	if (this->endPoint == nullptr && genCount == endPointGC) {
		this->endPoint = this->current;
		this->locks.emplace_back(new Lock(this->endPoint));
	}
}

/**
 * Cell constructor
 * @param x X coordinate
 * @param y Y coordinate
 */
Cell::Cell(unsigned int x, unsigned int y) {
	this->x = x;
	this->y = y;
}

/**
 * Get cell at coordinates
 * @param x X coordinate
 * @param y Y coordinate
 * @return Cell at coordinates
 */
Cell* Maze::getCell(unsigned int x, unsigned int y) const { return this->maze[x * this->width + y]; }

/**
 * Check if cell is visited
 * @return Is cell visited
 */
bool Cell::isVisited() const { return this->visited; }

/**
 * Set visited status of cell
 * @param v Visited status
 * @return Visited status
 */
bool Cell::setVisited(bool v) { return this->visited = v; }

/**
 * Get X coordinate of cell
 * @return X coordinate
 */
unsigned int Cell::getX() const { return this->x; }

/**
 * Get Y coordinate of cell
 * @return Y coordinate
 */
unsigned int Cell::getY() const { return this->y; }

/**
 * Maze constructor
 * @param width Width of maze
 * @param height Height of maze
 * @param character Pointer to character
 */
Maze::Maze(const unsigned int width, const unsigned int height, Character* character) {
	this->width = width;
	this->height = height;
	this->maze = std::vector<Cell*>(this->width * this->height);
	this->player = character;

	for (unsigned int i = 0; i < this->width; i++) {
		for (unsigned int j = 0; j < this->height; j++) {
			Cell* cell = new Cell(i, j);
			cell->setX(i);
			cell->setY(j);
			this->maze[i * this->width + j] = new Cell(i, j);
		}
	}
	for (unsigned int i = 0; i < this->width; i++) {
		for (unsigned int j = 0; j < this->height; j++) {
			Cell* cell = this->getCell(i, j);
			if (j != 0) // if not top row
				cell->neighbours.emplace_back(this->getCell(i, j - 1)); // top
			if (i != 0) // if not left column
				cell->neighbours.emplace_back(this->getCell(i - 1, j)); // left
			if (j != this->height - 1) // if not bottom row
				cell->neighbours.emplace_back(this->getCell(i, j + 1)); // bottom
			if (i != this->width - 1) // if not right column
				cell->neighbours.emplace_back(this->getCell(i + 1, j)); // right
		}
	}
	this->current = this->getCell(0, 0);
	gens = this->getHeight() * this->getWidth();
	endPointGC = static_cast<unsigned int>(gens * (rnd(9, 10) / 10.0f));

	for (unsigned int i = 0; i < Game::level; i++) {
		peppermintsGC.emplace_back(static_cast<unsigned int>(gens * (rnd(2, 4) / 10.0f)));
		enemiesGC.emplace_back(static_cast<unsigned int>(gens * (rnd(6, 9) / 10.0f)));
	}
}

/**
 * Maze destructor
 */
Maze::~Maze() {
	for (const auto cell : this->maze) { delete cell; }
	for (const auto peppermint : this->peppermints) { delete peppermint; }
	for (const auto enemy : this->enemies) { delete enemy; }
	for (const auto lock : this->locks) { delete lock; }
}

/**
 * Get edge of cell
 * @param edge Edge to get
 * @return Is wall present
 */
bool Cell::getEdge(unsigned int edge) const { return this->walls[edge]; }

/**
 * Get width of maze
 * @return Width of maze
 */
unsigned int Maze::getWidth() { return this->width; }

/**
 * Get height of maze
 * @return Height of maze
 */
unsigned int Maze::getHeight() { return this->height; }

/**
 * Set X coordinate of cell
 * @param x X coordinate
 * @return X coordinate
 */
unsigned int Cell::setX(unsigned int x) { return this->x = x; }

/**
 * Set Y coordinate of cell
 * @param y Y coordinate
 * @return Y coordinate
 */
unsigned int Cell::setY(unsigned int y) { return this->y = y; }

/**
 * Is maze done generating
 * @return true if maze is done generating
 */
bool Maze::isDoneGenerating() const { return this->doneGenerating; }
