#version 330 core
out vec4 FragColor;

uniform vec3 uniformColor;
in vec3 ourColor;

uniform float mixFactor;

void main()
{
    vec3 mixedColor = mix(uniformColor, ourColor, mixFactor);
    FragColor = vec4(mixedColor, 1.0);
}