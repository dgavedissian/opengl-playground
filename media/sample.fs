#version 330 core

in vec3 oNormal;
in vec2 oTexcoord;

out vec4 colour;

uniform sampler2D tex;

void main()
{
    float lighting = dot(oNormal, vec3(1.0, 0.0, 0.0));
    colour = texture(tex, oTexcoord) * lighting;
}
