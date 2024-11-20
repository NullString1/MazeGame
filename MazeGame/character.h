#pragma once
#include "glad/glad.h"
#include "gameObject.h"
#include "entity.h"

class Character final : virtual public GameObject, public Entity {
public:
	unsigned int getScore() const;
	void setScore(unsigned int score);
	void incrementScore();
	void decrementScore();
	unsigned int getCollectedPeppermints() const;
	void incrementCollectedPeppermints();
	void decrementCollectedPeppermints();
	static GLuint getTexture(unsigned int a);
	GLuint getTexture() override;
	inline static GLuint texture, texture2;
	using GameObject::GameObject;
	using GameObject::setX;
	using GameObject::setY;
	using GameObject::getX;
	using GameObject::getY;
	void draw() override {
		Entity::draw();
	}
private:
	unsigned int score = 0;
	unsigned int collectedPeppermints = 0;
};