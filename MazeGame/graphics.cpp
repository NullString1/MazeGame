#define NOMINMAX
#include <typeinfo>
#include "graphics.h"
#include "font.h"
#include "time.h"
#include <format>

int createWindow() { // https://learnopengl.com/Getting-started/Hello-Window
    if (!glfwInit()) {
        fprintf_s(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Game::window = glfwCreateWindow(1000, 1000, "MazeGame", nullptr, nullptr); // 1000x1000 window
    if (!Game::window)
    {   
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

void loadShaders()
{
    Game::lineShader = new Shader("lineVertex.glsl", "lineFrag.glsl");
    Game::characterShader = new Shader("characterVertex.glsl", "characterFrag.glsl");
    Game::goalShader = new Shader("goalVertex.glsl", "goalFrag.glsl");
    Game::textShader = new Shader("textVertex.glsl", "textFrag.glsl");
}

void loadTextures() {
	Game::characterTexture = loadDDSTexture(R"(C:\Users\bilbo\source\repos\MazeGame\MazeGame\character1.DDS)");
	Game::characterTexture2 = loadDDSTexture(R"(C:\Users\bilbo\source\repos\MazeGame\MazeGame\character2.DDS)");
	Game::peppermintTexture = loadDDSTexture(R"(C:\Users\bilbo\source\repos\MazeGame\MazeGame\peppermint.DDS)");
}

void setupVAOVBO() {
    glGenVertexArrays(1, &Game::mazeVAO);
	glGenVertexArrays(1, &Game::charVAO);
	glGenVertexArrays(1, &Game::goalVAO);

    glGenBuffers(1, &Game::mazeVBO);
	glGenBuffers(1, &Game::charVBO);
	glGenBuffers(1, &Game::goalVBO);
	glGenBuffers(1, &Game::EBO);

    glBindVertexArray(Game::mazeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, Game::mazeVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);   // Position attribute = 0, 2 floats, no normalisation, 

    glEnableVertexAttribArray(0); // Enable vertex attribute

	glBindVertexArray(Game::charVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::charVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);   // Position attribute = 0, 2 floats, no normalisation, 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));     // Texture coords attribute = 0, 2 floats, no normalisation, 
	                                                                                                    //no stride (space between values), offset of previous 3 floats
	glEnableVertexAttribArray(0); // Enable vertex attribute
	glEnableVertexAttribArray(1); // Enable texture attribute

	glBindVertexArray(Game::goalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Game::goalVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);   // Position attribute = 0, 2 floats, no normalisation, 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));     // Texture coords attribute = 0, 2 floats, no normalisation, 

	glEnableVertexAttribArray(0); // Enable vertex attribute
	glEnableVertexAttribArray(1); // Enable texture attribute
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        switch (key)
        {
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
		default: 
			break;
        }
    }
}

float* hexColour2Float(const int hexColour) {
	float r = ((hexColour >> 24) & 0xFF) / 255.0f;
	float g = ((hexColour >> 16) & 0xFF) / 255.0f;
	float b = ((hexColour >> 8) & 0xFF) / 255.0f;
	float a = (hexColour & 0xFF) / 255.0f;
	static float out[4] = { r, g, b, a };
	return out;
}

void drawLines(const std::vector<float>& lv, const unsigned int colour, const bool doBuffer) {
    Game::lineShader->use();
    Game::lineShader->setFloat4("colour", hexColour2Float(colour));
    glBindVertexArray(Game::mazeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, Game::mazeVBO);
    if (doBuffer) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*lv.size(), lv.data(), GL_STATIC_DRAW);
    }
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lv.size())/2);
}

GLuint loadDDSTexture(const char* path) {
    DDS_HEADER fileHeader;
    FILE* fp;

    if (fopen_s(&fp, path, "rb") != 0) {
		fprintf_s(stderr, "Failed to open file\n");
        return -1;
    }

    char fileCode[4];
    fread_s(fileCode, 4, 4, 1, fp);
    if (strncmp(fileCode, R"(DDS )", 4) != 0) {
		fprintf_s(stderr, "Not a DDS file\n");
        fclose(fp);
        return -1;
    }

    fread_s(&fileHeader, sizeof(fileHeader), 124, 1, fp);

    unsigned char* mipMapBuffer;
    unsigned int bufferSize = fileHeader.dwMipMapCount > 1 ? fileHeader.dwPitchOrLinearSize*2 : fileHeader.dwPitchOrLinearSize;
	mipMapBuffer = new unsigned char[bufferSize];
	fread_s(mipMapBuffer,bufferSize, 1, bufferSize, fp);
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int blockSize = (format == 0x83F1) ? 8 : 16;
	unsigned int offset = 0;

	for (unsigned int level = 0; level < fileHeader.dwMipMapCount && (fileHeader.dwWidth || fileHeader.dwHeight); ++level) {
		unsigned int size = ((fileHeader.dwWidth + 3) / 4) * ((fileHeader.dwHeight + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, fileHeader.dwWidth, fileHeader.dwHeight, 0, size, mipMapBuffer + offset);
		offset += size;
		fileHeader.dwWidth /= 2;
		fileHeader.dwHeight /= 2;
	}
    delete[] mipMapBuffer;
	return textureID;
}

inline static void normaliseCoords(const unsigned int x, const unsigned int y, float& _x, float& _y) {
	_x = -0.9f + x * 0.1f + 0.2f;
	_y = 0.9f - y * 0.1f;
}

inline static float* normaliseCoords(const unsigned int x, const unsigned int y) {
	static float out[2];
	out[0] = -0.9f + x * 0.1f + 0.2f;
	out[1] = 0.9f - y * 0.1f;
	return out;
}

void GameObject::draw() {
    float x, y;
    normaliseCoords(this->x, this->y, x, y);
	const bool isGoal = dynamic_cast<Goal*>(this) != nullptr;
	GLuint *VAO, *VBO;
    std::vector<float> vertices;
	vertices.reserve(20);

    glActiveTexture(GL_TEXTURE0);
    

	const float size = this->getSize();
    if (!isGoal) {
		glBindTexture(GL_TEXTURE_2D, dynamic_cast<Character*>(this)->getTexture(rand() %2));
        Game::characterShader->use();
        Game::characterShader->setInt("texture1", 0);
        vertices = {
            x,          y,            0.0f,       1.0f,   1.0f, // top right
            x,          y - size,     0.0f,       1.0f,   0.0f, // bottom right
            x - size,   y - size,     0.0f,       0.0f,   0.0f, // bottom left
            x - size,   y,            0.0f,       0.0f,   1.0f, // top left

        };
		VAO = &Game::charVAO;
		VBO = &Game::charVBO;
    }
    else {
        glBindTexture(GL_TEXTURE_2D, this->getTexture());
		Game::goalShader->use();
		Game::goalShader->setInt("texture1", 0);
        vertices = {
        x - 0.025f,               y - 0.025f,           0.0f,       1.0f, 1.0f, // top right
        x - 0.025f,               y - size - 0.025f,    0.0f,       1.0f, 0.0f, // bottom right
        x - size - 0.025f,        y - size - 0.025f,    0.0f,       0.0f, 0.0f, // bottom left
        x - size - 0.025f,        y - 0.025f,           0.0f,       0.0f, 1.0f, // top left

        };
		VAO = &Game::goalVAO;
		VBO = &Game::goalVBO;
    }

    const std::vector<unsigned int> indices = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Game::EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Maze::toVertices(std::vector<float>& vertices) const
{
	vertices.clear();
    for (unsigned int i = 0; i < this->height; i++) { // maze->height
        for (unsigned int j = 0; j < this->width; j++) { // maze->width
            const Cell* cell = this->getCell(j,i);
            const float x = -0.9f + j * 0.1f + 0.1f;
            const float y = 0.9f - i * 0.1f;
            constexpr float length = 0.1f;
            constexpr float space = 0.1f;
            std::vector<float> v;
            // draw top edge
            if (cell->getEdge(0)) {
                v = { x, y, x + length, y };
                vertices.insert(vertices.end(), v.begin(), v.end());
            }
            // draw left edge
            if (cell->getEdge(3)) {
                v = { x, y, x, y - length };
                vertices.insert(vertices.end(), v.begin(), v.end());
            }
            // draw bottom edge
            if (cell->getEdge(2)) {
                v = { x, y - space, x + length, y - space };
                vertices.insert(vertices.end(), v.begin(), v.end());
            }
            // draw right edge
            if (cell->getEdge(1)) {
                v = { x + space, y, x + space, y - length };
                vertices.insert(vertices.end(), v.begin(), v.end());
            }

        }
    }
}

int setupGraphics(Maze& _maze) {
	if (createWindow() == -1)
		return -1;
	loadFont();
    Game::maze = &_maze;
	Goal::texture = Game::peppermintTexture;
	Character::texture = Game::characterTexture;
	Character::texture2 = Game::characterTexture2;
	Enemy::texture = Game::characterTexture;
    return 0;
}

inline static void drawScore(const Character* chr) {
	drawText(std::format("Score: {}", chr->getScore()).c_str(), 20.0f, 40.0f, 1.0f, Game::textShader);
}

static void drawTimer(std::chrono::steady_clock::time_point& t) {
    auto time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - t).count();
    auto minutes = time / 60;
	auto seconds = time % 60;

    drawText(std::format("Time: {}m {}s", minutes, seconds).c_str(), 140.0f, 40.0f, 1.0f, Game::textShader);
}

void render() {
    static std::vector<float> v;

    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!Game::maze->isDoneGenerating()) {
        Game::maze->generateMaze();
        Game::maze->toVertices(v);
    } else {
        std::ranges::for_each(Game::maze->goals, [&](Goal* goal) {
            if (goal->isVisible())
                goal->draw();
		});
        std::ranges::for_each(Game::maze->enemies, [&](Enemy* enemy) {
            enemy->draw();
			});
    }

	drawLines(v, 0xFF0A00FF, !Game::maze->isDoneGenerating());
    Game::maze->player->draw();
    drawScore(Game::maze->player);
	drawTimer(Game::gameTimer);


	processInput(Game::window);
    glfwSwapBuffers(Game::window);
    glfwPollEvents();
}

void close() {
    glDeleteVertexArrays(1, &Game::mazeVAO);
	glDeleteVertexArrays(1, &Game::charVAO);
    glDeleteBuffers(1, &Game::mazeVBO);
	glDeleteBuffers(1, &Game::charVBO);
    glDeleteProgram(Game::lineShader->ID);
    glfwTerminate();
}