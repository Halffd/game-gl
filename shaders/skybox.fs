#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    vec4 envColor = texture(skybox, TexCoords);
    envColor = envColor; // No gamma correction needed for skybox
    FragColor = envColor;
}