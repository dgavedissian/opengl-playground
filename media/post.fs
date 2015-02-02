#version 330 core

in vec2 oTexcoord;

out vec4 colour;

uniform sampler2D framebuffer;

void main()
{
    vec4 fb = texture(framebuffer, oTexcoord);
    colour.r = (fb.r * 0.393) + (fb.g * 0.769) + (fb.b * 0.189);
    colour.g = (fb.r * 0.349) + (fb.g * 0.686) + (fb.b * 0.168);    
    colour.b = (fb.r * 0.272) + (fb.g * 0.534) + (fb.b * 0.131);
    colour.a = 1.0;
}
