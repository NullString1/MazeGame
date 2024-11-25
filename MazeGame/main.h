#pragma once
#include "maze.h"
#include "character.h"

void newLevel(unsigned int w, unsigned int h);
void loadSave();
int main();
int gameMenu();
int gameLoop(Maze& maze, Character& character);