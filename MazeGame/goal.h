#pragma once
#include "glad/glad.h"
#include "gameObject.h"

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

class Peppermint: public Goal {
public:
	GLuint getTexture() override;
	inline static GLuint texture;
	using Goal::Goal;
};

class Lock : public Goal {
public:
	GLuint getTexture() override;
	inline static GLuint texture;
	using Goal::Goal;
};