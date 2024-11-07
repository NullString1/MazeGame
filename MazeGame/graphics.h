#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "maze.h"
#include <chrono>

int createWindow();
void render(Maze* maze);
void error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void setupVAOVBO();
float* hexColour2Float(int hexColour);
void drawLines(std::vector<float> lv, unsigned int colour);
GLuint loadDDSTexture(const char* path);
void close();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

struct DDS_PIXELFORMAT {
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

struct DDS_HEADER {
    uint32_t           dwSize;
    uint32_t           dwFlags;
    uint32_t           dwHeight;
    uint32_t           dwWidth;
    uint32_t           dwPitchOrLinearSize;
    uint32_t           dwDepth;
    uint32_t           dwMipMapCount;
    uint32_t           dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t           dwCaps;
    uint32_t           dwCaps2;
    uint32_t           dwCaps3;
    uint32_t           dwCaps4;
    uint32_t           dwReserved2;
};


