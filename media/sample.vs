#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

out vec2 oTexcoord;

uniform mat4 modelViewProj;

void main()
{
    gl_Position = modelViewProj * vec4(position, 1.0f);
    oTexcoord = texcoord;
}
