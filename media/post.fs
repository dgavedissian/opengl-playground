#version 330 core

in vec2 oTexcoord;

out vec4 colour;

uniform sampler2D gb0;
uniform sampler2D gb1;
uniform sampler2D gb2;

void main()
{
    colour = vec4(texture(gb2, oTexcoord).rgb, 1.0);
}
