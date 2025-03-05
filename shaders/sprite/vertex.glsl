<<<<<<< HEAD

#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
=======
#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords; // Output texture coordinates

// New uniforms to handle texture offsets and sizes
uniform vec2 textureOffset; // Offset for the texture
uniform vec2 textureSize;   // Size of the texture region to sample

uniform mat4 model;         // Model matrix
uniform mat4 view;          // View matrix
uniform mat4 projection;    // Projection matrix

void main()
{
    // Calculate texture coordinates based on the given offset and size
    TexCoords = vertex.zw * textureSize + textureOffset;

    // Apply model, view, and projection transformations to the vertex position
    gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
>>>>>>> jam/master
}