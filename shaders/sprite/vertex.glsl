#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec2 TextureSize;

uniform mat4 model;
uniform mat4 projection;
uniform vec2 uv;
uniform vec2 size;

void main()
{
    // Handle UV coordinates
    TexCoords = (uv.x == 0.0 && uv.y == 0.0) ? vertex.zw : uv;
    
    // Pass texture size to fragment shader
    TextureSize = size;
    
    // Transform vertex position
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}