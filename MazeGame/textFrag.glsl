#version 330 core

in vec4 ourColour;
in vec2 TexCoord;

uniform sampler2D fontTexture;

out vec4 fragColor;

void main()
{
	fragColor = vec4(texture(fontTexture, TexCoord).r) * ourColour;
}
