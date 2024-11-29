#include "gameObject.h"
#include "graphics.h"


/**
 * 
 * @return x coordinate
 */
unsigned int GameObject::getX() { return *this->pos[0]; }

/**
 *
 * @return y coordinate
 */
unsigned int GameObject::getY() { return *this->pos[1]; }

/**
 *
 * @param x x coordinate
 */
void GameObject::setX(unsigned int x) { this->x = x; }

/**
 *
 * @param y y coordinate
 */
void GameObject::setY(unsigned int y) { this->y = y; }

/**
 *
 * @return size of object
 */
float GameObject::getSize() { return 0.1f; }

/**
 * GameObject constructor. Takes x and y coordinates on maze
 */
GameObject::GameObject(unsigned int x, unsigned int y) {
	this->GameObject::setX(x);
	this->GameObject::setY(y);
}

/**
 * GameObject constructor. Takes cell on maze
 */
GameObject::GameObject(Cell* cell) {
	this->GameObject::setX(cell->getX());
	this->GameObject::setY(cell->getY());
}

GameObject::GameObject() = default;


/**
 * 
 * @param direction Direction to move object in
 * @return new position of object
 */
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
