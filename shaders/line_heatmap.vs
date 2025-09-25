#version 330 core
layout (location = 0) in vec2 aPos;

out float yValue;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    yValue = aPos.y;
    gl_Position = projection * view * vec4(aPos.x, aPos.y, 0.0, 1.0);
}
