#pragma once
#include "glad/glad.h"
#include "maze.h"

class Maze; // Forward declaration

class GameObject
{
public:
	enum Direction {
		UP = 0,
		RIGHT = 1,
		DOWN = 2,
		LEFT = 3
	};
	virtual void draw();
	virtual unsigned int** move(Direction direction);
	virtual void setX(unsigned int x);
	virtual void setY(unsigned int y);
	virtual void setMaze(Maze* _maze);
	virtual unsigned int getX();
	virtual unsigned int getY();
	virtual float getSize();
	virtual GLuint getTexture() = 0;
	GameObject(unsigned int x, unsigned int y);
	GameObject(Cell* cell);
	GameObject();
	GLuint texture;
private:
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int* pos[2] = { &x, &y };
	Maze* maze;
};