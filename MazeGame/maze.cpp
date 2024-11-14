#include <chrono>
#include "maze.h"
#include "goal.h"
#include "character.h"
#include "enemy.h"

std::vector<Cell*> stack;

int random(int min, int max) {
	return rand() % (max - min + 1) + min;
}

void Maze::generateMaze() {
	if (!this->current->isVisited()) {
		this->current->setVisited(true);
		stack.emplace_back(this->current);
	}

	std::vector<Cell*> unvisited;
	std::for_each(this->current->neighbours.begin(), this->current->neighbours.end(), [&](Cell* cell) {
		if (!cell->isVisited()) {
			unvisited.emplace_back(cell);
		}
	});

	if (unvisited.size() > 0) {
		unsigned int index = rand() % unvisited.size();
		Cell* next = unvisited[index];
		int x = this->current->getX() - next->getX();
		if (x == 1) {
			this->current->walls[3] = false;
			next->walls[1] = false;
		}
		else if (x == -1) {
			this->current->walls[1] = false;
			next->walls[3] = false;
		}

		int y = this->current->getY() - next->getY();
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
	else if (stack.size() > 0) {
		this->current = stack.back();
		stack.pop_back();
	}
	else {
		this->doneGenerating = true;
		this->startPoint = this->getCell(0, 0);
		this->endPoint = this->getCell(random(static_cast<int>(this->width-this->width*0.2), this->width-1), 
			random(static_cast<int>(this->height - this->height*0.2), this->height-1));
		this->goals.emplace_back(new Goal(this->endPoint));
		this->enemies.emplace_back(
			new Enemy(
				this->getCell(
					random(static_cast<int>(this->width - this->width * 0.5), this->width - 1),
					random(static_cast<int>(this->height - this->height * 0.5), this->height - 1)
				)
			)
		);
		for (auto cell : this->maze) {
			cell->neighbours.clear();
			for (GameObject::Direction d : {GameObject::Direction::UP, GameObject::Direction::DOWN, GameObject::Direction::LEFT, GameObject::Direction::RIGHT}) {
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
}

Cell::Cell(unsigned int x, unsigned int y) {
	this->x = x;
	this->y = y;
}
Cell* Maze::getCell(unsigned int x, unsigned int y) {
	return this->maze[x * this->width + y];
}

bool Cell::isVisited() {
	return this->visited;
}

bool Cell::setVisited(bool v) {
	return this->visited = v;
}

unsigned int Cell::getX() {
	return this->x;
}

unsigned int Cell::getY() {
	return this->y;
}

Maze::Maze(unsigned int width, unsigned int height, Character* character) {
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
bool Cell::getEdge(unsigned int edge) {
	return this->walls[edge];
}

unsigned int Maze::getWidth() {
	return this->width;
}

unsigned int Maze::getHeight() {
	return this->height;
}

unsigned int Cell::setX(unsigned int x) {
	return this->x = x;
}

unsigned int Cell::setY(unsigned int y) {
	return this->y = y;
}

bool Maze::isDoneGenerating() {
	return this->doneGenerating;
}