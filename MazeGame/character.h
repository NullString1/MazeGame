#pragma once
#include "glad/glad.h"
#include "maze.h"

class Maze; // forward declaration

class Character {
public:
	void drawCharacter();
	unsigned int getX() {
		return this->pos[0];
	}
	unsigned int getY() {
		return this->pos[1];
	}
	unsigned int getScore() {
		return this->score;
	}
	void setX(unsigned int x) {
		this->pos[0] = x;
	}
	void setY(unsigned int y) {
		this->pos[1] = y;
	}
	void setScore(unsigned int score) {
		this->score = score;
	}
	void incrementScore() {
		this->score++;
	}
	enum Direction {
		UP=0,
		RIGHT=1,
		DOWN=2,
		LEFT=3
	};
	unsigned int* move(Direction direction);
	void setTexture(GLuint texture) {
		this->characterTexture = texture;
	}
	void setMaze(Maze* _maze) {
		this->maze = _maze;
	}
private:
	unsigned int score = 0;
	unsigned int pos[2] = { 0, 0 };
	Maze* maze;
	GLuint characterTexture;
};