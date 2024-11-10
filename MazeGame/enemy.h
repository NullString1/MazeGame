#pragma once
#include <glad/glad.h>
#include "gameObject.h"

class Enemy : public GameObject
{
public:
	void tick();
	GLuint getTexture();
	inline static GLuint texture;
	using GameObject::GameObject;
};
