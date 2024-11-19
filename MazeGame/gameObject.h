#pragma once
#include "glad/glad.h"

class Cell;

class GameObject
{
public:
	virtual ~GameObject() = default;

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
	virtual unsigned int getX();
	virtual unsigned int getY();
	virtual float getSize();
	virtual GLuint getTexture() = 0;
	GameObject(unsigned int x, unsigned int y);
	explicit GameObject(Cell* cell);
	GameObject();
	GLuint texture;
private:
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int* pos[2] = { &x, &y };
};