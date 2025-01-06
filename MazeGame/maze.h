#pragma once
#include <vector>
#include "goal.h"

class Character; // forward declaration
class Peppermint; // forward declaration
class Lock; // forward declaration
class Enemy; // forward declaration

class Cell {
public:
	bool walls[4] = { true, true, true, true }; // top right bottom left
	std::vector<Cell*> neighbours;

	Cell(unsigned int x, unsigned int y);

	bool getEdge(unsigned int edge) const;
	bool isVisited() const;
	bool setVisited(bool v);
	unsigned int getX() const, getY() const;
	unsigned int setX(unsigned int x), setY(unsigned int y);
	bool operator==(const Cell& other) const {
		return this->x == other.x && this->y == other.y;
	}

private:
	unsigned int x, y;
	bool visited = false;
};

class Maze {
public:
	Cell* current, * startPoint, * endPoint;
	Character* player;
	std::vector<Peppermint*> peppermints;
	std::vector<Lock*> locks;
	std::vector<Enemy*> enemies;

	Maze(unsigned int width, unsigned int height, Character* character);
	~Maze();

	Cell* getCell(unsigned int x, unsigned int y) const;
	Cell* randCell() const;
	Cell* randCell(unsigned int lowLimitX, unsigned int highLimitX, unsigned int lowLimitY, unsigned int highLimitY) const;
	void generateMaze();
	void toVertices(std::vector<float>& vertices) const;
	unsigned int getWidth(), getHeight();
	bool isDoneGenerating() const;
	void resetMaze();
	void resetMaze(bool resetWalls);
	void resizeMaze(unsigned int w, unsigned int h);

private:
	inline static unsigned int genCount = 0;
	unsigned int width, height;
	bool doneGenerating = false;
	std::vector<Cell*> maze;
	inline static unsigned int gens, endPointGC;
	std::vector<unsigned int> enemiesGC, peppermintsGC;
};