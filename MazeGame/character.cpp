#include "character.h"

/**
 * 
 * @return character's score (unsigned int)
 */
unsigned int Character::getScore() const { return this->score; }


/**
 * 
 * @param score_ character's score (unsigned int)
 */
void Character::setScore(const unsigned int score_) { this->score = score_; }

/**
 * Increment character's score by 1
 */
void Character::incrementScore() { this->score++; }

/**
 * Decrement character's score by 1
 */
void Character::decrementScore() {
	if (score > 0)
		this->score--;
}

/**
 *
 * @return number of collected peppermints (unsigned int)
 */
unsigned int Character::getCollectedPeppermints() const { return this->collectedPeppermints; }

/**
 *
 * @param collectedPeppermints number of collected peppermints (unsigned int)
 */
void Character::setCollectedPeppermints(unsigned int collectedPeppermints) {
	this->collectedPeppermints = collectedPeppermints;
}

/**
 * Increment number of collected peppermints by 1
 */
void Character::incrementCollectedPeppermints() { this->collectedPeppermints++; }

/**
 * Decrement number of collected peppermints by 1
 */
void Character::decrementCollectedPeppermints() {
	if (collectedPeppermints > 0)
		this->collectedPeppermints--;
}


/**
 * 
 * @param a animation frame (unsigned int)
 * @return Texture (GLuint)
 */
GLuint Character::getTexture(unsigned int a) { return a == 0 ? Character::texture : Character::texture2; };

/**
 *
 * @return Texture (GLuint)
 */
GLuint Character::getTexture() { return Character::texture; };
