#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec3 aColor; // New color attribute

out vec2 TexCoord;
out vec3 Color; // Pass color to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aUV;

    Color = aColor; // Pass color to fragment shader
    //gl_Position = vec4(aPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}