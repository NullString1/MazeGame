#version 330 core

out vec4 FragColour;
in float height;

uniform sampler1D texture1;

void main()
{
    FragColour = texture(texture1, height).rgba;
}