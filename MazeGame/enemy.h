#pragma once
#include <glad/glad.h>
#include <queue>
#include <stack>
#include "gameObject.h"


class Enemy final : public GameObject
{
public:
	void tick();
	GLuint getTexture() override;
	inline static GLuint texture;
	using GameObject::GameObject;
	std::queue<GameObject::Direction> moves;
	unsigned int ticksSinceLastPF = 0;
	unsigned int ticksSinceLastMove = 0;
	const unsigned int minTicksSinceLastMove = 20;
};

typedef std::pair<unsigned int, unsigned int> CoordPair;

std::stack<CoordPair> findPathBFS(Cell* startCell, Cell* goalCell);