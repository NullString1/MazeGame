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
	bool visited = false;
	unsigned int x, y;

	Cell(unsigned int x, unsigned int y);
	bool getEdge(unsigned int edge);
};

class Maze {
public:
	unsigned int width, height;
	bool doneGenerating = false;
	Cell* current, *startPoint, *endPoint;
	Character* player;
	vector<Goal*> goals;
	Maze(unsigned int width, unsigned int height, Character* character);

	Cell* getCell(unsigned int x, unsigned int y);
	
	void generateMaze();
	void toVertices(vector<float>* vertices);
private:
	vector<Cell*> maze;
};