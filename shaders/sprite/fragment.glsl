#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

void main()
{
    // Adjust the TexCoords based on the texture offset and size
    vec2 adjustedTexCoords = TexCoords;

    // Sample the texture with adjusted coordinates
    color = vec4(spriteColor, 1.0) * texture(image, adjustedTexCoords);
}