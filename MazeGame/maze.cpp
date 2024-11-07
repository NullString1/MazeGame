#include "maze.h"
#include <chrono>

vector<Cell*> stack;

void Maze::generateMaze() {
	if (!this->current->visited) {
		this->current->visited = true;
		stack.push_back(this->current);
	}

	vector<Cell*> unvisited;
	for_each(this->current->neighbours.begin(), this->current->neighbours.end(), [&](Cell* cell) {
		if (!cell->visited) {
			unvisited.push_back(cell);
		}
	});

	if (unvisited.size() > 0) {
		unsigned int index = rand() % unvisited.size();
		Cell* next = unvisited[index];
		int x = this->current->x - next->x;
		if (x == 1) {
			this->current->walls[3] = false;
			next->walls[1] = false;
		}
		else if (x == -1) {
			this->current->walls[1] = false;
			next->walls[3] = false;
		}

		int y = this->current->y - next->y;
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
		this->endPoint = this->getCell(rand() % this->width, rand() % this->height);

	}
}