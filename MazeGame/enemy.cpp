#include <map>
#include "enemy.h"
#include "graphics.h"

void Enemy::tick() {
	if (this->ticksSinceLastPF > (5*this->minTicksSinceLastMove) || this->moves.empty()) {
		unsigned int target[2] = { Game::maze->player->getX(), Game::maze->player->getY()};
		std::vector<CoordPair> path = findPathBFS(Game::maze->getCell(this->getX(), this->getY()), Game::maze->getCell(target[0], target[1]));
		std::reverse(path.begin(), path.end());
		CoordPair previous = CoordPair(this->getX(), this->getY());
		for (CoordPair cell : path) {
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
		path.clear();
		this->ticksSinceLastPF = 0;
	}
	if (this->moves.size() > 0 && this->ticksSinceLastMove > this->minTicksSinceLastMove) {
		this->move(this->moves.front());
		this->moves.pop();
		this->ticksSinceLastMove = 0;
	}
	this->ticksSinceLastMove++;

};

std::vector<CoordPair> findPathBFS(Cell* startCell, Cell* goalCell) {
	std::queue<CoordPair> queue;
	std::map<CoordPair, CoordPair> cameFrom;

	CoordPair start = CoordPair(startCell->getX(), startCell->getY());
	CoordPair goal = CoordPair(goalCell->getX(), goalCell->getY());

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
			if (cameFrom.count(n) == 0) {
				queue.push(n);
				cameFrom.emplace(n, current);
			}
		}
	}
	std::vector<CoordPair> path;
	CoordPair end = goal;
	if (cameFrom.count(end) == 0) {
		return path;
	}
	while (end != CoordPair(MAXDWORD, MAXDWORD)) {
		path.emplace_back(end);
		end = cameFrom.at(end);
	}
	return path;
}

GLuint Enemy::getTexture() {
	return Enemy::texture;
}