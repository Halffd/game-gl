#version 330 core
out vec4 FragColor;

in float yValue;

uniform float yMin;
uniform float yMax;

vec3 heatmap_color(float value) {
    float ratio = (value - yMin) / (yMax - yMin);
    ratio = clamp(ratio, 0.0, 1.0);
    vec3 blue = vec3(0.0, 0.0, 1.0);
    vec3 red = vec3(1.0, 0.0, 0.0);
    return mix(blue, red, ratio);
}

void main()
{
    FragColor = vec4(heatmap_color(yValue), 1.0);
}
