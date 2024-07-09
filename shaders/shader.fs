#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 mixColor;


vec4 verticalLines(vec2 uv, float size, vec4 color1, vec4 color2) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.x, 2.0);
    return mix(color1, color2, checker);
}

void main()
{
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 res = mix(tex1, tex2, 0.2);
    FragColor = verticalLines(TexCoord, 10.0, vec4(1.0), vec4(0.0));
}