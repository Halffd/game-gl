#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in float zValue;

uniform float zMin;
uniform float zMax;

vec3 heatmap_color(float value) {
    float ratio = (value - zMin) / (zMax - zMin);
    ratio = clamp(ratio, 0.0, 1.0);
    vec3 blue = vec3(0.0, 0.0, 1.0);
    vec3 red = vec3(1.0, 0.0, 0.0);
    return mix(blue, red, ratio);
}

void main()
{
    FragColor = vec4(heatmap_color(zValue), 1.0);
}
