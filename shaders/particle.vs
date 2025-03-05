#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

<<<<<<< HEAD
uniform mat4 projection;
=======
uniform mat4 projection; // Projection matrix
uniform mat4 view;       // View matrix
>>>>>>> jam/master
uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    TexCoords = vertex.zw;
    ParticleColor = color;
<<<<<<< HEAD
    gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
=======
    gl_Position = projection * view * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
>>>>>>> jam/master
}