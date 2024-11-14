#pragma once
#include <glad/glad.h>
#include <queue>
#include "gameObject.h"


class Enemy : public GameObject
{
public:
	void tick();
	GLuint getTexture();
	inline static GLuint texture;
	using GameObject::GameObject;
	std::queue<GameObject::Direction> moves;
	unsigned int ticksSinceLastPF = 0;
	unsigned int ticksSinceLastMove = 0;
	const unsigned int minTicksSinceLastMove = 20;
};

typedef std::pair<unsigned int, unsigned int> CoordPair;

std::vector<CoordPair> findPathBFS(Cell* startCell, Cell* goalCell);