#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "shader.h"
#include <stdio.h>
#include <vector>

unsigned char fontBitmap[512*512];
stbtt_pack_context pc;
stbtt_packedchar cdata[128];
GLuint fontTexture, fontVAO, fontVBO, fontEBO;

void loadFont() {
	FILE* fontFile;
	fopen_s(&fontFile, "C:\\Windows\\Fonts\\Arial.ttf", "rb");
	if (!fontFile) {
		printf("Failed to open font file\n");
		return;
	}
	fseek(fontFile, 0, SEEK_END);
	long size = ftell(fontFile);
	rewind(fontFile);
	unsigned char* fontBuffer = (unsigned char*)malloc(size);
	if (!fontBuffer) {
		printf("Failed to allocate memory for font buffer\n");
		return;
	}
	fread(fontBuffer, 1, size, fontFile);
	fclose(fontFile);

	stbtt_PackBegin(&pc, fontBitmap, 512, 512, 0, 1, NULL);
	//stbtt_PackSetOversampling(&pc, 1, 1);
	stbtt_PackFontRange(&pc, fontBuffer, 0, 32.0, 32, 128, cdata);
	stbtt_PackEnd(&pc);

	free(fontBuffer);

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, fontBitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glGenVertexArrays(1, &fontVAO);
	glBindVertexArray(fontVAO);

	glGenBuffers(1, &fontVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &fontEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fontEBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

//void drawText(const char* text, float x, float y, float scale, Shader* shader) {
//	shader->use();
//	shader->setInt("texture", 1);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, fontTexture);
//	std::vector<float> vertices;
//	for (int i = 0; text[i]; i++) {
//		float x0 = cdata[text[i] - 32].x0;
//		float y0 = cdata[text[i] - 32].y0;
//		float x1 = cdata[text[i] - 32].x1;
//		float y1 = cdata[text[i] - 32].y1;
//		float xoff = cdata[text[i] - 32].xoff2 - cdata[text[i]-32].xoff;
//		float yoff = cdata[text[i] - 32].yoff2 - cdata[text[i]-32].yoff;
//		vertices.insert(vertices.end(), { x,		y,		 0.0f,	1.0f, 1.0f, 1.0f,	x0, y0 });
//		vertices.insert(vertices.end(), { x,		y-yoff,	 0.0f,	1.0f, 1.0f, 1.0f,	x0, y1 });
//		vertices.insert(vertices.end(), { x-xoff,	y-yoff,	 0.0f,	1.0f, 1.0f, 1.0f,	x1, y1 });
//		vertices.insert(vertices.end(), { x-xoff,	y,		 0.0f,	1.0f, 1.0f, 1.0f,	x1, y0 });
//	}
//	std::vector<unsigned int> indices = {
//		0, 1, 3, // first triangle
//		1, 2, 3  // second triangle
//	};
//	glBindVertexArray(fontVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fontEBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
//	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//}

void drawText(const char* text, float x, float y, float scale, Shader* shader) {
	shader->use();
	shader->setInt("fontTexture", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	glm::mat4 projection = glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f, -1.0f, 1.0f);
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
		/*vertices.insert(vertices.end(), {
			x0, y0, 0.0f, 1.0f, 1.0f, 1.0f, q.s0, q.t0,
			x0, y1, 0.0f, 1.0f, 1.0f, 1.0f, q.s0, q.t1,
			x1, y1, 0.0f, 1.0f, 1.0f, 1.0f, q.s1, q.t1,
			x1, y0, 0.0f, 1.0f, 1.0f, 1.0f, q.s1, q.t0
			});*/
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

	glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
}
