#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out float zValue;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    zValue = aPos.y; // The y-value in the mesh is the z-value of the function
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
