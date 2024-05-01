#version 330 core
out vec4 FragColor;

uniform vec3 uniformColor;
in vec3 ourColor;
uniform float mixFactor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}