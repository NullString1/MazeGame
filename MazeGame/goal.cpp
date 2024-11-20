#include "goal.h"
#include "maze.h"
#include <algorithm>
#include <random>

void Goal::setVisible(const bool visible) {
	this->visible = visible;
}
bool Goal::isVisible() const
{
	return this->visible;
}
GLuint Goal::getTexture() {
	return Goal::texture;
}
float Goal::getSize() {
	return 0.05f;
}
GLuint Peppermint::getTexture() {
	return Peppermint::texture;
}
GLuint Lock::getTexture() {
	return Lock::texture;
}
Lock::Lock(Cell* cell) {
	std::ranges::shuffle(questions, std::mt19937(std::random_device()()));
	this->question = &questions.back();
	questions.pop_back();
	this->GameObject::setX(cell->getX());
	this->GameObject::setY(cell->getY());
}

void Lock::newQuestion() {
	this->question = &questions.back();
	questions.pop_back();
}
