#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 mixColor;

vec4 l(vec2 uv, float size, vec4 color1, vec4 color2) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.y, 2.0);
    return mix(color1, color2, checker);
}
vec4 grayscale(vec4 color) {
    float average = (color.r + color.g + color.b) / 3.0;
    return vec4(average, average, average, color.a);
}
void main()
{
    vec4 color = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    FragColor = l(TexCoord, 10.0, color, color / 3.0);

}