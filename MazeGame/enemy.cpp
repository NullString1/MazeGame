#include <map>
#include "enemy.h"
#include "graphics.h"

void Enemy::tick() {
	if (this->ticksSinceLastPF > (5*this->minTicksSinceLastMove) || this->moves.empty()) {
		const unsigned int target[2] = { Game::maze->player->getX(), Game::maze->player->getY()};
		std::stack<CoordPair> path = findPathBFS(Game::maze->getCell(this->getX(), this->getY()), Game::maze->getCell(target[0], target[1]));
		CoordPair previous = CoordPair(this->getX(), this->getY());
		for (unsigned int i = 0; i<path.size(); i++) {
			const CoordPair cell = path.top();
			path.pop();
			if (cell.first > previous.first) {
				this->moves.emplace(GameObject::Direction::RIGHT);
			}
			else if (cell.first < previous.first) {
				this->moves.emplace(GameObject::Direction::LEFT);
			}
			else if (cell.second > previous.second) {
				this->moves.emplace(GameObject::Direction::DOWN);
			}
			else if (cell.second < previous.second) {
				this->moves.emplace(GameObject::Direction::UP);
			}
			previous = cell;
		}
		this->ticksSinceLastPF = 0;
	}
	if (!this->moves.empty() && this->ticksSinceLastMove > this->minTicksSinceLastMove) {
		this->move(this->moves.front());
		this->moves.pop();
		this->ticksSinceLastMove = 0;
	}
	this->ticksSinceLastMove++;

};

std::stack<CoordPair> findPathBFS(Cell* startCell, Cell* goalCell) {
	std::queue<CoordPair> queue;
	std::map<CoordPair, CoordPair> cameFrom;

	CoordPair start = CoordPair(startCell->getX(), startCell->getY());
	const CoordPair goal = CoordPair(goalCell->getX(), goalCell->getY());

	queue.push(start);
	cameFrom.emplace(start, CoordPair(MAXDWORD, MAXDWORD));
	
	while (!queue.empty()) {
		CoordPair current = queue.front();
		queue.pop();
		if (current == goal) {
			break;
		}
		for (Cell* neighbour : Game::maze->getCell(current.first, current.second)->neighbours) {
			CoordPair n = CoordPair(neighbour->getX(), neighbour->getY());
			if (!cameFrom.contains(n)) {
				queue.push(n);
				cameFrom.emplace(n, current);
			}
		}
	}
	std::stack<CoordPair> path;
	CoordPair end = goal;
	if (!cameFrom.contains(end)) {
		return path;
	}
	while (end != CoordPair(MAXDWORD, MAXDWORD)) {
		path.emplace(end);
		end = cameFrom.at(end);
	}
	return path;
}

GLuint Enemy::getTexture() {
	return Enemy::texture;
}