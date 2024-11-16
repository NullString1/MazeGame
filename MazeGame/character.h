#pragma once
#include "glad/glad.h"
#include "gameObject.h"

class Character final : public GameObject {
public:
	unsigned int getScore() const;
	void setScore(unsigned int score);
	void incrementScore();
	void decrementScore();
	GLuint getTexture(unsigned int a);
	GLuint getTexture() override;
	inline static GLuint texture, texture2;
	using GameObject::GameObject;
private:
	unsigned int score = 0;
};