#pragma once
#include <glad/glad.h>
#include <queue>
#include <stack>

#include "entity.h"
#include "gameObject.h"


class Enemy final : virtual public GameObject, public Entity {
public:
	void tick();
	GLuint getTexture() override;
	inline static GLuint texture;
	using GameObject::GameObject;
	std::queue<Direction> moves;
	unsigned int ticksSinceLastPF = 0;
	unsigned int ticksSinceLastMove = 0;
	static constexpr unsigned int minTicksSinceLastMove = 20;
	void draw() override {
		Entity::draw();
	}
};

typedef std::pair<unsigned int, unsigned int> CoordPair;

std::stack<CoordPair> findPathBFS(Cell* startCell, Cell* goalCell);