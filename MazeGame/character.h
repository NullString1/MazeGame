#pragma once
#include "glad/glad.h"
#include "gameObject.h"

class Character: public GameObject {
public:
	unsigned int getScore();
	void setScore(unsigned int score);
	void incrementScore();
	void decrementScore();
	GLuint getTexture();
	inline static GLuint texture;
	using GameObject::GameObject;
private:
	unsigned int score = 0;
};