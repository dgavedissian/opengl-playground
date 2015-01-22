#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

out vec2 oTexcoord;

void main()
{
    gl_Position.xyz = position;
    gl_Position.w = 1.0;

    oTexcoord = texcoord;
}
