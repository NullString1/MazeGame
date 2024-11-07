#pragma once
#include "glad/glad.h"
class Character {
public:
	void drawCharacter(unsigned int _x, unsigned int _y);
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
		UP,
		RIGHT,
		DOWN,
		LEFT
	};
	unsigned int* move(Direction direction) {
		switch (direction) {
		case UP:
			this->pos[1]--;
			break;
		case RIGHT:
			this->pos[0]++;
			break;
		case DOWN:
			this->pos[1]++;
			break;
		case LEFT:
			this->pos[0]--;
			break;
		}
		return this->pos;
	}
private:
	unsigned int score;
	unsigned int x, y;
	unsigned int pos[2] = { 0, 0 };
	GLuint characterTexture;
};