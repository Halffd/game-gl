#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec4 mixColor;

void main()
{
    vec4 textureColor = texture(ourTexture, TexCoord);

    if (mixColor != vec4(0.0, 0.0, 0.0, 0.0))
    {
        FragColor = mix(textureColor, mixColor, 0.5);
    }
    else
    {
        FragColor = textureColor;
    }
}