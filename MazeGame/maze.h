#pragma once
#include <vector>
#include <algorithm>
#include "character.h"
#include "goal.h"
using namespace std;

class Character; // forward declaration
class Goal; // forward declaration

class Cell {
public:
	bool walls[4] = { true, true, true, true }; // top right bottom left
	vector<Cell*> neighbours;

	Cell(unsigned int x, unsigned int y);

	bool getEdge(unsigned int edge);
	bool isVisited();
	bool setVisited(bool v);
	unsigned int getX(), getY(), setX(unsigned int x), setY(unsigned int y);

private:
	unsigned int x, y;
	bool visited = false;
};

class Maze {
public:
	Cell* current, *startPoint, *endPoint;
	Character* player;
	vector<Goal*> goals;

	Maze(unsigned int width, unsigned int height, Character* character);

	Cell* getCell(unsigned int x, unsigned int y);
	void generateMaze();
	void toVertices(vector<float>* vertices);
	unsigned int getWidth(), getHeight();
	bool isDoneGenerating();


private:
	unsigned int width, height;
	bool doneGenerating = false;
	vector<Cell*> maze;
};