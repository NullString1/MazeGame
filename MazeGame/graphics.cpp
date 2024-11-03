#include "graphics.h"
#include "shader.h"
#include <vector>
#include <windows.h>

GLFWwindow* window;
Shader* shader; 
unsigned int VAO, VBO;
int createWindow() { // https://learnopengl.com/Getting-started/Hello-Window
    if (!glfwInit()) {
        fprintf_s(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
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
    glViewport(0, 0, width, height);

	shader = new Shader("lineVertex.glsl", "lineFrag.glsl");
	setupVAOVBO();

    return 0;
}

void setupVAOVBO() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);   // Position attribute = 0, 2 floats, no normalisation, 
                                                                                    //no stride (space between values), no offset before data
    glEnableVertexAttribArray(0); // Enable vertex attribute
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

float* hexColour2Float(int hexColour) {
	float r = ((hexColour >> 24) & 0xFF) / 255.0f;
	float g = ((hexColour >> 16) & 0xFF) / 255.0f;
	float b = ((hexColour >> 8) & 0xFF) / 255.0f;
	float a = (hexColour & 0xFF) / 255.0f;
	static float out[4] = { r, g, b, a };
	return out;
}

void drawLines(std::vector<float> lv) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*lv.size(), lv.data(), GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, (GLsizei) lv.size()/2);
}

vector<float> Maze::toVertices() {
	vector<float> vertices;
    for (unsigned int i = 0; i < this->height; i++) { // maze->height
        for (unsigned int j = 0; j < this->width; j++) { // maze->width
            Cell* cell = this->getCell(i, j);
            float x = -0.9f + j * 0.1f + 0.1f;
            float y = 0.9f - i * 0.1f;
            float length = 0.1f;
            float space = 0.1f;
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
    return vertices;
}

int drawMaze(unsigned int mazeSize, Maze* maze) {
	int lineWidth = 10;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		
		shader->use();
		shader->setFloat4("ourColour", hexColour2Float(0xFF0A00FF));
		drawLines(maze->toVertices());
		
		processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
		Sleep(10);
        if (!maze->doneGenerating)
           maze->generateMaze();
    }

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader->ID);
    glfwTerminate();

    return 0;
}