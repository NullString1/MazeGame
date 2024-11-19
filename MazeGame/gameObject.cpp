#include "gameObject.h"
#include "graphics.h"

unsigned int GameObject::getX() {
	return *this->pos[0];
}
unsigned int GameObject::getY() {
	return *this->pos[1];
}
void GameObject::setX(unsigned int x) {
	this->x = x;
}
void GameObject::setY(unsigned int y) {
	this->y = y;
}
float GameObject::getSize() {
	return 0.1f;
}
GameObject::GameObject(unsigned int x, unsigned int y) {
	this->GameObject::setX(x);
	this->GameObject::setY(y);
}
GameObject::GameObject(Cell* cell) {
	this->GameObject::setX(cell->getX());
	this->GameObject::setY(cell->getY());
}
GameObject::GameObject() = default;

unsigned int** GameObject::move(Direction direction) {
	const Cell* currentCell = Game::maze->getCell(*this->pos[0], *this->pos[1]);
	if (!currentCell->getEdge(direction)) {
		switch (direction) {
		case UP:
			this->y--;
			break;
		case DOWN:
			this->y++;
			break;
		case RIGHT:
			this->x++;
			break;
		case LEFT:
			this->x--;
			break;
		}
	}
	return this->pos;
}