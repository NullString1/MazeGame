#include "goal.h"
#include "maze.h"
#include <algorithm>
#include <random>


/**
 * @param visible Should goal be drawn
 */
void Goal::setVisible(const bool visible) { this->visible = visible; }

/**
 * @return Is goal visible
 */
bool Goal::isVisible() const { return this->visible; }

/**
 * @return Texture of goal
 */
GLuint Goal::getTexture() { return Goal::texture; }

/**
 * @return Size of goal
 */
float Goal::getSize() { return 0.05f; }

/**
 * @return Peppermint's texture
 */
GLuint Peppermint::getTexture() { return Peppermint::texture; }

/**
 * @return Lock's texture
 */
GLuint Lock::getTexture() { return Lock::texture; }

/**
 * Creates new lock at cell
 * @param cell Cell to create the lock on
 */
Lock::Lock(Cell* cell) {
	std::ranges::shuffle(questions, std::mt19937(std::random_device()()));
	this->question = &questions.back();
	questions.pop_back();
	this->GameObject::setX(cell->getX());
	this->GameObject::setY(cell->getY());
}

/**
 * Picks new question from questions for the lock
 */
void Lock::newQuestion() {
	this->question = &questions.back();
	questions.pop_back();
}
