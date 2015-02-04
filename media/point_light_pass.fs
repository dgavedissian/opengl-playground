#version 330 core

out vec4 oColour;

uniform sampler2D gb0;
uniform sampler2D gb1;
uniform sampler2D gb2;
uniform vec2 screenSize;

uniform vec3 lightPos;

uniform float constant;
uniform float linear;
uniform float exponent;

vec2 calcScreenCoord()
{
    return gl_FragCoord.xy / screenSize;
}

void main()
{
    vec2 screenCoord = calcScreenCoord();

    vec3 colour = texture(gb0, screenCoord).rgb;
    vec3 position = texture(gb1, screenCoord).rgb;
    vec3 normal = texture(gb2, screenCoord).rgb;

    // Calculate shading
    vec3 lightDir = lightPos - position;
    float diffuse = max(dot(normal, normalize(lightDir)), 0.0);
    float distance = length(lightDir);
    float attenuation = constant + linear + exponent;
    vec3 result = colour * diffuse / attenuation;

    oColour = vec4(result, 1.0);
}
