#include <algorithm>
#include <chrono>
#include "maze.h"
#include "goal.h"
#include "character.h"
#include "enemy.h"

static std::vector<Cell*> stack;

static int random(const int min, const int max)
{
	return rand() % (max - min + 1) + min;
}

static float randomF(const float min, const float max)
{
	return static_cast<float>(rand()) / RAND_MAX * (max - min) + min;
}

Cell* Maze::randCell() const {
	return this->getCell(random(0, this->width - 1), random(0, this->height - 1));

}

Cell* Maze::randCell(unsigned int lowLimitX, unsigned int highLimitX, unsigned int lowLimitY, unsigned int highLimitY) const {
	return this->getCell(random(lowLimitX, highLimitX), random(lowLimitY, highLimitY));

}


void Maze::resetMaze() {
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
	for (const auto cell : this->maze) {
		cell->setVisited(false);
		cell->walls[0] = true;
		cell->walls[1] = true;
		cell->walls[2] = true;
		cell->walls[3] = true;
	}

}


void Maze::generateMaze() {
	genCount++;
	if (!this->current->isVisited()) {
		this->current->setVisited(true);
		stack.emplace_back(this->current);
	}

	std::vector<Cell*> unvisited;
	std::ranges::for_each(this->current->neighbours.begin(), this->current->neighbours.end(), [&](Cell* cell) {
		if (!cell->isVisited()) {
			unvisited.emplace_back(cell);
		}
		});

	if (!unvisited.empty()) {
		const unsigned int index = rand() % unvisited.size();
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
			for (const GameObject::Direction d : {GameObject::Direction::UP, GameObject::Direction::DOWN, GameObject::Direction::LEFT, GameObject::Direction::RIGHT}) {
				if (cell->getEdge(d)) {
					continue;
				}
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
	static const unsigned int gens = this->getHeight() * this->getWidth();
	static const unsigned int peppermintGC = static_cast<unsigned int>(gens * (random(2, 3) / 10.0f));
	static const unsigned int enemyGC = static_cast<unsigned int>(gens * (random(7, 8) / 10.0f));
	static const unsigned int endPointGC = static_cast<unsigned int>(gens * (random(9, 10) / 10.0f));

	if (this->peppermints.empty() && genCount == peppermintGC)
		this->peppermints.emplace_back(new Peppermint(this->current));
	else if (this->enemies.empty() && genCount == enemyGC)
		this->enemies.emplace_back(new Enemy(this->current));
	else if (this->endPoint == nullptr && genCount == endPointGC) {
		this->endPoint = this->current;
		this->locks.emplace_back(new Lock(this->endPoint));
	}
}

Cell::Cell(unsigned int x, unsigned int y) {
	this->x = x;
	this->y = y;
}
Cell* Maze::getCell(unsigned int x, unsigned int y) const
{
	return this->maze[x * this->width + y];
}

bool Cell::isVisited() const
{
	return this->visited;
}

bool Cell::setVisited(bool v) {
	return this->visited = v;
}

const unsigned int Cell::getX() {
	return this->x;
}

const unsigned int Cell::getY() {
	return this->y;
}

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
}
bool Cell::getEdge(unsigned int edge) const
{
	return this->walls[edge];
}

const unsigned int Maze::getWidth()  {
	return this->width;
}

const unsigned int Maze::getHeight()  {
	return this->height;
}

unsigned int Cell::setX(unsigned int x) {
	return this->x = x;
}

unsigned int Cell::setY(unsigned int y) {
	return this->y = y;
}

bool Maze::isDoneGenerating() const
{
	return this->doneGenerating;
}
