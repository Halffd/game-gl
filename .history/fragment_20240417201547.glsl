#version 330 core
out vec4 FragColor;

uniform vec4 startColor;
uniform vec4 endColor;

void main()
{
    FragColor = mix(startColor, endColor, gl_FragCoord.y / 800.0);
}