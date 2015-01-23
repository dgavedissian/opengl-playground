#version 330 core

in vec2 oTexcoord;

out vec4 colour;

uniform sampler2D framebuffer;
uniform float val;

void main()
{
    colour = texture(framebuffer, oTexcoord + vec2(sin(val + oTexcoord.y * 20.0) * 0.01, 0.0));
}
