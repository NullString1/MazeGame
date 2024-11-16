#pragma once
#include "glad/glad.h"
#include "gameObject.h"

class Cell; // forward declaration

class Goal: public GameObject {
public:
	void setVisible(bool visible);
	bool isVisible() const;
	GLuint getTexture() override;
	float getSize() override;
	inline static GLuint texture;
	using GameObject::GameObject;
private:
	bool visible = true;
};