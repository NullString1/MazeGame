#include "character.h"

unsigned int* Character::move(Direction direction) {
	Cell* currentCell = this->maze->getCell(this->pos[0], this->pos[1]);
	Cell* nextCell = nullptr;
	switch (direction) {
	case UP:
		if (this->pos[1] == 0) return this->pos;
		nextCell = this->maze->getCell(this->pos[0], this->pos[1] - 1);
		break;
	case RIGHT:
		if (this->pos[0] == this->maze->width - 1) return this->pos;
		nextCell = this->maze->getCell(this->pos[0] + 1, this->pos[1]);
		break;
	case DOWN:
		if (this->pos[1] == this->maze->height - 1) return this->pos;
		nextCell = this->maze->getCell(this->pos[0], this->pos[1] + 1);
		break;
	case LEFT:
		if (this->pos[0] == 0) return this->pos;
		nextCell = this->maze->getCell(this->pos[0] - 1, this->pos[1]);
		break;
	}
	if (!currentCell->getEdge(direction) || !nextCell->getEdge((direction+2) %4)){
		switch (direction) {
		case UP:
			this->pos[1]--;
			break;
		case RIGHT:
			this->pos[0]++;
			break;
		case DOWN:
			this->pos[1]++;
			break;
		case LEFT:
			this->pos[0]--;
			break;
		}
	}
	return this->pos;
}