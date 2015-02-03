#version 330 core

in vec2 oTexcoord;

out vec4 colour;

uniform sampler2D gb0;
uniform sampler2D gb1;
uniform sampler2D gb2;

void main()
{
    vec3 diffuse = texture(gb0, oTexcoord).rgb;
    vec3 normal = texture(gb2, oTexcoord).rgb;

    // Perform directional lighting
    float lighting = dot(normal, vec3(1.0, 0.0, 0.0));
    colour = vec4(diffuse * lighting, 1.0);
}
