#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "shader.h"
#include <cstdio>
#include <vector>
#include "font.h"

static unsigned char fontBitmap[512 * 512];
stbtt_pack_context pc;
static stbtt_packedchar cdata[128];
static GLuint fontTexture, fontVAO, fontVBO, fontEBO;


/**
 * Load Arial, create bitmap with font size 32 and store it in fontTexture.
 */
void loadFont() {
	FILE* fontFile;
	fopen_s(&fontFile, R"(C:\Windows\Fonts\Arial.ttf)", "rb");
	if (!fontFile) {
		printf("Failed to open font file\n");
		return;
	}
	fseek(fontFile, 0, SEEK_END);
	const long size = ftell(fontFile);
	rewind(fontFile);
	unsigned char* fontBuffer = static_cast<unsigned char*>(malloc(size));
	if (!fontBuffer) {
		printf("Failed to allocate memory for font buffer\n");
		return;
	}
	fread(fontBuffer, 1, size, fontFile);
	fclose(fontFile);

	stbtt_PackBegin(&pc, fontBitmap, 512, 512, 0, 1, nullptr);
	stbtt_PackSetOversampling(&pc, 1, 1);
	stbtt_PackFontRange(&pc, fontBuffer, 0, 32.0, 32, 128, cdata);
	stbtt_PackEnd(&pc);

	free(fontBuffer);

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, fontBitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenVertexArrays(1, &fontVAO);
	glBindVertexArray(fontVAO);

	glGenBuffers(1, &fontVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &fontEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fontEBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

/**
 * Draw text on screen.
 *
 * @param text text to draw
 * @param x x coordinate
 * @param y y coordinate
 * @param scale text scale
 * @param shader shader to render text
 */
void drawText(const char* text, float x, float y, const float scale, const Shader* shader) {
	shader->use();
	shader->setInt("fontTexture", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	const glm::mat4 projection = glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f, -1.0f, 1.0f);
	shader->setMat4("projection", projection);
	unsigned int indexOffset = 0;

	for (int i = 0; text[i]; i++) {
		if (text[i] < 32 || text[i] >= 128) continue; // Skip non-printable characters

		stbtt_aligned_quad q;
		stbtt_GetPackedQuad(cdata, 512, 512, text[i] - 32, &x, &y, &q, 0);

		float x0 = q.x0 * scale;
		float y0 = q.y0 * scale;
		float x1 = q.x1 * scale;
		float y1 = q.y1 * scale;

		y0 = 1000.0f - y0;
		y1 = 1000.0f - y1;

		vertices.insert(vertices.end(), {
			                x0, y0, 0.0f, 1.0f, 1.0f, 1.0f, q.s0, q.t0,
			                x0, y1, 0.0f, 1.0f, 1.0f, 1.0f, q.s0, q.t1,
			                x1, y1, 0.0f, 1.0f, 1.0f, 1.0f, q.s1, q.t1,
			                x1, y0, 0.0f, 1.0f, 1.0f, 1.0f, q.s1, q.t0
		                });
		indices.insert(indices.end(), {
			               indexOffset, indexOffset + 1, indexOffset + 2,
			               indexOffset, indexOffset + 2, indexOffset + 3
		               });

		indexOffset += 4;
	}

	glBindVertexArray(fontVAO);
	glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fontEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
}
