#pragma once
#include "gameObject.h"

class Item : virtual public GameObject {
public:
	void draw() override;
	GLuint getTexture() override {
		return texture;
	};
	void setTexture(GLuint texture) {
		this->texture = texture;
	};
};