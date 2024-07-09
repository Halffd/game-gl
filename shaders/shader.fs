#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 mixColor;

vec4 circularGradient(vec2 uv, vec4 color1, vec4 color2) {
    float dist = length(uv - vec2(0.5, 0.5)); // Calculate the distance from the center (0.5, 0.5)
    return mix(color1, color2, dist);
}

void main()
{
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 res = mix(tex1, tex2, 0.2);
    FragColor = circularGradient(TexCoord, res, vec4(0.0, 0.0, 0.0, 1.0));
}