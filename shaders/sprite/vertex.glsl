#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

// New uniforms to handle texture offsets and sizes
uniform vec2 textureOffset; // Offset for the texture
uniform vec2 textureSize;   // Size of the texture region to sample

uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = vertex.zw * textureSize + textureOffset;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}