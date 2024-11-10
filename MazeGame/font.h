#pragma once
#include "glad/glad.h"

void loadFont();
void drawText(const char* text, float x, float y, float scale, Shader* shader);