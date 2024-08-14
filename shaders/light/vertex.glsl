#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in vec3 aColor; // New color attribute

out vec2 TexCoord;
out vec3 Color; // Pass color to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Color = aColor; // Pass color to fragment shader
    //gl_Position = vec4(aPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}