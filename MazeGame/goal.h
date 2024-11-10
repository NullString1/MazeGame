#pragma once
#include "glad/glad.h"
#include "gameObject.h"

class Cell; // forward declaration

class Goal: public GameObject {
public:
	void setVisible(bool visible);
	bool isVisible();
	GLuint getTexture();
	float getSize();
	inline static GLuint texture;
	using GameObject::GameObject;
private:
	bool visible = true;
};