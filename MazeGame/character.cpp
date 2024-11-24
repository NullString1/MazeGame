#include "character.h"

unsigned int Character::getScore() const
{
	return this->score;
}
void Character::setScore(const unsigned int score_) {
	this->score = score_;
}
void Character::incrementScore() {
	this->score++;
}
void Character::decrementScore() {
	if (score > 0)
		this->score--;
}

unsigned int Character::getCollectedPeppermints() const {
	return this->collectedPeppermints;
}

void Character::setCollectedPeppermints(unsigned int collectedPeppermints) {
	this->collectedPeppermints = collectedPeppermints;
}

void Character::incrementCollectedPeppermints() {
	this->collectedPeppermints++;
}

void Character::decrementCollectedPeppermints() {
	if (collectedPeppermints > 0)
		this->collectedPeppermints--;
}

GLuint Character::getTexture(unsigned int a) {
	return a == 0 ? Character::texture : Character::texture2;
};

GLuint Character::getTexture() {
	return Character::texture;
};