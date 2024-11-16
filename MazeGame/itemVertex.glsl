#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texturePos;

out vec2 TextureCoord;
uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(position, 1.0);
	TextureCoord = vec2(texturePos.x, texturePos.y);
}