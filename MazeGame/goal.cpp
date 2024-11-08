#include "goal.h"

Goal::Goal(unsigned int x, unsigned int y) {
	this->x = x;
	this->y = y;
}
Goal::Goal(Cell* cell) {
	this->x = cell->getX();
	this->y = cell->getY();
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
void Goal::setVisible(bool visible) {
	this->visible = visible;
}
bool Goal::isVisible() {
	return this->visible;
}