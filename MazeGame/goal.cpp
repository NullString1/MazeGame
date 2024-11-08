#include "goal.h"

Goal::Goal(unsigned int x, unsigned int y) {
	this->x = x;
	this->y = y;
}
Goal::Goal(Cell* cell) {
	this->x = cell->x;
	this->y = cell->y;
}
unsigned int Goal::getX() {
	return this->x;
}
unsigned int Goal::getY() {
	return this->y;
}
void Goal::setTexture(GLuint texture) {
	this->texture = texture;
}