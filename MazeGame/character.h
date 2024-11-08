#pragma once
#include "glad/glad.h"
#include "maze.h"

class Maze; // forward declaration

class Character {
public:
	enum Direction {
		UP=0,
		RIGHT=1,
		DOWN=2,
		LEFT=3
	};
	unsigned int* move(Direction direction);
	unsigned int getX();
	unsigned int getY();
	unsigned int getScore();
	void setX(unsigned int x);
	void setY(unsigned int y);
	void setScore(unsigned int score);
	void incrementScore();
	void setTexture(GLuint texture);
	void setMaze(Maze* _maze);
	void drawCharacter();
	
private:
	unsigned int score = 0;
	unsigned int pos[2] = { 0, 0 };
	Maze* maze;
	GLuint characterTexture;
};