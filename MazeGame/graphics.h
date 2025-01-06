#pragma once
#include <chrono>
#include <vector>
#include <random>
#include <windows.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "maze.h"
#include "shader.h"
#include "character.h"

int createWindow();
int setupGraphics();
void render();
void setupVAOVBO();
float* hexColour2Float(int hexColour);
void drawLines(const std::vector<float>& lv, bool doBuffer);
void error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLuint loadDDSTexture(const char* path);
void close();
void loadShaders();
void loadTextures();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void drawItems(const Character* chr);
void generateLineTexture();
void renderMenu(unsigned int& size);
int maze_rnd();
int maze_rnd(int min, int max);
int rnd();
int rnd(int min, int max);

enum questionState: std::uint8_t {
	SHOWN,
	ANSWERED,
	HIDDEN,
	CORRECT,
	INCORRECT
};
class Game {
public:
	inline static GLFWwindow* window;
	inline static GLuint characterTexture, characterTexture2, peppermintTexture, enemyTexture, lineTexture, lockTexture, mazeVAO, charVAO, mazeVBO, charVBO, goalVBO, goalVAO, itemVBO, itemVAO, EBO;
	inline static Shader* lineShader, * characterShader, * goalShader, * textShader, * itemShader;
	inline static std::chrono::steady_clock::time_point fps_start_t, fps_end_t, gameTimer;
	inline static Maze* maze;
	inline static constexpr int lineWidth = 10;
	inline static bool gameOver = false;
	inline static questionState questionState = HIDDEN;
	inline static std::string textInput;
	inline static unsigned int level = 1;
	inline static std::mt19937 maze_rng_mt19937, maze_rng_mt_saved, rng_mt19937, rng_mt_saved;
	inline static std::uniform_int_distribution<> maze_rng, maze_rng_saved, rng, rng_saved;
	inline static unsigned int rngSeed;
};

inline bool shouldClose() {
	return glfwWindowShouldClose(Game::window);
};

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


