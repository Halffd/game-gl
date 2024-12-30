#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    // Sample the texture
    vec4 texColor = texture(sprite, TexCoords);

    // Make the texture greener by multiplying the green channel
    // You can adjust the multiplier to control the intensity of greenness
    float greenMultiplier = 7.5; // Adjust this value as needed
    texColor.g *= greenMultiplier;

    // Combine the texture color with the particle color
    color = texColor * ParticleColor;
}