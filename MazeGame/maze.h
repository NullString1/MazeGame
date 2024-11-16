#pragma once
#include <vector>

class Character; // forward declaration
class Goal; // forward declaration
class Enemy; // forward declaration

class Cell {
public:
	bool walls[4] = { true, true, true, true }; // top right bottom left
	std::vector<Cell*> neighbours;

	Cell(unsigned int x, unsigned int y);

	bool getEdge(unsigned int edge) const;
	bool isVisited() const;
	bool setVisited(bool v);
	unsigned int getX(), getY(), setX(unsigned int x), setY(unsigned int y);
	bool operator==(const Cell& other) const {
		return this->x == other.x && this->y == other.y;
	}

private:
	unsigned int x, y;
	bool visited = false;
};

class Maze {
public:
	Cell *current, *startPoint, *endPoint;
	Character* player;
	std::vector<Goal*> goals;
	std::vector<Enemy*> enemies;

	Maze(unsigned int width, unsigned int height, Character* character);

	Cell* getCell(unsigned int x, unsigned int y) const;
	void generateMaze();
	void toVertices(std::vector<float>& vertices) const;
	unsigned int getWidth(), getHeight();
	bool isDoneGenerating() const;


private:
	unsigned int width, height;
	bool doneGenerating = false;
	std::vector<Cell*> maze;
};