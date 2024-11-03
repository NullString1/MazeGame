#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "maze.h"

int createWindow();
int drawMaze(unsigned int mazeSize, Maze* maze);
void error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void setupVAOVBO();