#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 mixColor;


vec4 checkerboard(vec2 uv, float size) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.x + grid.y, 2.0);
    return vec4(checker, checker, checker, 1.0);
}

void main()
{
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 res = mix(tex1, tex2, 0.2);
    FragColor = checkerboard(TexCoord, 10.0);
}