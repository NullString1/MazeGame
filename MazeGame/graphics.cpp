#define NOMINMAX
#include "graphics.h"
#include "shader.h"
#include "character.h"
#include <vector>
#include <windows.h>

GLFWwindow* window;
GLuint characterTexture, peppermintTexture;
Shader *lineShader, *characterShader, *goalShader; 
int lineWidth = 10;
unsigned int mazeVAO, charVAO, mazeVBO, charVBO, goalVBO, goalVAO, EBO;
std::chrono::steady_clock::time_point startT, endT;
Maze* maze;

int createWindow() { // https://learnopengl.com/Getting-started/Hello-Window
    if (!glfwInit()) {
        fprintf_s(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(1000, 1000, "MazeGame", NULL, NULL); // 1000x1000 window
    if (!window)
    {   
        fprintf_s(stderr, "Failed to create window with GLFW\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // Get OpenGL functions
    {
		fprintf_s(stderr, "Failed to initialize GLAD\n");
        return -1;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
    glViewport(0, 0, width, height);

	lineShader = new Shader("lineVertex.glsl", "lineFrag.glsl");
    characterShader = new Shader("characterVertex.glsl", "characterFrag.glsl");
	goalShader = new Shader("goalVertex.glsl", "goalFrag.glsl");
    characterTexture = loadDDSTexture("C:\\Users\\bilbo\\source\\repos\\MazeGame\\MazeGame\\character1.DDS");
	peppermintTexture = loadDDSTexture("C:\\Users\\bilbo\\source\\repos\\MazeGame\\MazeGame\\peppermint.DDS");
	setupVAOVBO();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return 0;
}

void setupVAOVBO() {
    glGenVertexArrays(1, &mazeVAO);
	glGenVertexArrays(1, &charVAO);
	glGenVertexArrays(1, &goalVAO);
    glGenBuffers(1, &mazeVBO);
	glGenBuffers(1, &charVBO);
	glGenBuffers(1, &goalVBO);
	glGenBuffers(1, &EBO);

    glBindVertexArray(mazeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mazeVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);   // Position attribute = 0, 2 floats, no normalisation, 
    glEnableVertexAttribArray(0); // Enable vertex attribute

	glBindVertexArray(charVAO);
	glBindBuffer(GL_ARRAY_BUFFER, charVBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);   // Position attribute = 0, 2 floats, no normalisation, 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));     // Texture coords attribute = 0, 2 floats, no normalisation, 
	                                                                                                    //no stride (space between values), offset of previous 3 floats
	glEnableVertexAttribArray(0); // Enable vertex attribute
	glEnableVertexAttribArray(1); // Enable texture attribute

	glBindVertexArray(goalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, goalVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);   // Position attribute = 0, 2 floats, no normalisation, 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));     // Texture coords attribute = 0, 2 floats, no normalisation, 
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
            maze->player->move(maze->player->UP);
            break;
        case GLFW_KEY_S:
            maze->player->move(maze->player->DOWN);
            break;
        case GLFW_KEY_A:
            maze->player->move(maze->player->LEFT);
            break;
        case GLFW_KEY_D:
            maze->player->move(maze->player->RIGHT);
            break;
        }
    }
}

float* hexColour2Float(int hexColour) {
	float r = ((hexColour >> 24) & 0xFF) / 255.0f;
	float g = ((hexColour >> 16) & 0xFF) / 255.0f;
	float b = ((hexColour >> 8) & 0xFF) / 255.0f;
	float a = (hexColour & 0xFF) / 255.0f;
	static float out[4] = { r, g, b, a };
	return out;
}

void drawLines(std::vector<float> lv, unsigned int colour, bool doBuffer) {
    lineShader->use();
    lineShader->setFloat4("colour", hexColour2Float(colour));
    glBindVertexArray(mazeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mazeVBO);
    if (doBuffer) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*lv.size(), lv.data(), GL_STATIC_DRAW);
    }
    glDrawArrays(GL_LINES, 0, (GLsizei) lv.size()/2);
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
    if (strncmp(fileCode, "DDS ", 4) != 0) {
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

void Character::drawCharacter() {
	float x = -0.9f + this->getX() * 0.1f + 0.2f;
	float y = 0.9f - this->getY() * 0.1f;
    float size = 0.1f;
    characterShader->use();
	characterShader->setInt("texture1", 0);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, characterTexture);

    vector<float> vertices = {
        x,          y,          0.0f,       1.0f, 1.0f, // top right
        x,          y - size,   0.0f,       1.0f, 0.0f, // bottom right
        x - size,   y - size,   0.0f,       0.0f, 0.0f, // bottom left
        x - size,   y,          0.0f,       0.0f, 1.0f, // top left

    };

    vector<unsigned int> indices = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glBindVertexArray(charVAO);
	glBindBuffer(GL_ARRAY_BUFFER, charVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Goal::draw() {
    float x = -0.9f + this->getX() * 0.1f + 0.2f;
    float y = 0.9f - this->getY() * 0.1f;
    float size = 0.05f;
    goalShader->use();
    characterShader->setInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, peppermintTexture);

    vector<float> vertices = {
        x - 0.025f,              y - 0.025f,          0.0f,       1.0f, 1.0f, // top right
        x - 0.025f,              y - size - 0.025f,   0.0f,       1.0f, 0.0f, // bottom right
        x - size - 0.025f,       y - size - 0.025f,   0.0f,       0.0f, 0.0f, // bottom left
        x - size - 0.025f,       y - 0.025f,          0.0f,       0.0f, 1.0f, // top left

    };

    vector<unsigned int> indices = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glBindVertexArray(goalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, goalVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Maze::toVertices(vector<float>* vertices) {
	vertices->clear();
    for (unsigned int i = 0; i < this->height; i++) { // maze->height
        for (unsigned int j = 0; j < this->width; j++) { // maze->width
            Cell* cell = this->getCell(j,i);
            float x = -0.9f + j * 0.1f + 0.1f;
            float y = 0.9f - i * 0.1f;
            float length = 0.1f;
            float space = 0.1f;
            std::vector<float> v;
            // draw top edge
            if (cell->getEdge(0)) {
                v = { x, y, x + length, y };
                vertices->insert(vertices->end(), v.begin(), v.end());
            }
            // draw left edge
            if (cell->getEdge(3)) {
                v = { x, y, x, y - length };
                vertices->insert(vertices->end(), v.begin(), v.end());
            }
            // draw bottom edge
            if (cell->getEdge(2)) {
                v = { x, y - space, x + length, y - space };
                vertices->insert(vertices->end(), v.begin(), v.end());
            }
            // draw right edge
            if (cell->getEdge(1)) {
                v = { x + space, y, x + space, y - length };
                vertices->insert(vertices->end(), v.begin(), v.end());
            }

        }
    }
}

int setupGraphics(Maze* _maze) {
	if (createWindow() == -1)
		return -1;
	maze = _maze;
    maze->player->setTexture(characterTexture);
    return 0;
}

void render() {
    static vector<float> v;
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	startT = std::chrono::high_resolution_clock::now();

    if (!maze->doneGenerating){
        maze->generateMaze();
		maze->toVertices(&v);
    }
    else {
        for_each(maze->goals.begin(), maze->goals.end(), [&](Goal* goal) {
            if (goal->isVisible())
                goal->draw();
		});
    }
	drawLines(v, 0xFF0A00FF, !maze->doneGenerating);
	maze->player->drawCharacter();

	processInput(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
	endT = std::chrono::high_resolution_clock::now();
	Sleep(static_cast<DWORD>(std::max<long long>(0, 1000 / 60 - std::chrono::duration_cast<std::chrono::milliseconds>(endT - startT).count()))); // 60fps = 1000/60 = 16.666ms
}

bool shouldClose() {
	return glfwWindowShouldClose(window);
}

void close() {
    glDeleteVertexArrays(1, &mazeVAO);
	glDeleteVertexArrays(1, &charVAO);
    glDeleteBuffers(1, &mazeVBO);
	glDeleteBuffers(1, &charVBO);
    glDeleteProgram(lineShader->ID);
    glfwTerminate();
}