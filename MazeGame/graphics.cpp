#define NOMINMAX
#include <typeinfo>
#include <format>
#include <filesystem>
#include "graphics.h"
#include "font.h"
#include "goal.h"
#include "enemy.h"
#include "item.h"


/**
 * Create window with GLFW and initialise OpenGL
 * @return 0 on success. -1 on error
 */
int createWindow() {
	// https://learnopengl.com/Getting-started/Hello-Window
	if (!glfwInit()) {
		fprintf_s(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Game::window = glfwCreateWindow(1000, 1000, "MazeGame", nullptr, nullptr); // 1000x1000 window
	if (!Game::window) {
		fprintf_s(stderr, "Failed to create window with GLFW\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(Game::window);
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) // Get OpenGL functions
	{
		fprintf_s(stderr, "Failed to initialize GLAD\n");
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(Game::window, &width, &height);
	glfwSetFramebufferSizeCallback(Game::window, framebuffer_size_callback);
	glfwSetKeyCallback(Game::window, key_callback);
	glViewport(0, 0, width, height);

	loadShaders();
	loadTextures();
	setupVAOVBO();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return 0;
}

/**
 * Setup graphics for game. Load textures, fonts, shaders and set up VAOs and VBOs
 * @return 0 on success, -1 on error
 */
int setupGraphics() {
	if (createWindow() == -1)
		return -1;
	loadFont();
	Peppermint::texture = Game::peppermintTexture;
	Character::texture = Game::characterTexture;
	Character::texture2 = Game::characterTexture2;
	Enemy::texture = Game::enemyTexture;
	Lock::texture = Game::lockTexture;
	return 0;
}

/**
 * Error callback for GLFW
 * @param error error code
 * @param description error description
 */
void error_callback(int error, const char* description) { fprintf(stderr, "Error: %s\n", description); }

/**
 * Callback for window resizing. Changes viewport size
 * @param window window
 * @param width new width
 * @param height new height
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

/**
 * Process input from window. Used to detect ESC key press
 * @param window window
 */
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

/**
 * Callback for key presses. Used to move player, and accept text input for questions
 * @param window window (unused)
 * @param key key pressed
 * @param scancode scancode (unused)
 * @param action action
 * @param mods mods (unused)
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Game::questionState == HIDDEN && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		switch (key) {
		case GLFW_KEY_W:
			Game::maze->player->move(Character::UP);
			break;
		case GLFW_KEY_S:
			Game::maze->player->move(Character::DOWN);
			break;
		case GLFW_KEY_A:
			Game::maze->player->move(Character::LEFT);
			break;
		case GLFW_KEY_D:
			Game::maze->player->move(Character::RIGHT);
			break;
		case GLFW_KEY_P:
			Game::textInput = "P";
			break;
		case GLFW_KEY_E:
			Game::textInput = "E";
			break;
		case GLFW_KEY_L:
			Game::textInput = "L";
			break;
		case GLFW_KEY_KP_ADD:
			Game::textInput = "+";
			break;
		case GLFW_KEY_KP_SUBTRACT:
			Game::textInput = "-";
			break;
		default:
			break;
		}
	}
	else if (Game::questionState != HIDDEN && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		if (key == GLFW_KEY_ENTER) { Game::questionState = ANSWERED; }
		else if (key == GLFW_KEY_BACKSPACE) {
			if (!Game::textInput.empty())
				Game::textInput.pop_back();
		}
		else if ((key >= GLFW_KEY_A && key <= GLFW_KEY_Z) || (GLFW_KEY_0 <= key && key <= GLFW_KEY_9)) {
			Game::textInput += static_cast<char>(key);
		}
	}
}

/**
 * Convert hex colour to normalised float array (rgba) (-1.0f to 1.0f)
 */
float* hexColour2Float(const int hexColour) {
	float r = ((hexColour >> 24) & 0xFF) / 255.0f;
	float g = ((hexColour >> 16) & 0xFF) / 255.0f;
	float b = ((hexColour >> 8) & 0xFF) / 255.0f;
	float a = (hexColour & 0xFF) / 255.0f;
	static float out[4] = {r, g, b, a};
	return out;
}


/**
 * Normalise maze coordinates to OpenGL coordinates
 * @param x Input x
 * @param y Input y
 * @param _x Output x 
 * @param _y Output y
 */
static void normaliseGraphCoords(const unsigned int x, const unsigned int y, float& _x, float& _y) {
	_x = -0.9f + x * 0.1f + 0.2f;
	_y = 0.9f - y * 0.1f;
}

[[maybe_unused]] static float* normaliseGraphCoords(const unsigned int x, const unsigned int y) {
	static float out[2];
	out[0] = -0.9f + x * 0.1f + 0.2f;
	out[1] = 0.9f - y * 0.1f;
	return out;
}


/**
 * Load shaders 
 */
void loadShaders() {
	Game::lineShader = new Shader("lineVertex.glsl", "lineFrag.glsl");
	Game::characterShader = new Shader("characterVertex.glsl", "characterFrag.glsl");
	Game::goalShader = new Shader("goalVertex.glsl", "goalFrag.glsl");
	Game::textShader = new Shader("textVertex.glsl", "textFrag.glsl");
	Game::itemShader = new Shader("itemVertex.glsl", "itemFrag.glsl");
}

/**
 * Load textures and generate line texture
 */
void loadTextures() {
	Game::characterTexture = loadDDSTexture(R"(character1.DDS)");
	Game::characterTexture2 = loadDDSTexture(R"(character2.DDS)");
	Game::peppermintTexture = loadDDSTexture(R"(peppermint.DDS)");
	Game::enemyTexture = loadDDSTexture(R"(farquad.DDS)");
	Game::lockTexture = loadDDSTexture(R"(lock.DDS)");
	generateLineTexture();
}

/**
 * Setup Vertex Array Objects and Vertex Buffer Objects
 */
void setupVAOVBO() {
	glGenVertexArrays(1, &Game::mazeVAO);
	glGenVertexArrays(1, &Game::charVAO);
	glGenVertexArrays(1, &Game::goalVAO);
	glGenVertexArrays(1, &Game::itemVAO);

	glGenBuffers(1, &Game::mazeVBO);
	glGenBuffers(1, &Game::charVBO);
	glGenBuffers(1, &Game::goalVBO);
	glGenBuffers(1, &Game::itemVBO);
	glGenBuffers(1, &Game::EBO);

	glBindVertexArray(Game::mazeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::mazeVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	// Position attribute = 0, 2 floats, no normalisation

	glEnableVertexAttribArray(0); // Enable vertex attribute

	glBindVertexArray(Game::charVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::charVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	// Position attribute = 0, 2 floats, no normalisation, 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	// Texture coords attribute = 0, 2 floats, no normalisation, 
	//no stride (space between values), offset of previous 3 floats
	glEnableVertexAttribArray(0); // Enable vertex attribute
	glEnableVertexAttribArray(1); // Enable texture attribute

	glBindVertexArray(Game::goalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::goalVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	// Position attribute = 0, 2 floats, no normalisation, 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	// Texture coords attribute = 0, 2 floats, no normalisation, 

	glEnableVertexAttribArray(0); // Enable vertex attribute
	glEnableVertexAttribArray(1); // Enable texture attribute

	glBindVertexArray(Game::itemVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::itemVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	// Position attribute = 0, 2 floats, no normalisation, 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	// Texture coords attribute = 0, 2 floats, no normalisation,

	glEnableVertexAttribArray(0); // Enable vertex attribute
	glEnableVertexAttribArray(1); // Enable texture attribute
}

/**
 * Generate line texture for maze. Creates a 1D texture with alternating red and white lines
 */
void generateLineTexture() {
	unsigned char* data = new unsigned char[Game::lineWidth * Game::lineWidth * 4];
	for (unsigned int i = 0; i < Game::lineWidth * Game::lineWidth * 4; i += 4) {
		if ((i / 4) % 2 == 0) {
			data[i] = 255; // Red
			data[i + 1] = 0; // Green
			data[i + 2] = 0; // Blue
		}
		else {
			data[i] = 255; // Red
			data[i + 1] = 255; // Green
			data[i + 2] = 255; // Blue
		}
		data[i + 3] = 255; // Alpha
	}

	glGenTextures(1, &Game::lineTexture);
	glBindTexture(GL_TEXTURE_1D, Game::lineTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, Game::lineWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_1D);
	delete[] data;
}

/**
 * Load DDS texture from file
 * @param path path to DDS file
 * @return texture ID
 */
GLuint loadDDSTexture(const char* path) {
	DDS_HEADER fileHeader;
	FILE* fp;

	if (fopen_s(&fp, path, "rb") != 0) {
		fprintf_s(stderr, "Failed to open file\n");
		return -1;
	}

	char fileCode[4];
	fread_s(fileCode, 4, 4, 1, fp);
	if (memcmp(fileCode, "DDS ", 4) != 0) {
		fprintf_s(stderr, "Not a DDS file\n");
		fclose(fp);
		return -1;
	}

	fread_s(&fileHeader, sizeof(fileHeader), 124, 1, fp);

	unsigned int bufferSize = fileHeader.dwMipMapCount > 1
		                          ? fileHeader.dwPitchOrLinearSize * 2
		                          : fileHeader.dwPitchOrLinearSize;
	unsigned char* mipMapBuffer = new unsigned char[bufferSize];
	fread_s(mipMapBuffer, bufferSize, 1, bufferSize, fp);
	fclose(fp);

	unsigned int format;

	switch (fileHeader.ddspf.dwFourCC) {
	case 0x31545844: // DXT1
		format = 0x83F1;
		break;
	case 0x33545844: // DXT3
		format = 0x83F2;
		break;
	case 0x35545844: // DXT5
		format = 0x83F3;
		break;
	default:
		delete[] mipMapBuffer;
		return -1;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int blockSize = (format == 0x83F1) ? 8 : 16;
	unsigned int offset = 0;

	for (unsigned int level = 0; level < fileHeader.dwMipMapCount && (fileHeader.dwWidth || fileHeader.dwHeight); ++
	     level) {
		unsigned int size = ((fileHeader.dwWidth + 3) / 4) * ((fileHeader.dwHeight + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, fileHeader.dwWidth, fileHeader.dwHeight, 0, size,
		                       mipMapBuffer + offset);
		offset += size;
		fileHeader.dwWidth /= 2;
		fileHeader.dwHeight /= 2;
	}
	delete[] mipMapBuffer;
	return textureID;
}


/**
 * Draw maze lines on screen
 * @param lv vector of line vertices 
 * @param doBuffer whether to buffer data to gpu. Not needed if maze is not changing
 */
void drawLines(const std::vector<float>& lv, const bool doBuffer) {
	Game::lineShader->use();
	glBindVertexArray(Game::mazeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::mazeVBO);
	glBindTexture(GL_TEXTURE_1D, Game::lineTexture);
	if (doBuffer) { glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lv.size(), lv.data(), GL_STATIC_DRAW); }
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lv.size()) / 2);
}

int maze_rnd(int min, int max) {
	return maze_rnd() % (max - min + 1) + min;
}

int maze_rnd() {
	return Game::maze_rng(Game::maze_rng_mt19937);
}

int rnd(int min, int max) {
	return rnd() % (max - min + 1) + min;
}

int rnd() {
	return Game::rng(Game::rng_mt19937);
}


/**
 * Draw GameObject on screen at x, y. Uses goalShader
 */
void GameObject::draw() {
	float x, y;
	normaliseGraphCoords(this->x, this->y, x, y);
	std::vector<float> vertices;
	vertices.reserve(20);

	const float size = this->getSize();

	glBindVertexArray(Game::goalVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->getTexture());
	Game::goalShader->use();
	Game::goalShader->setInt("texture1", 0);
	vertices = {
		x - 0.025f, y - 0.025f, 0.0f, 1.0f, 1.0f, // top right
		x - 0.025f, y - size - 0.025f, 0.0f, 1.0f, 0.0f, // bottom right
		x - size - 0.025f, y - size - 0.025f, 0.0f, 0.0f, 0.0f, // bottom left
		x - size - 0.025f, y - 0.025f, 0.0f, 0.0f, 1.0f, // top left

	};

	const std::vector<unsigned int> indices = {
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	glBindBuffer(GL_ARRAY_BUFFER, Game::goalVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Game::EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

/**
 * Draw Entity on screen
 */
void Entity::draw() {
	float x, y;
	normaliseGraphCoords(this->getX(), this->getY(), x, y);
	std::vector<float> vertices;
	vertices.reserve(20);

	const float size = this->getSize();

	glActiveTexture(GL_TEXTURE0);

	Character* chr = dynamic_cast<Character*>(this);
	if (chr != nullptr)
		glBindTexture(GL_TEXTURE_2D, chr->getTexture(rand() % 2));
	else
		glBindTexture(GL_TEXTURE_2D, this->getTexture());

	Game::characterShader->use();
	Game::characterShader->setInt("texture1", 0);

	vertices = {
		x, y, 0.0f, 1.0f, 1.0f, // top right
		x, y - size, 0.0f, 1.0f, 0.0f, // bottom right
		x - size, y - size, 0.0f, 0.0f, 0.0f, // bottom left
		x - size, y, 0.0f, 0.0f, 1.0f, // top left

	};

	const std::vector<unsigned int> indices = {
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	glBindVertexArray(Game::charVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::charVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Game::EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

/**
 * Draw Item on screen. Used to draw collected peppermints 
 */
void Item::draw() {
	Game::itemShader->use();
	Game::itemShader->setInt("texture1", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->getTexture());
	glBindVertexArray(Game::itemVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::itemVBO);
	const glm::mat4 projection = glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f, -1.0f, 1.0f);
	Game::itemShader->setMat4("projection", projection);

	float x = static_cast<float>(this->getX());
	float y = 1000.0f - 20.0f;
	constexpr float size = 25.0f;

	std::vector<float> vertices = {
		x, y, 0.0f, 1.0f, 1.0f, // top right
		x, y - size, 0.0f, 1.0f, 0.0f, // bottom right
		x - size, y - size, 0.0f, 0.0f, 0.0f, // bottom left
		x - size, y, 0.0f, 0.0f, 1.0f, // top left

	};

	const std::vector<unsigned int> indices = {
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	glBindVertexArray(Game::itemVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::itemVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Game::EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}


/**
 * Generate vector of line vertices from maze
 * @param vertices reference to output vector
 */
void Maze::toVertices(std::vector<float>& vertices) const {
	vertices.clear();
	for (unsigned int i = 0; i < this->height; i++) {
		// maze->height
		for (unsigned int j = 0; j < this->width; j++) {
			// maze->width
			const Cell* cell = this->getCell(j, i);
			const float x = -0.9f + j * 0.1f + 0.1f;
			const float y = 0.9f - i * 0.1f;
			constexpr float length = 0.1f;
			constexpr float space = 0.1f;
			std::vector<float> v;
			// draw top edge
			if (cell->getEdge(0)) {
				v = {x, y, x + length, y};
				vertices.insert(vertices.end(), v.begin(), v.end());
			}
			// draw left edge
			if (cell->getEdge(3)) {
				v = {x, y, x, y - length};
				vertices.insert(vertices.end(), v.begin(), v.end());
			}
			// draw bottom edge
			if (cell->getEdge(2)) {
				v = {x, y - space, x + length, y - space};
				vertices.insert(vertices.end(), v.begin(), v.end());
			}
			// draw right edge
			if (cell->getEdge(1)) {
				v = {x + space, y, x + space, y - length};
				vertices.insert(vertices.end(), v.begin(), v.end());
			}
		}
	}
}


/**
 * Draw player's score 
 * @param chr pointer to character
 */
static void drawScore(const Character* chr) {
	drawText(std::format("Score: {}", chr->getScore()).c_str(), 20.0f, 40.0f, 1.0f, Game::textShader);
}

/**
 * Draw timer on screen
 * @param t time point to calculate time since
 */
static void drawTimer(const std::chrono::steady_clock::time_point& t) {
	auto time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - t).count();
	auto minutes = time / 60;
	auto seconds = time % 60;

	drawText(std::format("Time: {}m {}s / 5m", minutes, seconds).c_str(), 140.0f, 40.0f, 1.0f, Game::textShader);
}

/**
 * Draw collected items on screen
 * @param chr pointer to character
 */
static void drawItems(const Character* chr) {
	drawText("Items:", 385.0f, 40.0f, 1.0f, Game::textShader);
	for (unsigned int i = 0; i < chr->getCollectedPeppermints(); i++) {
		Item item;
		item.setTexture(Peppermint::texture);
		item.setX(495 + i * 25);
		item.draw();
	}
}


/**
 * Render game. Draws maze, player, enemies, locks, peppermints, score, timer, items and text
 */
void render() {
	static std::vector<float> v;

	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (!Game::maze->isDoneGenerating()) {
		Game::maze->generateMaze();
		Game::maze->toVertices(v);
	}
	else {
		std::ranges::for_each(Game::maze->peppermints, [&](Peppermint* peppermint) {
			if (peppermint->isVisible())
				peppermint->draw();
		});
		std::ranges::for_each(Game::maze->locks, [&](Lock* lock) {
			lock->draw();
			if (lock->showQuestion) {
				drawText(std::format("{} {}", lock->question->first, Game::textInput).c_str(),
				         200.0f, 700.0f, 1.0f, Game::textShader);
				Game::questionState = SHOWN;
			}
			if (Game::questionState == CORRECT) {
				drawText("Correct! +1 Point", 200.0f, 700.0f, 1.0f, Game::textShader);
				Game::questionState = HIDDEN;
			}
			else if (Game::questionState == INCORRECT) {
				drawText("Incorrect! Restarting..", 200.0f, 700.0f, 1.0f, Game::textShader);
				Game::questionState = HIDDEN;
			}
		});
		std::ranges::for_each(Game::maze->enemies, [&](Enemy* enemy) { enemy->draw(); });
	}

	drawLines(v, !Game::maze->isDoneGenerating());
	Game::maze->player->draw();
	drawScore(Game::maze->player);
	drawTimer(Game::gameTimer);
	drawItems(Game::maze->player);
	drawText(std::format("Level: {}", Game::level).c_str(), 500.0f, 40.0f, 1.0f, Game::textShader);
	drawText("Press ESC to exit and save", 650.0f, 40.0f, 1.0f, Game::textShader);

	if (Game::gameOver)
		drawText("Game Over", 150.0f, 250.0f, 2.0f, Game::textShader);

	processInput(Game::window);
	glfwSwapBuffers(Game::window);
	glfwPollEvents();
}

/**
 * Render menu. Draws menu with options to play, load and exit
 * @param size reference to maze size
 */
void renderMenu(unsigned int& size) {
	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	drawText("MAZE GAME!", 300 / 2, 100, 2, Game::textShader);
	drawText("PLAY (P)", 300, 300, 1, Game::textShader);
	drawText("LOAD (L)", 450, 300, 1, Game::textShader);
	drawText("EXIT (E)", 600, 300, 1, Game::textShader);
	drawText(std::format("Size: {} +/-", size).c_str(), 750, 300, 1, Game::textShader);

	processInput(Game::window);
	glfwSwapBuffers(Game::window);
	glfwPollEvents();
}


/**
 * Save the current game state to file
 */
static void saveGame() {
	std::ofstream file(std::filesystem::current_path().append("save.conf"));
	file << Game::level << '\n';
	file << Game::maze->player->getScore() << '\n';
	file << Game::maze->player->getCollectedPeppermints() << '\n';
	file << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - Game::gameTimer).
		count() << '\n';
	file << Game::maze_rng_mt_saved << '\n';
	file << Game::maze_rng_saved << '\n';
	file << Game::rng_mt_saved << '\n';
	file << Game::rng_saved << '\n';
	file << Game::maze->getWidth() << "\n";
	file << Game::maze->player->getX() << "\n";
	file << Game::maze->player->getY() << "\n";
	file.close();
}

/**
 * Close the game. Save game state and free resources
 */
void close() {
	saveGame();
	glDeleteVertexArrays(1, &Game::mazeVAO);
	glDeleteVertexArrays(1, &Game::charVAO);
	glDeleteBuffers(1, &Game::mazeVBO);
	glDeleteBuffers(1, &Game::charVBO);
	glDeleteProgram(Game::lineShader->ID);
	glfwTerminate();
	exit(0);
}
