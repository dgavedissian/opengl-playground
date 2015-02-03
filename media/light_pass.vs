#version 330 core

uniform mat4 worldViewProj;

layout (location = 0) in vec3 position;

void main()
{
    gl_Position = worldViewProj * vec4(position, 1.0);
}
