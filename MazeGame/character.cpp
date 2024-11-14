#include "character.h"

unsigned int Character::getScore() {
	return this->score;
}
void Character::setScore(unsigned int score) {
	this->score = score;
}
void Character::incrementScore() {
	this->score++;
}
void Character::decrementScore() {
	if (score > 0)
		this->score--;
}
GLuint Character::getTexture() {
	return this->texture;
};