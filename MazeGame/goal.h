#pragma once
#include "glad/glad.h"
#include "maze.h"

class Cell; // forward declaration

class Goal {
public:
	Goal(unsigned int x, unsigned int y);
	Goal(Cell* cell);
	unsigned int getX();
	unsigned int getY();
	void setTexture(GLuint texture);
	void draw();
	void setVisible(bool visible) {
		this->visible = visible;
	}
	bool isVisible() {
		return this->visible;
	}
private:
	unsigned int x, y;
	GLuint texture;
	bool visible = true;
};