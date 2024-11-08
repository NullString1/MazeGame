#include "character.h"

unsigned int Character::getX() {
	return this->pos[0];
}
unsigned int Character::getY() {
	return this->pos[1];
}
unsigned int Character::getScore() {
	return this->score;
}
void Character::setX(unsigned int x) {
	this->pos[0] = x;
}
void Character::setY(unsigned int y) {
	this->pos[1] = y;
}
void Character::setScore(unsigned int score) {
	this->score = score;
}
void Character::incrementScore() {
	this->score++;
}
void Character::setTexture(GLuint texture) {
	this->characterTexture = texture;
}
void Character::setMaze(Maze* _maze) {
	this->maze = _maze;
}

unsigned int* Character::move(Direction direction) {
	Cell* currentCell = this->maze->getCell(this->pos[0], this->pos[1]);
	if (!currentCell->getEdge(direction)){
		switch (direction) {
		case UP:
			this->pos[1]--;
			break;
		case DOWN:
			this->pos[1]++;
			break;
		case RIGHT:
			this->pos[0]++;
			break;
		case LEFT:
			this->pos[0]--;
			break;
		}
	}
	return this->pos;
}