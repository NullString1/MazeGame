#pragma once
#include <vector>
#include <algorithm>
#include "character.h"
using namespace std;

class Cell {
public:
	bool walls[4] = { true, true, true, true }; // top right bottom left
	vector<Cell*> neighbours;
	bool visited = false;
	unsigned int x, y;

	Cell(unsigned int x, unsigned int y) {
		this->x = x;
		this->y = y;
	}

	bool getEdge(unsigned int edge) {
		return this->walls[edge];
	}

};

class Maze {
public:
	unsigned int width, height;
	bool doneGenerating = false;
	Cell* current, *startPoint, *endPoint;
	Character* player;

	Maze(unsigned int width, unsigned int height, Character* character) {
		this->width = width;
		this->height = height;
		this->maze = vector<Cell*>(this->width * this->height);
		this->player = character;

		for (unsigned int i = 0; i < this->width; i++) {
			for (unsigned int j = 0; j < this->height; j++) {
				Cell* cell = new Cell(i, j);
				cell->x = i;
				cell->y = j;
				this->maze[i * this->width + j] = new Cell(i, j);
			}
		}
		for (unsigned int i = 0; i < this->width; i++) {
			for (unsigned int j = 0; j < this->height; j++) {
				Cell* cell = this->getCell(i, j);
				if (j != 0) // if not top row
					cell->neighbours.push_back(this->getCell(i, j - 1)); // top
				if (i != 0) // if not left column
					cell->neighbours.push_back(this->getCell(i - 1, j)); // left
				if (j != this->height - 1) // if not bottom row
					cell->neighbours.push_back(this->getCell(i, j + 1)); // bottom
				if (i != this->width - 1) // if not right column
					cell->neighbours.push_back(this->getCell(i + 1, j)); // right
			}
		}
		this->current = this->getCell(0, 0);
	}

	Cell* getCell(unsigned int x, unsigned int y) {
		return this->maze[x * this->width + y];
	}
	void generateMaze();
	vector<float> toVertices();

private:
	vector<Cell*> maze;
};