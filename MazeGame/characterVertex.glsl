#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texturePos;

out vec2 TextureCoord;

void main()
{
	gl_Position = vec4(position, 1.0);
	TextureCoord = vec2(texturePos.x, texturePos.y);
}