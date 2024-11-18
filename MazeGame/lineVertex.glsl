#version 330 core
layout (location = 0) in vec3 aPos;

out float height;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    height = aPos.y*10 + aPos.x*10;
}