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
GLuint Character::getTexture(unsigned int a) {
	return a==0 ? Character::texture:Character::texture2;
};

GLuint Character::getTexture() {
	return Character::texture;
};