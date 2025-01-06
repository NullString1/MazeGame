#pragma once
#include "maze.h"
#include "character.h"

void newLevel(unsigned int w, unsigned int h);
void loadSave();
int main();
unsigned int gameMenu();
void gameLoop(Maze& maze, Character& character);